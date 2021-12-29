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
 * Copyright (C) 2016-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VIDEO_DECODER_H
#define DIGIKAM_VIDEO_DECODER_H

// Qt includes

#include <QString>

// Local includes

#include "videothumbwriter.h"

namespace Digikam
{

class VideoDecoder
{
public:

    explicit VideoDecoder(const QString& filename);
    ~VideoDecoder();

public:

    QString getCodec()       const;
    int     getWidth()       const;
    int     getHeight()      const;
    int     getDuration()    const;
    bool    getInitialized() const;

    void seek(int timeInSeconds);
    bool decodeVideoFrame()  const;
    void getScaledVideoFrame(int scaledSize,
                             bool maintainAspectRatio,
                             VideoFrame& videoFrame);

    void initialize(const QString& filename);
    void destroy();

private:

    // Disable
    VideoDecoder(const VideoDecoder&)            = delete;
    VideoDecoder& operator=(const VideoDecoder&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_VIDEO_DECODER_H
