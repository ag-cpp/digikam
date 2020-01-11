/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Onedrive web service
 *
 * Copyright (C) 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "odtalker.h"

// Qt includes

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QByteArray>
#include <QList>
#include <QPair>
#include <QFileInfo>
#include <QWidget>
#include <QMessageBox>
#include <QApplication>
#include <QMimeDatabase>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QNetworkAccessManager>

// KDE includes

#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "wstoolutils.h"
#include "odwindow.h"
#include "oditem.h"
#include "odmpform.h"
#include "webbrowserdlg.h"
#include "previewloadthread.h"

namespace DigikamGenericOneDrivePlugin
{

class Q_DECL_HIDDEN ODTalker::Private
{
public:

    enum State
    {
        OD_USERNAME = 0,
        OD_LISTFOLDERS,
        OD_CREATEFOLDER,
        OD_ADDPHOTO
    };

public:

    explicit Private()
      : clientId(QLatin1String("4c20a541-2ca8-4b98-8847-a375e4d33f34")),
        clientSecret(QLatin1String("wtdcaXADCZ0|tcDA7633|@*")),
        authUrl(QLatin1String("https://login.live.com/oauth20_authorize.srf")),
        tokenUrl(QLatin1String("https://login.live.com/oauth20_token.srf")),
        scope(QLatin1String("Files.ReadWrite User.Read")),
        redirectUrl(QLatin1String("https://login.live.com/oauth20_desktop.srf")),
        serviceName(QLatin1String("Onedrive")),
        serviceTime(QLatin1String("token_time")),
        serviceKey(QLatin1String("access_token")),
        state(OD_USERNAME),
        parent(nullptr),
        netMngr(nullptr),
        reply(nullptr),
        settings(nullptr),
        browser(nullptr)
    {
    }

public:

    QString                         clientId;
    QString                         clientSecret;
    QString                         authUrl;
    QString                         tokenUrl;
    QString                         scope;
    QString                         redirectUrl;
    QString                         accessToken;
    QString                         serviceName;
    QString                         serviceTime;
    QString                         serviceKey;

    QDateTime                       expiryTime;

    State                           state;

    QWidget*                        parent;

    QNetworkAccessManager*          netMngr;
    QNetworkReply*                  reply;

    QSettings*                      settings;

    WebBrowserDlg*                  browser;

    QList<QPair<QString, QString> > folderList;
    QList<QString>                  nextFolder;
};

ODTalker::ODTalker(QWidget* const parent)
    : d(new Private)
{
    d->parent   = parent;
    d->netMngr  = new QNetworkAccessManager(this);
    d->settings = WSToolUtils::getOauthSettings(this);

    connect(this, SIGNAL(oneDriveLinkingFailed()),
            this, SLOT(slotLinkingFailed()));

    connect(this, SIGNAL(oneDriveLinkingSucceeded()),
            this, SLOT(slotLinkingSucceeded()));

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));
}

ODTalker::~ODTalker()
{
    if (d->reply)
    {
        d->reply->abort();
    }

    WSToolUtils::removeTemporaryDir("onedrive");

    delete d;
}

void ODTalker::link()
{
    emit signalBusy(true);

    QUrl url(d->authUrl);
    QUrlQuery query(url);
    query.addQueryItem(QLatin1String("client_id"), d->clientId);
    query.addQueryItem(QLatin1String("scope"), d->scope);
    query.addQueryItem(QLatin1String("redirect_uri"), d->redirectUrl);
    query.addQueryItem(QLatin1String("response_type"), QLatin1String("token"));
    url.setQuery(query);

    delete d->browser;
    d->browser = new WebBrowserDlg(url, d->parent, true);
    d->browser->setModal(true);

    connect(d->browser, SIGNAL(urlChanged(QUrl)),
            this, SLOT(slotCatchUrl(QUrl)));

    connect(d->browser, SIGNAL(closeView(bool)),
            this, SIGNAL(signalBusy(bool)));

    d->browser->show();
}

void ODTalker::unLink()
{
    d->accessToken = QString();

    d->settings->beginGroup(d->serviceName);
    d->settings->remove(QString());
    d->settings->endGroup();

    emit oneDriveLinkingSucceeded();
}

void ODTalker::slotCatchUrl(const QUrl& url)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Received URL from webview:" << url;

    QString   str = url.toString();
    QUrlQuery query(str.section(QLatin1Char('#'), -1, -1));

    if (query.hasQueryItem(QLatin1String("access_token")))
    {
        d->accessToken = query.queryItemValue(QLatin1String("access_token"));
        int seconds    = query.queryItemValue(QLatin1String("expires_in")).toInt();
        d->expiryTime  = QDateTime::currentDateTime().addSecs(seconds);

        writeSettings();

        qDebug(DIGIKAM_WEBSERVICES_LOG) << "Access token received";
        emit oneDriveLinkingSucceeded();
    }
    else
    {
        emit oneDriveLinkingFailed();
    }
}

