/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-04-21
 * Description : video thumbnails extraction based on ffmpeg
 *
 * Copyright (C) 2010      by Dirk Vanden Boer <dirk dot vdb at gmail dot com>
 * Copyright (C) 2016-2018 by Maik Qualmann <metzpinguin at gmail dot com>
 * Copyright (C) 2016-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "videodecoder.h"
#include "videodecoder_p.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

VideoDecoder::VideoDecoder(const QString& filename)
    : d(new Private)
{
    initialize(filename);
}

VideoDecoder::~VideoDecoder()
{
    destroy();
    delete d;
}

void VideoDecoder::initialize(const QString& filename)
{
    d->lastWidth  = -1;
    d->lastHeight = -1;
    d->lastPixfmt = AV_PIX_FMT_NONE;

#if LIBAVFORMAT_VERSION_MAJOR < 58

    av_register_all();

#endif

#if LIBAVCODEC_VERSION_MAJOR < 58

    avcodec_register_all();

#endif

    if (avformat_open_input(&d->pFormatContext,
                            filename.toUtf8().data(), nullptr, nullptr) != 0)
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "Could not open input file: "
                                    << filename;
        return;
    }

    if (avformat_find_stream_info(d->pFormatContext, nullptr) < 0)
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "Could not find stream information";
        return;
    }

    if (!d->initializeVideo())
    {
        return;
    }

    d->pFrame = av_frame_alloc();

    if (d->pFrame)
    {
        d->initialized = true;
    }
}

bool VideoDecoder::getInitialized() const
{
    return d->initialized;
}

void VideoDecoder::destroy()
{
    d->deleteFilterGraph();

    if (d->pVideoCodecContext)
    {
        avcodec_close(d->pVideoCodecContext);
        d->pVideoCodecContext = nullptr;
    }

    if (d->pFormatContext)
    {
        avformat_close_input(&d->pFormatContext);
        d->pFormatContext = nullptr;
    }

    if (d->pPacket)
    {
        av_packet_unref(d->pPacket);
        delete d->pPacket;
        d->pPacket = nullptr;
    }

    if (d->pFrame)
    {
        av_frame_free(&d->pFrame);
        d->pFrame = nullptr;
    }

    if (d->pFrameBuffer)
    {
        av_free(d->pFrameBuffer);
        d->pFrameBuffer = nullptr;
    }
}

QString VideoDecoder::getCodec() const
{
    QString codecName;

    if (d->pVideoCodec)
    {
        codecName = QLatin1String(d->pVideoCodec->name);
    }

    return codecName;
}

int VideoDecoder::getWidth() const
{
    if (d->pVideoCodecContext)
    {
        return d->pVideoCodecContext->width;
    }

    return -1;
}

int VideoDecoder::getHeight() const
{
    if (d->pVideoCodecContext)
    {
        return d->pVideoCodecContext->height;
    }

    return -1;
}

int VideoDecoder::getDuration() const
{
    if (d->pFormatContext)
    {
        return static_cast<int>(d->pFormatContext->duration / AV_TIME_BASE);
    }

    return 0;
}

void VideoDecoder::seek(int timeInSeconds)
{
    if (!d->allowSeek)
    {
        return;
    }

    qint64 timestamp = AV_TIME_BASE * static_cast<qint64>(timeInSeconds);

    if (timestamp < 0)
    {
        timestamp = 0;
    }

    int ret = av_seek_frame(d->pFormatContext, -1, timestamp,
                            AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);

    if (ret >= 0)
    {
        avcodec_flush_buffers(d->pVideoCodecContext);
    }
    else
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "Seeking in video failed";
        return;
    }

    int keyFrameAttempts = 0;
    bool gotFrame        = false;

    do
    {
        int count = 0;
        gotFrame  = false;

        while (!gotFrame && (count < 20))
        {
            d->getVideoPacket();
            gotFrame = d->decodeVideoPacket();
            count++;
        }

        keyFrameAttempts++;
    }
    while ((!gotFrame || !d->pFrame->key_frame) &&
           (keyFrameAttempts < 200));

    if (!gotFrame)
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "Seeking in video failed";
    }
}

bool VideoDecoder::decodeVideoFrame() const
{
    bool frameFinished = false;

    while (!frameFinished && d->getVideoPacket())
    {
        frameFinished = d->decodeVideoPacket();
    }

    if (!frameFinished)
    {
        qDebug(DIGIKAM_GENERAL_LOG) << "decodeVideoFrame() failed: frame not finished";
    }

    return frameFinished;
}

void VideoDecoder::getScaledVideoFrame(int scaledSize,
                                       bool maintainAspectRatio,
                                       VideoFrame& videoFrame)
{
    if (d->pFrame->interlaced_frame)
    {
        d->processFilterGraph(d->pFrame,
                              d->pFrame,
                              d->pVideoCodecContext->pix_fmt,
                              d->pVideoCodecContext->width,
                              d->pVideoCodecContext->height);
    }

    int scaledWidth, scaledHeight;
    d->convertAndScaleFrame(AV_PIX_FMT_RGB24,
                            scaledSize,
                            maintainAspectRatio,
                            scaledWidth,
                            scaledHeight);

    videoFrame.width    = scaledWidth;
    videoFrame.height   = scaledHeight;
    videoFrame.lineSize = d->pFrame->linesize[0];

    videoFrame.frameData.clear();
    videoFrame.frameData.resize(videoFrame.lineSize * videoFrame.height);
    memcpy((&(videoFrame.frameData.front())),
           d->pFrame->data[0],
           videoFrame.lineSize * videoFrame.height);
}

} // namespace Digikam
