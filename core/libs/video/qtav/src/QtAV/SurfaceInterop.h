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

#ifndef QTAV_SURFACE_INTEROP_H
#define QTAV_SURFACE_INTEROP_H

#include <QSharedPointer>

// Local includes

#include "VideoFormat.h"

namespace QtAV
{

class Q_AV_EXPORT VideoSurfaceInterop
{
public:

    virtual ~VideoSurfaceInterop() {}

    /*!
     * \brief map
     * currently is used to map a frame from hardware decoder to opengl texture, host memory.
     * \param type currently only support GLTextureSurface and HostMemorySurface for some decoders
     * \param fmt
     *   HostMemorySurface: must be a packed rgb format
     * \param handle address of real handle. handle value can be modified in map() and the caller (VideoShader for example) should manage the changes.
     *   GLTextureSurface: usually opengl texture. maybe other objects for some decoders in the feature
     *   HostMemorySurface: a VideoFrame ptr
     * \param plane
     * \return Null if not supported or failed. handle if success.
     */
    virtual void* map(SurfaceType type, const VideoFormat& fmt, void* handle = 0, int plane = 0) {
        Q_UNUSED(type);
        Q_UNUSED(fmt);
        Q_UNUSED(handle);
        Q_UNUSED(plane);
        return 0;
    }

    // TODO: SurfaceType. unmap is currenty used by opengl rendering

    virtual void unmap(void* handle) { Q_UNUSED(handle);}

    //virtual void unmap(void* handle, SurfaceType type) { Q_UNUSED(handle);} //for SourceSurfaceType

    /*!
     * \brief createHandle
     * It is used by opengl renderer to create a texture when rendering frame from VDA/VideoToolbox decoder
     * VideoSurfaceInterop does not have the ownership. VideoShader does
     * \return NULL if not used for opengl rendering. handle if create here
     */
    virtual void* createHandle(void* handle, SurfaceType type, const VideoFormat &fmt, int plane, int planeWidth, int planeHeight) {
        Q_UNUSED(handle);
        Q_UNUSED(type);
        Q_UNUSED(fmt);
        Q_UNUSED(plane);
        Q_UNUSED(planeWidth);
        Q_UNUSED(planeHeight);
        return 0;
    }
};

typedef QSharedPointer<VideoSurfaceInterop> VideoSurfaceInteropPtr;

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::VideoSurfaceInteropPtr)

#endif // QTAV_SURFACE_INTEROP_H
