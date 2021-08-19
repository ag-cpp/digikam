/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a MJPEG frame generator.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mjpegframethread.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QString>
#include <QBuffer>
#include <QApplication>
#include <QIcon>

// Local includes

#include "digikam_debug.h"
#include "previewloadthread.h"
#include "frameutils.h"
#include "vidslidesettings.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegFrameThread::MjpegFrameThread(QObject* const parent)
    : ActionThreadBase(parent)
{
}

MjpegFrameThread::~MjpegFrameThread()
{
    // cancel the thread

    cancel();

    // wait for the thread to finish

    wait();
}

void MjpegFrameThread::createFrameJob(const MjpegStreamSettings& set)
{
    ActionJobCollection collection;

    MjpegFrameTask* const t = new MjpegFrameTask(set);

    connect(t, SIGNAL(signalFrameChanged(QByteArray)),
            this, SIGNAL(signalFrameChanged(QByteArray)));

    collection.insert(t, 0);

    appendJobs(collection);
}

// -----------------------------------------------------------

MjpegFrameTask::MjpegFrameTask(const MjpegStreamSettings& set)
    : ActionJob(nullptr),
      m_set    (set)
{
    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;

    /**
     * NOTE: QIcon depend of X11 under Linux which is not re-rentrant.
     * Load this image here in first from main thread.
     */
    m_broken = QIcon::fromTheme(QLatin1String("view-preview")).pixmap(VidSlideSettings::videoSizeFromType(type)).toImage();
    m_theend = QIcon::fromTheme(QLatin1String("window-close")).pixmap(VidSlideSettings::videoSizeFromType(type)).toImage();
}

MjpegFrameTask::~MjpegFrameTask()
{
}

QByteArray MjpegFrameTask::imageToJPEGArray(const QImage& frame) const
{
    QByteArray outbuf;
    QBuffer buffer(&outbuf);
    buffer.open(QIODevice::WriteOnly);
    frame.save(&buffer, "JPEG", m_set.quality);

    return outbuf;
}

QImage MjpegFrameTask::loadImageFromPreviewCache(const QString& path) const
{
    QImage qimg;
    qCDebug(DIGIKAM_GENERAL_LOG) << "MjpegStream: Generate frame for" << path;

    DImg dimg = PreviewLoadThread::loadHighQualitySynchronously(path);

    if (dimg.isNull())
    {
        // Generate an error frame.

        qimg = m_broken;
        qCWarning(DIGIKAM_GENERAL_LOG) << "MjpegStream: Failed to load" << path;
    }
    else
    {
        // Generate real preview frame.

        qimg = dimg.copyQImage();
    }

    // Resize output image to the wanted dimensions.

    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;
    qimg                           = FrameUtils::makeScaledImage(qimg, VidSlideSettings::videoSizeFromType(type));

    return qimg;
}

void MjpegFrameTask::run()
{
    QImage qiimg;   // Current image in stream.
    QImage qtimg;   // Current transition image.
    QImage qoimg;   // Next image in stream.

    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;
    int imgFrames                  = m_set.delay * m_set.rate;
    bool oneLoopDone               = false;

    TransitionMngr transmngr;
    transmngr.setOutputSize(VidSlideSettings::videoSizeFromType(type));

    EffectMngr effmngr;
    effmngr.setOutputSize(VidSlideSettings::videoSizeFromType(type));
    effmngr.setFrames(imgFrames);               // Ex: 30 frames at 10 img/s => 3 s of effect

    do
    {
        // To stream in loop forever.

        for (int i = 0 ; ((i < m_set.inputImages.count()) && !m_cancel) ; ++i)
        {
            // One loop strem all items one by one from the ordered list

            if ((i == 0) && !oneLoopDone)
            {
                // If we use transition, the first item at the first loop must be merged from a black image.

                qiimg = FrameUtils::makeFramedImage(QString(), VidSlideSettings::videoSizeFromType(type));
            }

            QString ofile;

            if (i < m_set.inputImages.count())
            {
                // The current item to pass to the next stage from a transition

                ofile = m_set.inputImages[i].toLocalFile();
            }

            qoimg      = loadImageFromPreviewCache(ofile);

            // Apply transition between images

            transmngr.setInImage(qiimg);
            transmngr.setOutImage(qoimg);
            transmngr.setTransition(m_set.transition);

            int ttmout = 0;

            do
            {
                // Loop over all stages to make the transition

                qtimg = transmngr.currentFrame(ttmout);

                emit signalFrameChanged(imageToJPEGArray(qtimg));

                QThread::msleep(lround(1000.0 / m_set.rate));
            }
            while ((ttmout != -1) && !m_cancel);

            /*
             * TODO: append OSD overlay on frame generated by effect.
             */

            // Apply effect on frame

            int count  = 0;
            int itmout = 0;
            effmngr.setImage(qoimg);
            effmngr.setEffect(m_set.effect);

            do
            {
                // Loop over all stages to make the effect

                qiimg = effmngr.currentFrame(itmout);

                emit signalFrameChanged(imageToJPEGArray(qiimg));

                count++;

                QThread::msleep(lround(1000.0 / m_set.rate));
            }
            while ((count < imgFrames) && !m_cancel);

            oneLoopDone = true;        // At least one loop is done.
        }
    }
    while (!m_cancel && m_set.loop);

    emit signalFrameChanged(imageToJPEGArray(m_theend));
    qCDebug(DIGIKAM_GENERAL_LOG) << "MjpegStream: end of stream";

    emit signalDone();
}

} // namespace DigikamGenericMjpegStreamPlugin
