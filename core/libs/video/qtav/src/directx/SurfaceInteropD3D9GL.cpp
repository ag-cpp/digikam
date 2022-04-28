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

#include "SurfaceInteropD3D9.h"

#define DX_LOG_COMPONENT "D3D9GL Interop"

// Local includes

#include "DirectXHelper.h"
#include "OpenGLHelper.h"
#include "VideoDecoderD3D.h"
#include "digikam_debug.h"

// dynamic gl or desktop gl

namespace QtAV
{

namespace d3d9
{

class GLInteropResource final : public InteropResource
{
public:

    GLInteropResource(IDirect3DDevice9* d3device);
    ~GLInteropResource();

    bool map(IDirect3DSurface9* surface, GLuint tex, int frame_w, int frame_h, int) override;
    bool unmap(GLuint tex)                                                          override;

private:

    bool ensureResource(int w, int h, GLuint tex);

    HANDLE interop_dev;
    HANDLE interop_obj;
};

InteropResource* CreateInteropGL(IDirect3DDevice9 *dev)
{
    return new GLInteropResource(dev);
}

GLInteropResource::GLInteropResource(IDirect3DDevice9 *d3device)
    : InteropResource(d3device)
    , interop_dev(nullptr)
    , interop_obj(nullptr)
{
}

GLInteropResource::~GLInteropResource()
{
    // FIXME: why unregister/close interop obj/dev here will crash(tested on intel driver)? must be in current opengl context?
}

bool GLInteropResource::map(IDirect3DSurface9 *surface, GLuint tex, int w, int h, int)
{
    if (!ensureResource(w, h, tex))
    {
        releaseDX();

        return false;
    }

    // open/close and register/unregster in every map/unmap to ensure called in current context and avoid crash (tested on intel driver)
    // interop operations begin

    WGL_ENSURE((interop_dev = gl().DXOpenDeviceNV(d3ddev)) != nullptr, false);

    // call in ensureResource or in map?

    WGL_ENSURE((interop_obj = gl().DXRegisterObjectNV(interop_dev, dx_surface, tex, GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV)) != nullptr, false);

    // prepare dx resources for gl

    const RECT src = { 0, 0, (~0-1)&w, (~0-1)&h}; // StretchRect does not supports odd values
    DX_ENSURE_OK(d3ddev->StretchRect(surface, &src, dx_surface, nullptr, D3DTEXF_NONE), false);

    // lock dx resources

    WGL_ENSURE(gl().DXLockObjectsNV(interop_dev, 1, &interop_obj), false);
    WGL_ENSURE(gl().DXObjectAccessNV(interop_obj, WGL_ACCESS_READ_ONLY_NV), false);
    DYGL(glBindTexture(GL_TEXTURE_2D, tex));

    return true;
}

bool GLInteropResource::unmap(GLuint tex)
{
    Q_UNUSED(tex);

    if (!interop_obj || !interop_dev)
        return false;

    DYGL(glBindTexture(GL_TEXTURE_2D, 0));
    WGL_ENSURE(gl().DXUnlockObjectsNV(interop_dev, 1, &interop_obj), false);
    WGL_WARN(gl().DXUnregisterObjectNV(interop_dev, interop_obj));

    // interop operations end

    WGL_WARN(gl().DXCloseDeviceNV(interop_dev));
    interop_obj = nullptr;
    interop_dev = nullptr;

    return true;
}

// IDirect3DDevice9 can not be used on WDDM OSes(>=vista)

bool GLInteropResource::ensureResource(int w, int h, GLuint tex)
{
    Q_UNUSED(tex);
    Q_ASSERT(gl().DXRegisterObjectNV && "WGL_NV_DX_interop is required");

    if (dx_surface && width == w && height == h)
        return true;

    releaseDX();
    HANDLE share_handle = nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    const bool has_alpha = QOpenGLContext::currentContext()->format().hasAlpha();

#else

    const bool has_alpha = QOpenGLContext::currentContext()->format().alpha();

#endif

    // _A8 for a yuv plane

    DX_ENSURE_OK(d3ddev->CreateTexture(w, h, 1,
                                       D3DUSAGE_RENDERTARGET,
                                       has_alpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8,
                                       D3DPOOL_DEFAULT,
                                       &dx_texture,
                                       &share_handle) , false);

    DX_ENSURE_OK(dx_texture->GetSurfaceLevel(0, &dx_surface), false);

    // required by d3d9 not d3d10&11: https://www.opengl.org/registry/specs/NV/DX_interop2.txt

    WGL_WARN(gl().DXSetResourceShareHandleNV(dx_surface, share_handle));
    width  = w;
    height = h;

    return true;
}

} // namespace d3d9

} // namespace QtAV
