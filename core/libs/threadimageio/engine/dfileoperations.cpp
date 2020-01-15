/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-10
 * Description : misc file operation methods
 *
 * Copyright (C) 2014-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "dfileoperations.h"
#include "digikam_config.h"

// C ANSI includes

#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>

// Qt includes

#include <QByteArray>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDesktopServices>
#include <QFileInfo>
#include <qplatformdefs.h>

#ifdef HAVE_DBUS
#   include <QDBusInterface>
#   include <QDBusPendingCall>
#endif

// KDE includes

#include <kmimetypetrader.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "metaenginesettings.h"

namespace Digikam
{

bool DFileOperations::localFileRename(const QString& source,
                                      const QString& orgPath,
                                      const QString& destPath,
                                      bool ignoreSettings)
{
    QString dest = destPath;

    // check that we're not replacing a symlink

    QFileInfo info(dest);

    if (info.isSymLink())
    {
        dest = info.symLinkTarget();

        qCDebug(DIGIKAM_GENERAL_LOG) << "Target filePath" << destPath
                                     << "is a symlink pointing to" << dest
                                     << ". Storing image there.";
    }

#ifndef Q_OS_WIN

    QByteArray dstFileName = QFile::encodeName(dest).constData();

    // Store old permissions:
    // Just get the current umask.

    mode_t curr_umask = umask(S_IREAD | S_IWRITE);

    // Restore the umask.

    umask(curr_umask);

    // For new files respect the umask setting.

    mode_t filePermissions = (S_IREAD | S_IWRITE | S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP) & ~curr_umask;

    // For existing files, use the mode of the original file.

    QT_STATBUF stbuf;

    if (QT_STAT(dstFileName.constData(), &stbuf) == 0)
    {
        filePermissions = stbuf.st_mode;
    }

#endif // Q_OS_WIN

    QT_STATBUF st;

    if (QT_STAT(QFile::encodeName(source).constData(), &st) == 0)
    {
        // See bug #329608: Restore file modification time from original file
        // only if updateFileTimeStamp for Setup/Metadata is turned off.

        if (!ignoreSettings && !MetaEngineSettings::instance()->settings().updateFileTimeStamp)
        {
            struct utimbuf ut;
            ut.modtime = st.st_mtime;
            ut.actime  = st.st_atime;

            if (::utime(QFile::encodeName(orgPath).constData(), &ut) != 0)
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore modification time for file"
                                               << dest;
            }
        }
    }

    // remove dest file if it exist

    if (orgPath != dest && QFile::exists(orgPath) && QFile::exists(dest))
    {
        QFile::remove(dest);
    }

    // rename tmp file to dest

    if (!renameFile(orgPath, dest))
    {
        return false;
    }

#ifndef Q_OS_WIN

    // restore permissions

    if (::chmod(dstFileName.constData(), filePermissions) != 0)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore file permissions for file"
                                       << dstFileName;
    }

#endif // Q_OS_WIN

    return true;
}

void DFileOperations::openFilesWithDefaultApplication(const QList<QUrl>& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    foreach (const QUrl& url, urls)
    {
        QDesktopServices::openUrl(url);
    }
}

QUrl DFileOperations::getUniqueFileUrl(const QUrl& orgUrl,
                                       bool* const newurl)
{
    if (newurl)
    {
        *newurl = false;
    }

    int counter = 0;
    QUrl destUrl(orgUrl);
    QFileInfo fi(destUrl.toLocalFile());
    QRegExp version(QLatin1String("(.+)_v(\\d+)"));
    QString completeBaseName = fi.completeBaseName();

    if (version.exactMatch(completeBaseName))
    {
        completeBaseName = version.cap(1);
        counter          = version.cap(2).toInt();
    }

    if (fi.exists())
    {
        bool fileFound = false;

        do
        {
            QFileInfo nfi(destUrl.toLocalFile());

            if (!nfi.exists())
            {
                fileFound = false;

                if (newurl)
                {
                    *newurl = true;
                }
            }
            else
            {
                fileFound = true;
                destUrl   = destUrl.adjusted(QUrl::RemoveFilename);
                destUrl.setPath(destUrl.path() + completeBaseName +
                                QString::fromUtf8("_v%1.").arg(++counter) + fi.completeSuffix());
            }
        }
        while (fileFound);
    }

    return destUrl;
}

bool DFileOperations::runFiles(KService* const service,
                               const QList<QUrl>& urls)
{
    return (runFiles(service->exec(), urls, service));
}

