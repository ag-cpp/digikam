/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free dot fr>
 * Copyright (C) 2012-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "jalbumgenerator.h"

// Qt includes

#include <QDir>
#include <QFutureWatcher>
#include <QRegExp>
#include <QStringList>
#include <QtConcurrentMap>
#include <QApplication>
#include <QProcess>
#include <QUrl>
#include <QList>
#include <QTemporaryFile>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "jalbuminfo.h"
#include "jalbumwizard.h"
#include "dprogresswdg.h"
#include "dhistoryview.h"
#include "dfileoperations.h"

namespace Digikam
{

class Q_DECL_HIDDEN JalbumGenerator::Private
{
public:

    explicit Private()
      : that(0),
        info(0),
        warnings(false),
        cancel(false),
        pview(0),
        pbar(0)
    {
    }

    JalbumGenerator* that;
    JalbumInfo*      info;
    QList<QUrl>      urls;

    // State info
    bool              warnings;

    bool              cancel;
    DHistoryView*     pview;
    DProgressWdg*     pbar;

public:

    bool init()
    {
        cancel = false;

        pview->setVisible(true);
        pbar->setVisible(true);

        return true;
    }

    bool createDir(const QString& dirName)
    {
        logInfo(i18n("Create directories"));

        if (!QDir().mkpath(dirName))
        {
            logError(i18n("Could not create folder '%1'", QDir::toNativeSeparators(dirName)));
            return false;
        }

        return true;
    }

    bool createUrlsList(void)
    {
        if (info->m_getOption == JalbumInfo::ALBUMS)
        {
            // Loop over albums selection

            DInfoInterface::DAlbumIDs::ConstIterator albumIt  = info->m_albumList.constBegin();
            DInfoInterface::DAlbumIDs::ConstIterator albumEnd = info->m_albumList.constEnd();

            for (; albumIt != albumEnd ; ++albumIt)
            {
                int id                     = *albumIt;

                // Gather image element list
                QList<QUrl> imageList;

                if (info->m_iface)
                {
                    imageList = info->m_iface->albumsItems(DInfoInterface::DAlbumIDs() << id);
                    urls.append(imageList);
                }
            }
        }
        else
        {
            urls = info->m_imageList;
        }

        return true;
    }

    bool createProjectFiles(QString& projDir)
    {
        logInfo(i18n("Create jAlbum project files"));

        QDir newAlbumDir = QDir(projDir);

        QFile createFile(newAlbumDir.filePath(QString::fromLatin1("albumfiles.txt")));
        if (!createFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            logInfo(i18n("Failed to create project files"));
            return false;
        }

        QTextStream out(&createFile);
        for (QList<QUrl>::ConstIterator it = urls.constBegin(); it != urls.constEnd(); ++it)
        {
            out << (*it).fileName().toLocal8Bit().data() << "\t" << (*it).path().toLocal8Bit().data() << "\n";
        }

        createFile.close();

        QFile settingsFile(newAlbumDir.filePath(QString::fromLatin1("jalbum-settings.jap")));
        if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            logInfo(i18n("Failed to create settings file"));
            return false;
        }

        QTextStream out2(&settingsFile);
        out2 << "#jAlbum Project\n";

        settingsFile.close();

        return true;
    }

    bool launchJalbum(QString& projDir, QString& jarDir)
    {
        logInfo(i18n("Launch jAlbum with new project files"));

        QString javaExecutable;
        QDir jrePath = QFileInfo(jarDir).dir();
        if (jrePath.cd(QString::fromLatin1("jre64/bin/")))
        {
            javaExecutable = jrePath.filePath(QString::fromLatin1("java"));
        }
        else
        {
            javaExecutable = QString::fromLatin1("java");
        }

        QDir newAlbumDir = QDir(projDir);

        QStringList args;
        args.append(QString::fromLatin1("-Xmx400M"));
        args.append(QString::fromLatin1("-jar"));
        args.append(jarDir);
        args.append(QDir::toNativeSeparators(newAlbumDir.filePath(QString::fromLatin1("jalbum-settings.jap"))));

        QProcess process;
        process.setProcessEnvironment(adjustedEnvironmentForAppImage());
        process.startDetached(javaExecutable, args);

        return true;
    }

    void logInfo(const QString& msg)
    {
        pview->addEntry(msg, DHistoryView::ProgressEntry);
    }

    void logError(const QString& msg)
    {
        pview->addEntry(msg, DHistoryView::ErrorEntry);
    }

    void logWarning(const QString& msg)
    {
        pview->addEntry(msg, DHistoryView::WarningEntry);
        warnings = true;
    }
};

// ----------------------------------------------------------------------

JalbumGenerator::JalbumGenerator(JalbumInfo* const info)
    : QObject(),
      d(new Private)
{
    d->that     = this;
    d->info     = info;
    d->warnings = false;

    connect(this, SIGNAL(logWarningRequested(QString)),
            SLOT(logWarning(QString)), Qt::QueuedConnection);
}

JalbumGenerator::~JalbumGenerator()
{
    delete d;
}

bool JalbumGenerator::run()
{
    if (!d->init())
        return false;

    QString destDir = d->info->destUrl().toLocalFile();
    qCDebug(DIGIKAM_GENERAL_LOG) << destDir;

    QString jarDir = d->info->jarUrl().toLocalFile();
    qCDebug(DIGIKAM_GENERAL_LOG) << jarDir;

    QString projDir = destDir + QString::fromLatin1("/") + d->info->imageSelectionTitle();
    qCDebug(DIGIKAM_GENERAL_LOG) << projDir;

    if (!d->createDir(projDir))
        return false;

    bool result = d->createUrlsList();
    if (result)
    {
        result = d->createProjectFiles(projDir);
    }
    if (result)
    {
        result = d->launchJalbum(projDir, jarDir);
    }

    return result;
}

bool JalbumGenerator::warnings() const
{
    return d->warnings;
}

void JalbumGenerator::logWarning(const QString& text)
{
    d->logWarning(text);
}

void JalbumGenerator::slotCancel()
{
    d->cancel = true;
}

void JalbumGenerator::setProgressWidgets(DHistoryView* const pView, DProgressWdg* const pBar)
{
    d->pview = pView;
    d->pbar  = pBar;

    connect(d->pbar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancel()));
}

} // namespace Digikam
