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

#ifndef QTAV_SURFACE_INTEROP_D3D9_H
#define QTAV_SURFACE_INTEROP_D3D9_H

// Windows includes

#include <d3d9.h>

// Local includes

#include "SurfaceInterop.h"

namespace QtAV
{

namespace d3d9
{

enum InteropType 
{
    InteropAuto,
    InteropEGL,
    InteropGL
};

class InteropResource
{
public:

    static bool isSupported(InteropType type = InteropAuto);
    static InteropResource* create(IDirect3DDevice9* dev, InteropType type = InteropAuto);
    typedef unsigned int GLuint;

    explicit InteropResource(IDirect3DDevice9* d3device);
    virtual ~InteropResource();

    /*!
     * \brief map
     * \param surface d3d9 surface
     * \param tex opengl texture
     * \param w frame width(visual width) without alignment, <= d3d9 surface width
     * \param h frame height(visual height)
     * \param plane useless now
     * \return true if success
     */
    virtual bool map(IDirect3DSurface9* surface, GLuint tex, int w, int h, int plane) = 0;

    virtual bool unmap(GLuint tex)
    {
        Q_UNUSED(tex);
        return true;
    }

protected:

    void releaseDX();

protected:

    IDirect3DDevice9*   d3ddev;
    IDirect3DTexture9*  dx_texture;
    IDirect3DSurface9*  dx_surface;     // size is frame size(visual size) for display
    int                 width, height;  // video frame width and dx_surface width without alignment, not dxva decoded surface width
};

typedef QSharedPointer<InteropResource> InteropResourcePtr;

class SurfaceInterop final: public VideoSurfaceInterop
{
public:

    SurfaceInterop(const InteropResourcePtr& res)
        : m_surface(0),
          m_resource(res),
          frame_width(0),
          frame_height(0)
    {
    }

    ~SurfaceInterop();

    /*!
     * \brief setSurface
     * \param surface d3d9 surface
     * \param frame_w frame width(visual width) without alignment, <= d3d9 surface width
     * \param frame_h frame height(visual height)
     */
    void setSurface(IDirect3DSurface9* surface, int frame_w, int frame_h);

    /// GLTextureSurface only supports rgb32

    void* map(SurfaceType type, const VideoFormat& fmt, void* handle, int plane) override;
    void unmap(void* handle) override;

protected:

    /// copy from gpu (optimized if possible) and convert to target format if necessary

    void* mapToHost(const VideoFormat& format, void* handle, int plane);

private:

    IDirect3DSurface9* m_surface;
    InteropResourcePtr m_resource;
    int                frame_width, frame_height;
};

} // namespace d3d9

} // namespace QtAV

#endif // QTAV_SURFACE_INTEROP_D3D9_H