bool DFileOperations::runFiles(const QString& appCmd,
                               const QList<QUrl>& urls,
                               KService* const service)
{
    QRegExp split(QLatin1String(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
    QStringList cmdList = appCmd.split(split, QString::SkipEmptyParts);
    QList<QUrl> urlList = urls;

    if (cmdList.isEmpty() || urlList.isEmpty())
    {
        return false;
    }

    if (!appCmd.contains(QLatin1String("%f"), Qt::CaseInsensitive) &&
        !appCmd.contains(QLatin1String("%u"), Qt::CaseInsensitive) &&
        !appCmd.contains(QLatin1String("%d"), Qt::CaseInsensitive))
    {
        cmdList << QLatin1String("%f");
    }

    QString exec;
    QString name;
    QString icon;
    QString term;

    QStringList dirs;
    QStringList files;
    QStringList cmdArgs;
    QStringList termOpts;

    bool useTerminal = false;
    bool openNewRun  = false;

    if (service)
    {
        name = service->desktopEntryName();
        icon = service->icon();

#ifdef Q_OS_LINUX

        if (service->terminal())
        {
            termOpts = service->terminalOptions().split(split, QString::SkipEmptyParts);
            term     = QStandardPaths::findExecutable(QLatin1String("konsole"));

            if (term.isEmpty())
            {
                term = QStandardPaths::findExecutable(QLatin1String("xterm"));
                termOpts.replaceInStrings(QLatin1String("--noclose"),
                                          QLatin1String("-hold"));
            }

            useTerminal = !term.isEmpty();
        }

#endif // Q_OS_LINUX

    }

    QProcess* const process = new QProcess();
    QProcessEnvironment env = adjustedEnvironmentForAppImage();

    foreach (const QUrl& url, urlList)
    {
        dirs  << url.adjusted(QUrl::RemoveFilename).toLocalFile();
        files << url.toLocalFile();
    }

    foreach (const QString& cmdString, cmdList)
    {
        QString cmd = cmdString;

        if (cmd.startsWith(QLatin1Char('"')) && cmd.endsWith(QLatin1Char('"')))
        {
            cmd.remove(0, 1).chop(1);
        }

        if (exec.isEmpty() && cmd.contains(QLatin1Char('=')))
        {
            QStringList envList = cmd.split(QLatin1Char('='), QString::SkipEmptyParts);

            if (envList.count() == 2)
            {
                env.insert(envList[0], envList[1]);
            }

            continue;
        }
        else if (exec.isEmpty())
        {
            exec = cmd;
            continue;
        }

        if      (cmd == QLatin1String("%c"))
        {
            cmdArgs << name;
        }
        else if (cmd == QLatin1String("%i"))
        {
            cmdArgs << icon;
        }
        else if (cmd == QLatin1String("%f"))
        {
            cmdArgs << files.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%F"))
        {
            cmdArgs << files;
        }
        else if (cmd == QLatin1String("%u"))
        {
            cmdArgs << files.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%U"))
        {
            cmdArgs << files;
        }
        else if (cmd == QLatin1String("%d"))
        {
            cmdArgs << dirs.first();
            openNewRun = true;
        }
        else if (cmd == QLatin1String("%D"))
        {
            cmdArgs << dirs;
        }
        else
        {
            cmdArgs << cmd;
        }
    }

    process->setProcessEnvironment(env);

    if (useTerminal)
    {
        termOpts << QLatin1String("-e") << exec << cmdArgs;
        process->start(term, termOpts);
    }
    else
    {
        process->start(exec, cmdArgs);
    }

    bool ret = true;
    ret     &= process->waitForStarted();

    if (openNewRun)
    {
        urlList.removeFirst();

        if (!urlList.isEmpty())
        {
            ret &= runFiles(appCmd, urlList, service);
        }
    }

    return ret;
}

KService::List DFileOperations::servicesForOpenWith(const QList<QUrl>& urls)
{
    // This code is inspired by KonqMenuActions:
    // kdebase/apps/lib/konq/konq_menuactions.cpp

    QStringList    mimeTypes;
    KService::List offers;

    foreach (const QUrl& item, urls)
    {
        const QString mimeType = QMimeDatabase().mimeTypeForFile(item.toLocalFile(),
                                                                 QMimeDatabase::MatchExtension).name();

        if (!mimeTypes.contains(mimeType))
        {
            mimeTypes << mimeType;
        }
    }

    if (!mimeTypes.isEmpty())
    {
        // Query trader

        const QString firstMimeType      = mimeTypes.takeFirst();
        const QString constraintTemplate = QLatin1String("'%1' in ServiceTypes");
        QStringList constraints;

        foreach (const QString& mimeType, mimeTypes)
        {
            constraints << constraintTemplate.arg(mimeType);
        }

        offers = KMimeTypeTrader::self()->query(firstMimeType,
                                                QLatin1String("Application"),
                                                constraints.join(QLatin1String(" and ")));

        // remove duplicate service entries

        QSet<QString> seenApps;

        for (KService::List::iterator it = offers.begin(); it != offers.end();)
        {
            const QString appName((*it)->name());

            if (!seenApps.contains(appName))
            {
                seenApps.insert(appName);
                ++it;
            }
            else
            {
                it = offers.erase(it);
            }
        }
    }

    return offers;
}

void DFileOperations::openInFileManager(const QList<QUrl>& urls)
{
    if (urls.isEmpty())
    {
        return;
    }

    bool equal = true;
    QUrl first = urls.first();
    first      = first.adjusted(QUrl::RemoveFilename);

    foreach (const QUrl& url, urls)
    {
        if (first != url.adjusted(QUrl::RemoveFilename))
        {
            equal = false;
            break;
        }
    }

    QList<QUrl> fileUrls;

    if (equal)
    {
        fileUrls = urls;
    }
    else
    {
        fileUrls = QList<QUrl>() << urls.first();
    }

    QString path = fileUrls.first().toLocalFile();

#ifdef Q_OS_WIN

    QStringList args;
    QFileInfo info(path);

    if (!info.isDir())
    {
        args << QLatin1String("/select,");
    }

    args << QDir::toNativeSeparators(path);

    if (QProcess::startDetached(QLatin1String("explorer"), args))
    {
        return;
    }

#elif defined Q_OS_OSX

    QStringList args;
    args << QLatin1String("-e");
    args << QLatin1String("tell application \"Finder\"");
    args << QLatin1String("-e");
    args << QLatin1String("activate");
    args << QLatin1String("-e");
    args << QString::fromUtf8("select POSIX file \"%1\"").arg(path);
    args << QLatin1String("-e");
    args << QLatin1String("end tell");
    args << QLatin1String("-e");
    args << QLatin1String("return");

    if (QProcess::execute(QLatin1String("/usr/bin/osascript"), args) == 0)
    {
        return;
    }

#elif HAVE_DBUS

    QDBusInterface iface(QLatin1String("org.freedesktop.FileManager1"),
                         QLatin1String("/org/freedesktop/FileManager1"),
                         QLatin1String("org.freedesktop.FileManager1"),
                         QDBusConnection::sessionBus());

    if (iface.isValid())
    {
        QStringList uris;

        foreach (const QUrl& url, fileUrls)
        {
            uris << url.toString();
        }

        iface.asyncCall(QLatin1String("ShowItems"), uris, QString());

        return;
    }

#endif

    QUrl url = fileUrls.first();
    url      = url.adjusted(QUrl::RemoveFilename |
                            QUrl::StripTrailingSlash);

    QDesktopServices::openUrl(url);
}

bool DFileOperations::copyFolderRecursively(const QString& srcPath,
                                            const QString& dstPath,
                                            const bool* cancel)
{
    QDir srcDir(srcPath);
    QString newCopyPath = dstPath + QLatin1Char('/') + srcDir.dirName();

    if (!srcDir.mkpath(newCopyPath))
    {
        return false;
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Files))
    {
        QString copyPath = newCopyPath + QLatin1Char('/') + fileInfo.fileName();

        if (cancel && *cancel)
        {
            return false;
        }

        if (!copyFile(fileInfo.filePath(), copyPath))
        {
            return false;
        }
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (!copyFolderRecursively(fileInfo.filePath(), newCopyPath, cancel))
        {
            return false;
        }
    }

    return true;
}

bool DFileOperations::copyFiles(const QStringList& srcPaths,
                                const QString& dstPath)
{
    foreach (const QString& path, srcPaths)
    {
        QFileInfo fileInfo(path);
        QString copyPath = dstPath + QLatin1Char('/') + fileInfo.fileName();

        if (!copyFile(fileInfo.filePath(), copyPath))
        {
            return false;
        }
    }

    return true;
}

bool DFileOperations::renameFile(const QString& srcFile,
                                 const QString& dstFile)
{
    QT_STATBUF st;
    int stat = QT_STAT(QFile::encodeName(srcFile).constData(), &st);

    bool ret = QFile::rename(srcFile, dstFile);

    if (ret && (stat == 0))
    {
        struct utimbuf ut;
        ut.modtime = st.st_mtime;
        ut.actime  = st.st_atime;

        if (::utime(QFile::encodeName(dstFile).constData(), &ut) != 0)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore modification time for file"
                                           << dstFile;
        }
    }

    return ret;
}

bool DFileOperations::copyFile(const QString& srcFile,
                               const QString& dstFile)
{
    QT_STATBUF st;
    QString tmpFile;

    int stat = QT_STAT(QFile::encodeName(srcFile).constData(), &st);
    int path = dstFile.lastIndexOf(QLatin1Char('/'));
    int dot  = dstFile.lastIndexOf(QLatin1Char('.'));
    dot      = (path > dot) ? -1 : dot;
    int ext  = dstFile.length() - dot;
    tmpFile  = dstFile.left(dot);
    tmpFile += QLatin1String(".digikamtempfile");
    tmpFile += (dot < 0) ? QLatin1String(".tmp")
                         : dstFile.right(ext);

    bool ret = QFile::copy(srcFile, tmpFile);

    if (ret && !(ret = QFile::rename(tmpFile, dstFile)))
    {
        QFile::remove(tmpFile);
    }

    if (ret && (stat == 0))
    {
        struct utimbuf ut;
        ut.modtime = st.st_mtime;
        ut.actime  = st.st_atime;

        if (::utime(QFile::encodeName(dstFile).constData(), &ut) != 0)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to restore modification time for file"
                                           << dstFile;
        }
    }

    return ret;
}

} // namespace Digikam
