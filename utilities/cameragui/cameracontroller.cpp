/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-17
 * Description : digital camera controller
 * 
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// C++ includes.

#include <typeinfo>
#include <cstdio>

// Qt includes.

#include <QMutex>
#include <QWaitCondition>
#include <QVariant>
#include <QImage>
#include <QFile>
#include <QRegExp>

// KDE includes.

#include <klocale.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kio/renamedialog.h>
#include <kstandarddirs.h>

// Local includes.

#include "ddebug.h"
#include "thumbnailsize.h"
#include "imagewindow.h"
#include "gpcamera.h"
#include "umscamera.h"
#include "dmetadata.h"
#include "jpegutils.h"
#include "cameracontroller.h"
#include "cameracontroller.moc"

namespace Digikam
{

class CameraCommand
{
public:

    enum Action
    {
        gp_none = 0,
        gp_connect,
        gp_cancel,
        gp_cameraInformations,
        gp_listfolders,
        gp_listfiles,
        gp_download,
        gp_upload,
        gp_delete,
        gp_lock,
        gp_thumbnail,
        gp_exif,
        gp_open,
        gp_freeSpace,
        gp_preview,
        gp_capture
    };

    Action                 action;
    QMap<QString,QVariant> map;
};

class CameraControllerPriv
{
public:

    CameraControllerPriv()
    {
        close         = false;
        overwriteAll  = false;
        skipAll       = false;
        canceled      = false;
        running       = false;
        downloadTotal = 0;
        parent        = 0;
        timer         = 0;
        camera        = 0;
    }

    bool                    close;
    bool                    overwriteAll;
    bool                    skipAll;
    bool                    canceled;
    bool                    running;

    int                     downloadTotal;

    QWidget                *parent;

    QTimer                 *timer;

    DKCamera               *camera;

    QMutex                  mutex;
    QWaitCondition          condVar;

