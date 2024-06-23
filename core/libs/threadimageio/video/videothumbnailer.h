/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-04-21
 * Description : video thumbnails extraction based on ffmpeg
 *
 * SPDX-FileCopyrightText: 2010      by Dirk Vanden Boer <dirk dot vdb at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2018 by Maik Qualmann <metzpinguin at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>
#include <QImage>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class VideoFrame;
class VideoThumbWriter;
class VideoThumbDecoder;
class VideoStripFilter;

class DIGIKAM_EXPORT VideoThumbnailer
{
public:

    VideoThumbnailer();
    VideoThumbnailer(int thumbnailSize,
                     bool workaroundIssues,
                     bool maintainAspectRatio,
                     bool smartFrameSelection);
    ~VideoThumbnailer();

public:

    void generateThumbnail(const QString& videoFile, QImage& image);

    void setThumbnailSize(int size);
    void setSeekPercentage(int percentage);
    void setSeekTime(const QString& seekTime);
    void setWorkAroundIssues(bool workAround);
    void setMaintainAspectRatio(bool enabled);
    void setSmartFrameSelection(bool enabled);
    void addFilter(VideoStripFilter* const filter);
    void removeFilter(const VideoStripFilter* const filter);
    void clearFilters();

private:

    void generateThumbnail(const QString& videoFile, VideoThumbWriter& imageWriter, QImage& image);
    void generateSmartThumbnail(VideoThumbDecoder& movieDecoder, VideoFrame& videoFrame);

    void applyFilters(VideoFrame& frameData);
    int  timeToSeconds(const QString& time) const;

private:

    // Disable
    VideoThumbnailer(const VideoThumbnailer&)            = delete;
    VideoThumbnailer& operator=(const VideoThumbnailer&) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
