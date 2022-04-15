/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_VIDEODECODERFFMPEGHW_H
#define QTAV_VIDEODECODERFFMPEGHW_H

#include "VideoDecoderFFmpegBase.h"

namespace QtAV {

class VideoDecoderFFmpegHWPrivate;
class VideoDecoderFFmpegHW : public VideoDecoderFFmpegBase
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoDecoderFFmpegHW)
    DPTR_DECLARE_PRIVATE(VideoDecoderFFmpegHW)
    Q_PROPERTY(int threads READ threads WRITE setThreads NOTIFY threadsChanged) // <=0 is auto
    Q_PROPERTY(CopyMode copyMode READ copyMode WRITE setCopyMode NOTIFY copyModeChanged)
    Q_ENUMS(CopyMode)
public:
    enum CopyMode {
        ZeroCopy,
        OptimizedCopy,
        GenericCopy
    };
    VideoFrame copyToFrame(const VideoFormat& fmt, int surface_h, quint8* src[], int pitch[], bool swapUV);
    // properties
    int threads() const;
    void setThreads(int value);
    void setCopyMode(CopyMode value);
    CopyMode copyMode() const;
Q_SIGNALS:
    void copyModeChanged();
    void threadsChanged();
protected:
    VideoDecoderFFmpegHW(VideoDecoderFFmpegHWPrivate &d);
private:
    VideoDecoderFFmpegHW();
};

} //namespace QtAV

#endif // QTAV_VIDEODECODERFFMPEGHW_H