    QList<CameraCommand*>   commands;
};

CameraController::CameraController(QWidget* parent, 
                                   const QString& title, const QString& model,
                                   const QString& port, const QString& path)
                : QThread(parent)
{
    d = new CameraControllerPriv;
    d->parent        = parent;

    // URL parsing (c) Stephan Kulow
    if (path.startsWith("camera:/"))
    {
        KUrl url(path);
        DDebug() << "path " << path << " " << url <<  " " << url.host() << endl;
        QString xport = url.host();
        if (xport.startsWith("usb:"))
        {
            DDebug() << "xport " << xport << endl;
            QRegExp x = QRegExp("(usb:[0-9,]*)");

            if (x.indexIn(xport) != -1) 
            {
                QString usbport = x.cap(1);
                DDebug() << "USB " << xport << " " << usbport << endl;
                // if ((xport == usbport) || ((count == 1) && (xport == "usb:"))) {
                //   model = xmodel;
                d->camera = new GPCamera(title, url.user(), "usb:", "/");
                // }
            }
        }
    }

    if (!d->camera)
    {
        if (model.toLower() == "directory browse")
            d->camera = new UMSCamera(title, model, port, path);
        else
            d->camera = new GPCamera(title, model, port, path);
    }

    // setup interthread signals

    qRegisterMetaType<GPItemInfo>("GPItemInfo");
    qRegisterMetaType<GPItemInfoList>("GPItemInfoList");

    connect(this, SIGNAL(signalInternalCheckRename(const QString&, const QString&, const QString&, const QString&)),
            this, SLOT(slotCheckRename(const QString&, const QString&, const QString&, const QString&)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(signalInternalDownloadFailed(const QString&, const QString&)),
            this, SLOT(slotDownloadFailed(const QString&, const QString&)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(signalInternalUploadFailed(const QString&, const QString&, const QString&)),
            this, SLOT(slotUploadFailed(const QString&, const QString&, const QString&)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(signalInternalDeleteFailed(const QString&, const QString&)),
            this, SLOT(slotDeleteFailed(const QString&, const QString&)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(signalInternalLockFailed(const QString&, const QString&)),
            this, SLOT(slotLockFailed(const QString&, const QString&)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(signalInternalOpen(const QString&, const QString&, const QString&)),
            this, SLOT(slotOpen(const QString&, const QString&, const QString&)));

    d->running = true;
    start();
}

CameraController::~CameraController()
{
    // clear commands, stop camera
    slotCancel();

    // stop thread
    {
        QMutexLocker lock(&d->mutex);
        d->running = false;
        d->condVar.wakeAll();
    }
    wait();

    delete d->camera;
    delete d;
}

bool CameraController::cameraThumbnailSupport()
{
    return d->camera->thumbnailSupport();
}

bool CameraController::cameraDeleteSupport()
{
    return d->camera->deleteSupport();
}

bool CameraController::cameraUploadSupport()
{
    return d->camera->uploadSupport();
}

bool CameraController::cameraMkDirSupport()
{
    return d->camera->mkDirSupport();
}

bool CameraController::cameraDelDirSupport()
{
    return d->camera->delDirSupport();
}

bool CameraController::cameraCaptureImageSupport()
{
    return d->camera->captureImageSupport();
}

QString CameraController::cameraPath()
{
    return d->camera->path();
}

QString CameraController::cameraTitle()
{
    return d->camera->title();
}

DKCamera::CameraDriverType CameraController::cameraDriverType()
{
    return d->camera->cameraDriverType();
}

void CameraController::slotCancel()
{
    d->canceled = true;
    d->camera->cancel();
    QMutexLocker lock(&d->mutex);
    d->commands.clear();
}

void CameraController::run()
{
    while (d->running)
    {
        CameraCommand *command;

        {
            QMutexLocker lock(&d->mutex);
            if (!d->commands.isEmpty())
            {
                command = d->commands.takeFirst();
            }
            else
            {
                sendBusy(false);
                d->condVar.wait(&d->mutex);
                continue;
            }
        }

        sendBusy(true);
        executeCommand(command);
        delete command;
    }
    sendBusy(false);
}

void CameraController::executeCommand(CameraCommand *cmd)
{
    switch (cmd->action)
    {
        case(CameraCommand::gp_connect):
        {
            sendInfo(i18n("Connecting to camera..."));

            bool result = d->camera->doConnect();

            emit signalConnected(result);

            if (result)
                sendInfo(i18n("Connection established"));
            else
                sendInfo(i18n("Connection failed"));

            break;
        }
        case(CameraCommand::gp_cameraInformations):
        {
            sendInfo(i18n("Getting camera information..."));

            QString summary, manual, about;

            d->camera->cameraSummary(summary);
            d->camera->cameraManual(manual);
            d->camera->cameraAbout(about);

            emit signalCameraInformations(summary, manual, about);
            break;
        }
        case(CameraCommand::gp_freeSpace):
        {
            sendInfo(i18n("Getting camera freespace available..."));
            unsigned long kBSize  = 0;
            unsigned long kBAvail = 0;
            d->camera->getFreeSpace(kBSize, kBAvail);
            emit signalFreeSpace(kBSize, kBAvail);
            break;
        }
        case(CameraCommand::gp_preview):
        {
            sendInfo(i18n("Getting preview..."));
            QImage preview;
            d->camera->getPreview(preview);
            emit signalPreview(preview);
            break;
        }
        case(CameraCommand::gp_capture):
        {
            sendInfo(i18n("Capture picture..."));
            GPItemInfo itemInfo;
            d->camera->capture(itemInfo);
            emit signalUploaded(itemInfo);
            break;
        }
        case(CameraCommand::gp_listfolders):
        {
            sendInfo(i18n("Listing folders..."));

            QStringList folderList;
            folderList.append(d->camera->path());
            d->camera->getAllFolders(d->camera->path(), folderList);

            emit signalFolderList(folderList);
            sendInfo(i18n("Listing folders is complete..."));

            break;
        }
        case(CameraCommand::gp_listfiles):
        {
            QString folder = cmd->map["folder"].toString();

            sendInfo(i18n("Listing files in %1...", folder));

            GPItemInfoList itemsList;
            // setting getImageDimensions to false is a huge speedup for UMSCamera
            if (!d->camera->getItemsInfoList(folder, itemsList, false))
            {
                sendError(i18n("Failed to list files in %1", folder));
            }

            if (!itemsList.isEmpty())
            {
                emit signalFileList(itemsList);
            }

            sendInfo(i18n("Listing files in %1 is complete", folder));

            break;
        }
        case(CameraCommand::gp_thumbnail):
        {
            QString folder = cmd->map["folder"].toString();
            QString file   = cmd->map["file"].toString();

            sendInfo(i18n("Getting thumbnail in progress..."));

            QImage thumbnail;
            d->camera->getThumbnail(folder, file, thumbnail);

            if (!thumbnail.isNull())
            {
                thumbnail = thumbnail.scaled(ThumbnailSize::Huge, ThumbnailSize::Huge,
                                             Qt::KeepAspectRatio);

                emit signalThumbnail(folder, file, thumbnail);
            }

            break;
        }
        case(CameraCommand::gp_exif):
        {
            QString folder = cmd->map["folder"].toString();
            QString file   = cmd->map["file"].toString();

            if ( typeid(*(d->camera)) == typeid(UMSCamera) )
            {
                emit signalExifFromFile(folder, file);
            }
            else
            {
                sendInfo(i18n("Getting EXIF information for %1...", file));

                char* edata = 0;
                int   esize = 0;
                d->camera->getExif(folder, file, &edata, esize);

                if (edata || esize)
                {
                    QByteArray  ba;
                    QDataStream ds(&ba, QIODevice::WriteOnly);
                    ds.writeRawData(edata, esize);
                    delete [] edata;

                    emit signalExifData(ba);
                }
            }
            break;
        }
        case(CameraCommand::gp_download):
        {
            QString   folder            = cmd->map["folder"].toString();
            QString   file              = cmd->map["file"].toString();
            QString   dest              = cmd->map["dest"].toString();

            if (!d->overwriteAll)
            {
                bool      autoRotate        = cmd->map["autoRotate"].toBool();
                bool      fixDateTime       = cmd->map["fixDateTime"].toBool();
                QDateTime newDateTime       = cmd->map["newDateTime"].toDateTime();
                bool      setPhotographerId = cmd->map["setPhotographerId"].toBool();
                QString   author            = cmd->map["author"].toString();
                QString   authorTitle       = cmd->map["authorTitle"].toString();
                bool      setCredits        = cmd->map["setCredits"].toBool();
                QString   credit            = cmd->map["credit"].toString();
                QString   source            = cmd->map["source"].toString();
                QString   copyright         = cmd->map["copyright"].toString();
                bool      convertJpeg       = cmd->map["convertJpeg"].toBool();
                QString   losslessFormat    = cmd->map["losslessFormat"].toString();
                sendInfo(i18n("Downloading file %1...", file));

                // download to a temp file

                emit signalDownloaded(folder, file, GPItemInfo::DownloadStarted);

                KUrl tempURL(dest);
                tempURL = tempURL.upUrl();
                tempURL.addPath(QString(".digikam-camera-tmp1-%1").arg(getpid()));
                QString temp = tempURL.path();

                bool result = d->camera->downloadItem(folder, file, tempURL.path());

                if (result)
                {
                    if (autoRotate)
                    {
                        DDebug() << "Exif autorotate: " << file << " using (" << tempURL.path() << ")" << endl;
                        sendInfo(i18n("EXIF rotating file %1...", file));
                        exifRotate(tempURL.path(), file);
                    }

                    if (fixDateTime || setPhotographerId || setCredits)
                    {
                        sendInfo(i18n("Setting Metadata tags to file %1...", file));
                        DMetadata metadata(tempURL.path());

                        if (fixDateTime)
                            metadata.setImageDateTime(newDateTime, true);

                        if (setPhotographerId)
                            metadata.setImagePhotographerId(author, authorTitle);

                        if (setCredits)
                            metadata.setImageCredits(credit, source, copyright);

                        metadata.applyChanges();
                    }

                    // Convert Jpeg file to lossless format if necessary,
                    // and move converted image to destination.

                    if (convertJpeg && isJpegImage(tempURL.path()))
                    {
                        sendInfo(i18n("Converting %1 to lossless file format...", file));

                        KUrl tempURL2(dest);
                        tempURL2 = tempURL2.upUrl();
                        tempURL2.addPath(QString(".digikam-camera-tmp2-%1").arg(getpid()));
                        temp = tempURL2.path();

                        if (!jpegConvert(tempURL.path(), tempURL2.path(), file, losslessFormat))
                        {
                            // convert failed. delete the temp file
                            unlink(QFile::encodeName(tempURL.path()));
                            unlink(QFile::encodeName(tempURL2.path()));
                            result = false;
                        }
                    }
                }

                if (!d->skipAll)
                {
                    // do UI operation from main thread
                    emit signalInternalCheckRename(folder, file, dest, temp);
                }
            }
            break;
        }
        case(CameraCommand::gp_open):
        {
            QString folder = cmd->map["folder"].toString();
            QString file   = cmd->map["file"].toString();
            QString dest   = cmd->map["dest"].toString();

            sendInfo(i18n("Retrieving file %1 from camera...", file));

            bool result = d->camera->downloadItem(folder, file, dest);

            if (result)
            {
                emit signalInternalOpen(folder, file, dest);
            }
            else
            {
                sendError(i18n("Failed to retrieve file %1 from camera", file));
            }
            break;
        }
        case(CameraCommand::gp_upload):
        {
            QString folder = cmd->map["destFolder"].toString();

            // We will using the same source file name to create the dest file
            // name in camera.
            QString file   = cmd->map["destFile"].toString();

            // The source file path to download in camera.
            QString src    = cmd->map["srcFilePath"].toString();

            sendInfo(i18n("Uploading file %1 to camera...", file));

            GPItemInfo itemsInfo;

            bool result = d->camera->uploadItem(folder, file, src, itemsInfo);

            if (result)
            {
                emit signalUploaded(itemsInfo);
            }
            else
            {
                emit signalInternalUploadFailed(folder, file, src);
            }
            break;
        }
        case(CameraCommand::gp_delete):
        {
            QString folder = cmd->map["folder"].toString();
            QString file   = cmd->map["file"].toString();

            sendInfo(i18n("Deleting file %1...", file));

            bool result = d->camera->deleteItem(folder, file);

            if (result)
            {
                emit signalDeleted(folder, file, true);
            }
            else
            {
                emit signalInternalDeleteFailed(folder, file);
            }
            break;
        }
        case(CameraCommand::gp_lock):
        {
            QString folder = cmd->map["folder"].toString();
            QString file   = cmd->map["file"].toString();
            bool    lock   = cmd->map["lock"].toBool();

            sendInfo(i18n("Toggle lock file %1...", file));

            bool result = d->camera->setLockItem(folder, file, lock);

            if (result)
            {
                emit signalLocked(folder, file, true);
            }
            else
            {
                emit signalInternalLockFailed(folder, file);
            }
            break;
        }
        default:
            DWarning() << " unknown action specified" << endl;
    }
}

void CameraController::sendBusy(bool val)
{
    emit signalBusy(val);
}

void CameraController::sendError(const QString& msg)
{
    emit signalErrorMsg(msg);
}

void CameraController::sendInfo(const QString& msg)
{
    if (!d->canceled)
        emit signalInfoMsg(msg);
}

void CameraController::slotCheckRename(const QString &folder, const QString &file, 
                                       const QString &destination, const QString &temp)
{
    bool skip      = false;
    bool cancel    = false;
    bool overwrite = false;
    QString dest   = destination;

    // Check if dest file already exist.

    if (!d->overwriteAll)
    {
        QFileInfo info(dest);

        while (info.exists())
        {
            if (d->skipAll)
            {
                skip = true;
                break;
            }

            KIO::RenameDialog dlg(d->parent, i18n("Rename File"), 
                                  folder + QString("/") + file, dest,
                                  KIO::RenameDialog_Mode(KIO::M_MULTI | KIO::M_OVERWRITE | KIO::M_SKIP));

            int result = dlg.exec();
            dest       = dlg.newDestUrl().path();

            switch (result)
            {
                case KIO::R_CANCEL:
                {
                    cancel = true;
                    break;
                }
                case KIO::R_SKIP:
                {
                    skip = true;
                    break;
                }
                case KIO::R_AUTO_SKIP:
                {
                    d->skipAll = true;
                    skip       = true;
                    break;
                }
                case KIO::R_OVERWRITE:
                {
                    overwrite = true;
                    break;
                }
                case KIO::R_OVERWRITE_ALL:
                {
                    d->overwriteAll = true;
                    overwrite       = true;
                    break;
                }
                default:
                    break;
            }

            if (cancel || skip || overwrite)
                break;
        }
    }

    if (cancel)
    {
        unlink(QFile::encodeName(temp));
        slotCancel();
        emit signalSkipped(folder, file);
        return;
    }
    else if (skip)
    {
        unlink(QFile::encodeName(temp));
        emit signalInfoMsg(i18n("Skipped file %1", file));
        emit signalSkipped(folder, file);        
        return;
    }
    
    // move the file to the destination file
    if (rename(QFile::encodeName(temp), QFile::encodeName(dest)) != 0)
    {
        // rename failed. delete the temp file
        unlink(QFile::encodeName(temp));
        emit signalDownloaded(folder, file, GPItemInfo::DownloadFailed);
    }
    else
    {
        emit signalDownloaded(folder, file, GPItemInfo::DownloadedYes);
    }
}

void CameraController::slotDownloadFailed(const QString &folder, const QString &file)
{
    Q_UNUSED(folder);
    QString msg = i18n("Failed to download file \"%1\".", file);

    if (!d->canceled)
    {
        if (queueIsEmpty())
        {
            KMessageBox::error(d->parent, msg);
        }
        else
        {
            msg += i18n(" Do you want to continue?");
            int result = KMessageBox::warningContinueCancel(d->parent, msg);
            if (result != KMessageBox::Continue)
                slotCancel();
        }
    }
}

void CameraController::slotUploadFailed(const QString &folder, const QString &file, const QString &src)
{
    Q_UNUSED(folder);
    Q_UNUSED(src);

    QString msg = i18n("Failed to upload file \"%1\".",file);

    if (!d->canceled)
    {
        if (queueIsEmpty())
        {
            KMessageBox::error(d->parent, msg);
        }
        else
        {
            msg += i18n(" Do you want to continue?");
            int result = KMessageBox::warningContinueCancel(d->parent, msg);
            if (result != KMessageBox::Continue)
                slotCancel();
        }
    }
}

void CameraController::slotDeleteFailed(const QString &folder, const QString &file)
{
    emit signalDeleted(folder, file, false);

    QString msg = i18n("Failed to delete file \"%1\".",file);

    if (!d->canceled)
    {
        if (queueIsEmpty())
        {
            KMessageBox::error(d->parent, msg);
        }
        else
        {
            msg += i18n(" Do you want to continue?");
            int result = KMessageBox::warningContinueCancel(d->parent, msg);
            if (result != KMessageBox::Continue)
                slotCancel();
        }
    }
}

void CameraController::slotLockFailed(const QString &folder, const QString &file)
{
    emit signalLocked(folder, file, false);

    QString msg = i18n("Failed to toggle lock file \"%1\".",file);

    if (!d->canceled)
    {
        if (queueIsEmpty())
        {
            KMessageBox::error(d->parent, msg);
        }
        else
        {
            msg += i18n(" Do you want to continue?");
            int result = KMessageBox::warningContinueCancel(d->parent, msg);
            if (result != KMessageBox::Continue)
                slotCancel();
        }
    }
}

void CameraController::slotOpen(const QString &folder, const QString &file, const QString &dest)
{
    Q_UNUSED(folder);
    Q_UNUSED(file);

    KUrl url = KUrl::fromPath(dest);
    KUrl::List urlList;
    urlList << url;

    ImageWindow *im = ImageWindow::imagewindow();
    im->loadURL(urlList, url, i18n("Camera \"%1\"",d->camera->model()), false);

    if (im->isHidden())
        im->show();
    else
        im->raise();

    im->setFocus();
}

void CameraController::addCommand(CameraCommand *cmd)
{
    QMutexLocker lock(&d->mutex);
    d->commands << cmd;
    d->condVar.wakeAll();
}

bool CameraController::queueIsEmpty()
{
    QMutexLocker lock(&d->mutex);
    return d->commands.isEmpty();
}

void CameraController::slotConnect()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_connect;
    addCommand(cmd);
}

void CameraController::listFolders()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_listfolders;
    addCommand(cmd);
}

void CameraController::listFiles(const QString& folder)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_listfiles;
    cmd->map.insert("folder", QVariant(folder));
    addCommand(cmd);
}

void CameraController::getThumbnail(const QString& folder, const QString& file)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_thumbnail;
    cmd->map.insert("folder", QVariant(folder));
    cmd->map.insert("file", QVariant(file));
    addCommand(cmd);
}

void CameraController::getExif(const QString& folder, const QString& file)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_exif;
    cmd->map.insert("folder", QVariant(folder));
    cmd->map.insert("file", QVariant(file));
    addCommand(cmd);
}

void CameraController::getCameraInformations()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_cameraInformations;
    addCommand(cmd);
}