void ODTalker::slotLinkingFailed()
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to Onedrive fail";
    emit signalBusy(false);
}

void ODTalker::slotLinkingSucceeded()
{
    if (d->accessToken.isEmpty())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "UNLINK to Onedrive";
        emit signalBusy(false);
        return;
    }

    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "LINK to Onedrive";

    if (d->browser)
    {
        d->browser->close();
    }

    emit signalLinkingSucceeded();
}

bool ODTalker::authenticated()
{
    return (!d->accessToken.isEmpty());
}

void ODTalker::cancel()
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    emit signalBusy(false);
}

void ODTalker::createFolder(QString& path)
{
    //path also has name of new folder so send path parameter accordingly
    QString name       = QUrl(path).fileName();
    QString folderPath = QUrl(path).adjusted(QUrl::RemoveFilename |
                                             QUrl::StripTrailingSlash).path();

    QUrl url;

    if (folderPath == QLatin1String("/"))
    {
        url = QUrl(QLatin1String("https://graph.microsoft.com/v1.0/me/drive/root/children"));
    }
    else
    {
        url = QUrl(QString::fromUtf8("https://graph.microsoft.com/v1.0/me/drive/root:/%1:/children").arg(folderPath));
    }

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));
    netRequest.setRawHeader("Authorization", QString::fromLatin1("bearer %1").arg(d->accessToken).toUtf8());

    QByteArray postData = QString::fromUtf8("{\"name\": \"%1\",\"folder\": {}}").arg(name).toUtf8();
    d->reply = d->netMngr->post(netRequest, postData);

    d->state = Private::OD_CREATEFOLDER;
    emit signalBusy(true);
}

void ODTalker::getUserName()
{
    QUrl url(QLatin1String("https://graph.microsoft.com/v1.0/me"));

    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", QString::fromLatin1("bearer %1").arg(d->accessToken).toUtf8());
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));

    d->reply = d->netMngr->get(netRequest);
    d->state = Private::OD_USERNAME;
    emit signalBusy(true);
}

/** Get list of folders by parsing json sent by onedrive
 */
void ODTalker::listFolders(const QString& folder)
{
    QString nextFolder;

    if (folder.isEmpty())
    {
        d->folderList.clear();
        d->nextFolder.clear();
    }
    else
    {
        nextFolder = QLatin1Char(':') + folder + QLatin1Char(':');
    }

    QUrl url(QString::fromLatin1("https://graph.microsoft.com/v1.0/me/drive/root%1/"
                                 "children?select=name,folder,path,parentReference").arg(nextFolder));

    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", QString::fromLatin1("bearer %1").arg(d->accessToken).toUtf8());
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));

    d->reply = d->netMngr->get(netRequest);

    d->state = Private::OD_LISTFOLDERS;
    emit signalBusy(true);
}

