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

#include "OpenGLWidgetRenderer.h"
#include "OpenGLRendererBase_p.h"

// Qt includes

#include <QGuiApplication>
#include <QResizeEvent>
#include <QScreen>

namespace QtAV
{

class OpenGLWidgetRendererPrivate : public OpenGLRendererBasePrivate
{
public:
    OpenGLWidgetRendererPrivate(QPaintDevice *pd)
        : OpenGLRendererBasePrivate(pd)
    {}
};

VideoRendererId OpenGLWidgetRenderer::id() const
{
    return VideoRendererId_OpenGLWidget;
}

OpenGLWidgetRenderer::OpenGLWidgetRenderer(QWidget *parent, Qt::WindowFlags f):
    QOpenGLWidget(parent, f)
  , OpenGLRendererBase(*new OpenGLWidgetRendererPrivate(this))
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidgetRenderer::initializeGL()
{
    onInitializeGL();
}

void OpenGLWidgetRenderer::paintGL()
{
    onPaintGL();
}

void OpenGLWidgetRenderer::resizeGL(int w, int h)
{
    // QGLWidget uses window()->windowHandle()->devicePixelRatio() for resizeGL(), while QOpenGLWidget does not, so scale here
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    if (!context())
        return;
    const qreal dpr = context()->screen()->devicePixelRatio();
#elif QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    // qApp->devicePixelRatio() is global, window()->windowHandle()->devicePixelRatio() depends on screen, check window() and windowHandle() is required.
    // QWidget.devicePixelRatio() is int, but float value is not implemented in old qt, so just use int is fine.
    const qreal dpr = devicePixelRatio();
#else
    const qreal dpr = qApp->devicePixelRatio();
#endif
    onResizeGL(w*dpr, h*dpr);
}

void OpenGLWidgetRenderer::resizeEvent(QResizeEvent *e)
{
    onResizeEvent(e->size().width(), e->size().height());
    QOpenGLWidget::resizeEvent(e); //will call resizeGL(). TODO:will call paintEvent()?
}

void OpenGLWidgetRenderer::showEvent(QShowEvent* /*e*/)
{
    onShowEvent(); // TODO: onShowEvent(w, h)?
    resizeGL(width(), height());
}
} // namespace QtAV
