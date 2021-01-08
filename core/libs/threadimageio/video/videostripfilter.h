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

#ifndef DIGIKAM_VIDEO_STRIP_FILTER_H
#define DIGIKAM_VIDEO_STRIP_FILTER_H

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class VideoFrame;

class DIGIKAM_EXPORT VideoStripFilter
{
public:

    explicit VideoStripFilter();
    ~VideoStripFilter();

    void process(VideoFrame& videoFrame);

private:

    // Hidden copy constructor and assignment operator.
    VideoStripFilter(const VideoStripFilter&);
    VideoStripFilter& operator=(const VideoStripFilter&);

};

} // namespace Digikam

#endif // DIGIKAM_VIDEO_STRIP_FILTER_H