bool ODTalker::addPhoto(const QString& imgPath, const QString& uploadFolder, bool rescale, int maxDim, int imageQuality)
{
    if (d->reply)
    {
        d->reply->abort();
        d->reply = nullptr;
    }

    emit signalBusy(true);

    ODMPForm form;
    QString path = imgPath;

    QMimeDatabase mimeDB;

    if (mimeDB.mimeTypeForFile(imgPath).name().startsWith(QLatin1String("image/")))
    {
        QImage image = PreviewLoadThread::loadHighQualitySynchronously(imgPath).copyQImage();

        if (image.isNull())
        {
            emit signalBusy(false);
            return false;
        }

        path = WSToolUtils::makeTemporaryDir("onedrive").filePath(QFileInfo(imgPath)
                                             .baseName().trimmed() + QLatin1String(".jpg"));

        if (rescale && (image.width() > maxDim || image.height() > maxDim))
        {
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        image.save(path, "JPEG", imageQuality);

        DMetadata meta;

        if (meta.load(imgPath))
        {
            meta.setItemDimensions(image.size());
            meta.setItemOrientation(DMetadata::ORIENTATION_NORMAL);
            meta.setMetadataWritingMode((int)DMetadata::WRITE_TO_FILE_ONLY);
            meta.save(path, true);
        }
    }

    if (!form.addFile(path))
    {
        emit signalBusy(false);
        return false;
    }

    QString uploadPath = uploadFolder + QUrl(imgPath).fileName();
    QUrl url(QString::fromLatin1("https://graph.microsoft.com/v1.0/me/drive/root:/%1:/content").arg(uploadPath));

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/octet-stream"));
    netRequest.setRawHeader("Authorization", QString::fromLatin1("bearer {%1}").arg(d->accessToken).toUtf8());

    d->reply = d->netMngr->put(netRequest, form.formData());

    d->state = Private::OD_ADDPHOTO;

    return true;
}

void ODTalker::slotFinished(QNetworkReply* reply)
{
    if (reply != d->reply)
    {
        return;
    }

    d->reply = nullptr;

    if (reply->error() != QNetworkReply::NoError)
    {
        if (d->state != Private::OD_CREATEFOLDER)
        {
            emit signalBusy(false);
            QMessageBox::critical(QApplication::activeWindow(),
                                  i18n("Error"), reply->errorString());

            reply->deleteLater();
            return;
        }
    }

    QByteArray buffer = reply->readAll();

    switch (d->state)
    {
        case Private::OD_LISTFOLDERS:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In OD_LISTFOLDERS";
            parseResponseListFolders(buffer);
            break;
        case Private::OD_CREATEFOLDER:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In OD_CREATEFOLDER";
            parseResponseCreateFolder(buffer);
            break;
        case Private::OD_ADDPHOTO:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In OD_ADDPHOTO";
            parseResponseAddPhoto(buffer);
            break;
        case Private::OD_USERNAME:
            qCDebug(DIGIKAM_WEBSERVICES_LOG) << "In OD_USERNAME";
            parseResponseUserName(buffer);
            break;
        default:
            break;
    }

    reply->deleteLater();
}

void ODTalker::parseResponseAddPhoto(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    bool success           = jsonObject.contains(QLatin1String("size"));
    emit signalBusy(false);

    if (!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

void ODTalker::parseResponseUserName(const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString name      = doc.object()[QLatin1String("displayName")].toString();
    emit signalBusy(false);
    emit signalSetUserName(name);
}

void ODTalker::parseResponseListFolders(const QByteArray& data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError)
    {
        emit signalBusy(false);
        emit signalListAlbumsFailed(i18n("Failed to list folders"));
        return;
    }

    QJsonObject jsonObject = doc.object();
    //qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Json: " << doc;
    QJsonArray jsonArray   = jsonObject[QLatin1String("value")].toArray();

    if (d->folderList.isEmpty())
    {
        d->folderList.append(qMakePair(QLatin1String(""), QLatin1String("root")));
    }

    foreach (const QJsonValue& value, jsonArray)
    {
        QString path;
        QString listName;
        QString folderPath;
        QString folderName;
        QJsonObject folder;
        QJsonObject parent;

        QJsonObject obj = value.toObject();
        folder          = obj[QLatin1String("folder")].toObject();
        parent          = obj[QLatin1String("parentReference")].toObject();

        if (!folder.isEmpty())
        {
            folderPath  = parent[QLatin1String("path")].toString();
            folderName  = obj[QLatin1String("name")].toString();

            path        = folderPath.section(QLatin1String("root:"), -1, -1) +
                                             QLatin1Char('/') + folderName;
            path        = QUrl(path).toString(QUrl::FullyDecoded);
            listName    = path.section(QLatin1Char('/'), 1);

            d->folderList.append(qMakePair(path, listName));

            if (folder[QLatin1String("childCount")].toInt() > 0)
            {
                d->nextFolder << path;
            }
        }
    }

    if (!d->nextFolder.isEmpty())
    {
        listFolders(d->nextFolder.takeLast());
    }
    else
    {
        std::sort(d->folderList.begin(), d->folderList.end());

        emit signalBusy(false);
        emit signalListAlbumsDone(d->folderList);
    }
}

void ODTalker::parseResponseCreateFolder(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    bool fail              = jsonObject.contains(QLatin1String("error"));

    emit signalBusy(false);

    if (fail)
    {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        emit signalCreateFolderFailed(jsonObject[QLatin1String("error_summary")].toString());
    }
    else
    {
        emit signalCreateFolderSucceeded();
    }
}

void ODTalker::writeSettings()
{
    d->settings->beginGroup(d->serviceName);
    d->settings->setValue(d->serviceTime, d->expiryTime);
    d->settings->setValue(d->serviceKey,  d->accessToken);
    d->settings->endGroup();
}

void ODTalker::readSettings()
{
    d->settings->beginGroup(d->serviceName);
    d->expiryTime  = d->settings->value(d->serviceTime).toDateTime();
    d->accessToken = d->settings->value(d->serviceKey).toString();
    d->settings->endGroup();

    if (d->accessToken.isEmpty())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Linking...";
        link();
    }
    else if (QDateTime::currentDateTime() > d->expiryTime)
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Access token has expired";
        d->accessToken = QString();
        link();
    }
    else
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Already Linked";
        emit oneDriveLinkingSucceeded();
    }
}

} // namespace DigikamGenericOneDrivePlugin
