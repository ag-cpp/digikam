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

#ifndef QTAV_WIDGETS_GLOBAL_H
#define QTAV_WIDGETS_GLOBAL_H

// Local includes

#include "VideoRenderer.h"
#include "digikam_export.h"

#define QTAV_WIDGETS_EXPORT         DIGIKAM_EXPORT
#define QTAV_WIDGETS_PRIVATE_EXPORT DIGIKAM_EXPORT

namespace QtAV
{

namespace Widgets
{

/*!
 * \brief registerRenderers
 * register built-in renderers.
 * If you do not explicitly use any var, function or class in this module in your code,
 * QtAVWidgets module maybe not linked to your program and renderers will not be available.
 * Then you have to call registerRenderers() to ensure QtAVWidgets module is linked.
 */
QTAV_WIDGETS_EXPORT void registerRenderers();

} // namespace Widgets

extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_Widget;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_GraphicsItem;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_GLWidget;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_GDI;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_Direct2D;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_XV;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_X11;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_GLWidget2;
extern QTAV_WIDGETS_EXPORT VideoRendererId VideoRendererId_OpenGLWidget;

// popup a dialog

QTAV_WIDGETS_EXPORT void about();
QTAV_WIDGETS_EXPORT void aboutFFmpeg();
QTAV_WIDGETS_EXPORT void aboutQtAV();

} // namespace QtAV

#endif // QTAV_WIDGETS_GLOBAL_H
