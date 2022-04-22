/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include "VideoPreviewWidget.h"

// Qt includes

#include <QResizeEvent>

// Local includes

#include "VideoFrameExtractor.h"
#include "VideoOutput.h"

namespace QtAV
{

VideoPreviewWidget::VideoPreviewWidget(QWidget *parent)
    : QWidget(parent)
    , m_keep_ar(false)
    , m_auto_display(false) // set to false initially to trigger connections in setAutoDisplayFrame() below -- will default to true
    , m_extractor(new VideoFrameExtractor(this))
    , m_out(new VideoOutput(VideoRendererId_Widget, this))

    // FIXME: opengl may crash, so use software renderer here
{
    setWindowFlags(Qt::FramelessWindowHint);
    Q_ASSERT_X(m_out->widget(), "VideoPreviewWidget()", "widget based renderer is not found");
    m_out->widget()->setParent(this);
    connect(m_extractor, SIGNAL(positionChanged()), this, SIGNAL(timestampChanged()));
    connect(m_extractor, SIGNAL(error(const QString &)), this, SIGNAL(gotError(const QString &)));
    connect(m_extractor, SIGNAL(aborted(const QString &)), this, SIGNAL(gotAbort(const QString &)));
    m_extractor->setAutoExtract(false);
    m_auto_display = false;
    setAutoDisplayFrame(true); // set up frame-related connections, defaulting autoDisplayFrame to true
}

void VideoPreviewWidget::setAutoDisplayFrame(bool b)
{
    if (!!b == !!m_auto_display) return; // avoid reduntant connect/disconnect calls

    if (b)
    {
        connect(m_extractor, SIGNAL(frameExtracted(QtAV::VideoFrame)),
                this, SLOT(displayFrame(QtAV::VideoFrame)));

        connect(m_extractor, SIGNAL(error(const QString &)),
                this, SLOT(displayNoFrame()));

        connect(m_extractor, SIGNAL(aborted(const QString &)),
                this, SLOT(displayNoFrame()));

        connect(this, SIGNAL(fileChanged()),
                this, SLOT(displayNoFrame()));
    }
    else
    {
        disconnect(m_extractor, SIGNAL(frameExtracted(QtAV::VideoFrame)),
                   this, SLOT(displayFrame(QtAV::VideoFrame)));

        disconnect(m_extractor, SIGNAL(error(const QString &)),
                   this, SLOT(displayNoFrame()));

        disconnect(m_extractor, SIGNAL(aborted(const QString &)),
                   this, SLOT(displayNoFrame()));

        disconnect(this, SIGNAL(fileChanged()),
                   this, SLOT(displayNoFrame()));
    }
}

void VideoPreviewWidget::resizeEvent(QResizeEvent *e)
{
    m_out->widget()->resize(e->size());
}

void VideoPreviewWidget::setTimestamp(qint64 value)
{
    m_extractor->setPosition(value);
}

qint64 VideoPreviewWidget::timestamp() const
{
    return m_extractor->position();
}

void VideoPreviewWidget::preview()
{
    m_extractor->extract();
}

void VideoPreviewWidget::setFile(const QString &value)
{
    if (m_file == value)
        return;

    m_file = value;
    m_extractor->setSource(m_file);

    Q_EMIT fileChanged();
}

QString VideoPreviewWidget::file() const
{
    return m_file;
}

void VideoPreviewWidget::displayFrame(const QtAV::VideoFrame &frame)
{
    int diff = qAbs(qint64(frame.timestamp()*1000.0) - m_extractor->position());

    if (diff > m_extractor->precision())
    {
        //qCWarning(DIGIKAM_QTAVWIDGETS_LOG_WARN).noquote() << QString::asprintf("timestamp difference (%d/%lld) is too large! ignore", diff);
    }

    if (!frame.isValid())
    {
        displayNoFrame();

        return;
    }

    QSize s = m_out->widget()->rect().size();

    if (m_keep_ar)
    {
        QSize fs(frame.size());
        fs.scale(s, Qt::KeepAspectRatio);
        s = fs;
    }

    // make sure frame is the same size as the output widget size, and of the desired format
    // if not, convert & scale

    VideoFrame f(frame.pixelFormat() == m_out->preferredPixelFormat() && frame.size() == s
                 ? frame
                 : frame.to(m_out->preferredPixelFormat(), s)
    );

    if (!f.isValid())
    {
        return;
    }

    m_out->receive(f);
    Q_EMIT gotFrame(f);
}

void VideoPreviewWidget::displayNoFrame()
{
    m_out->receive(VideoFrame());
}

} // namespace QtAV