void CameraController::getFreeSpace()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_freeSpace;
    addCommand(cmd);
}

void CameraController::getPreview()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_preview;
    addCommand(cmd);
}

void CameraController::capture()
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_capture;
    addCommand(cmd);
}

void CameraController::upload(const QFileInfo& srcFileInfo, const QString& destFile, const QString& destFolder)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_upload;
    cmd->map.insert("srcFilePath", QVariant(srcFileInfo.filePath()));
    cmd->map.insert("destFile", QVariant(destFile));
    cmd->map.insert("destFolder", QVariant(destFolder));
    addCommand(cmd);
    DDebug() << "Uploading '" << srcFileInfo.filePath() << "' into camera : '" << destFolder << 
                 "' (" << destFile << ")" << endl;
}

void CameraController::downloadPrep()
{
    d->overwriteAll  = false;
    d->skipAll       = false;
    d->downloadTotal = 0;
}

void CameraController::download(DownloadSettingsContainer downloadSettings)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_download;
    cmd->map.insert("folder", QVariant(downloadSettings.folder));
    cmd->map.insert("file", QVariant(downloadSettings.file));
    cmd->map.insert("dest", QVariant(downloadSettings.dest));
    cmd->map.insert("autoRotate", QVariant(downloadSettings.autoRotate));
    cmd->map.insert("fixDateTime", QVariant(downloadSettings.fixDateTime));
    cmd->map.insert("newDateTime", QVariant(downloadSettings.newDateTime));
    cmd->map.insert("setPhotographerId", QVariant(downloadSettings.setPhotographerId));
    cmd->map.insert("author", QVariant(downloadSettings.author));
    cmd->map.insert("authorTitle", QVariant(downloadSettings.authorTitle));
    cmd->map.insert("setCredits", QVariant(downloadSettings.setCredits));
    cmd->map.insert("credit", QVariant(downloadSettings.credit));
    cmd->map.insert("source", QVariant(downloadSettings.source));
    cmd->map.insert("copyright", QVariant(downloadSettings.copyright));
    cmd->map.insert("convertJpeg", QVariant(downloadSettings.convertJpeg));
    cmd->map.insert("losslessFormat", QVariant(downloadSettings.losslessFormat));
    addCommand(cmd);
}

void CameraController::deleteFile(const QString& folder, const QString& file)
{
    d->canceled        = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action        = CameraCommand::gp_delete;
    cmd->map.insert("folder", QVariant(folder));
    cmd->map.insert("file", QVariant(file));
    addCommand(cmd);
}

void CameraController::lockFile(const QString& folder, const QString& file, bool locked)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_lock;
    cmd->map.insert("folder", QVariant(folder));
    cmd->map.insert("file", QVariant(file));
    cmd->map.insert("lock", QVariant(locked));
    addCommand(cmd);
}

void CameraController::openFile(const QString& folder, const QString& file)
{
    d->canceled = false;
    CameraCommand *cmd = new CameraCommand;
    cmd->action = CameraCommand::gp_open;
    cmd->map.insert("folder", QVariant(folder));
    cmd->map.insert("file", QVariant(file));
    cmd->map.insert("dest", QVariant(KStandardDirs::locate("tmp", file)));
    addCommand(cmd);
}

}  // namespace Digikam
