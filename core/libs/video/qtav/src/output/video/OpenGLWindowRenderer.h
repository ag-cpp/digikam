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

#ifndef QTAV_OPENGL_WINDOW_RENDERER_H
#define QTAV_OPENGL_WINDOW_RENDERER_H

#ifndef QT_NO_OPENGL
#   include <QOpenGLWindow>
#   include "OpenGLRendererBase.h"

namespace QtAV
{

class OpenGLWindowRendererPrivate;

class QTAV_EXPORT OpenGLWindowRenderer : public QOpenGLWindow,
                                         public OpenGLRendererBase
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(OpenGLWindowRenderer)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QRectF regionOfInterest READ regionOfInterest WRITE setRegionOfInterest NOTIFY regionOfInterestChanged)
    Q_PROPERTY(qreal sourceAspectRatio READ sourceAspectRatio NOTIFY sourceAspectRatioChanged)
    Q_PROPERTY(qreal outAspectRatio READ outAspectRatio WRITE setOutAspectRatio NOTIFY outAspectRatioChanged)

    // fillMode

    // TODO: how to use enums in base class as property or Q_ENUM

    Q_PROPERTY(OutAspectRatioMode outAspectRatioMode READ outAspectRatioMode WRITE setOutAspectRatioMode NOTIFY outAspectRatioModeChanged)
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QRect videoRect READ videoRect NOTIFY videoRectChanged)
    Q_PROPERTY(QSize videoFrameSize READ videoFrameSize NOTIFY videoFrameSizeChanged)

public:

    explicit OpenGLWindowRenderer(UpdateBehavior updateBehavior = NoPartialUpdate,
                                  QWindow* const parent = nullptr);
    virtual VideoRendererId id() const              override;

    QWindow* qwindow()                              override final
    {
        return this;
    }

    DECLARE_VIDEO_RENDERER_SIGNALS

protected:

    virtual void initializeGL()                     override;
    virtual void paintGL()                          override;
    virtual void resizeGL(int w, int h)             override;
    virtual void resizeEvent(QResizeEvent*)         override;
    virtual void showEvent(QShowEvent*)             override;

    DECLARE_VIDEO_RENDERER_EMIT_METHODS
};

typedef OpenGLWindowRenderer VideoRendererOpenGLWindow;

} // namespace QtAV

#endif //QT_NO_OPENGL

#endif // QTAV_OPENGL_WINDOW_RENDERER_H
