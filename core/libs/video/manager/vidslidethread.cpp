/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "vidslidethread.h"

// Qt include

#include <QDir>
#include <QDateTime>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

VidSlideThread::VidSlideThread(QObject* const parent)
    : ActionThreadBase(parent)
{
    setObjectName(QLatin1String("VidSlideThread"));
}

VidSlideThread::~VidSlideThread()
{
    cancel();
    wait();

    if (m_settings && QDir().exists(m_settings->tempDir))
    {
        QDir(m_settings->tempDir).removeRecursively();
    }
}

void VidSlideThread::prepareFrames(VidSlideSettings* const settings)
{
    m_settings          = settings;

    m_settings->tempDir = m_settings->outputDir + QDir::separator() + QLatin1Char('.')     +
                          QString::number(QDateTime::currentDateTime().toSecsSinceEpoch()) +
                          QDir::separator();

    if (!QDir().mkpath(m_settings->tempDir))
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot create temporary directory:" << m_settings->tempDir;

        Q_EMIT signalMessage(i18n("Error while creating temporary directory %1!", m_settings->tempDir), true);

        return;
    }

    ActionJobCollection collection;

    VidSlideTask* const t = new VidSlideTask(settings);

    connect(t, SIGNAL(signalProgress(int)),
            this, SIGNAL(signalProgress(int)));

    connect(t, SIGNAL(signalMessage(QString,bool)),
            this, SIGNAL(signalMessage(QString,bool)));

    connect(t, SIGNAL(signalDone(bool)),
            this, SLOT(slotEncodeFrames(bool)));

    collection.insert(t, 0);

    appendJobs(collection);
}

void VidSlideThread::slotEncodeFrames(bool prepareDone)
{
    if (!prepareDone)
    {
        Q_EMIT signalDone(false);

        return;
    }

    Q_EMIT signalMessage(i18n("Encoding frames..."), false);

    // TODO: move to a separate thread.

    m_encoder = new FFmpegLauncher(this);
    m_encoder->setSettings(m_settings);

    connect(m_encoder, &ProcessLauncher::signalComplete,
            this, &VidSlideThread::slotEncodeDone);

    m_encoder->encodeFrames();
}

void VidSlideThread::slotEncodeDone(bool timedOut, int exitCode)
{
    bool b = false;

    if (timedOut || (exitCode != 0))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot generate output video" << m_settings->outputFile;
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Output video is" << m_settings->outputFile;
        b = true;
    }

    if (b)
    {
        if (QDir().exists(m_settings->tempDir))
        {
            QDir(m_settings->tempDir).removeRecursively();
        }

        Q_EMIT signalMessage(i18n("Encoding done."), false);
    }
    else
    {
        Q_EMIT signalMessage(i18n("Error while encoding frames!"), true);
    }

    Q_EMIT signalDone(b);
}

} // namespace Digikam

#include "moc_vidslidethread.cpp"
