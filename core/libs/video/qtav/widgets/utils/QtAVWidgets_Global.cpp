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

#include "QtAVWidgets_Global.h"

// Qt includes

#include <QApplication>
#include <QBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "WidgetRenderer.h"
#include "GraphicsItemRenderer.h"

#if QTAV_HAVE(GL)
#   include "GLWidgetRenderer.h"        // Qt5 and Qt6
#   include "GLWidgetRenderer2.h"       // Qt5
#endif

#ifndef QT_NO_OPENGL
#   include "OpenGLWidgetRenderer.h"
#endif

#include "QtAV_factory.h"
#include "QtAV_mkid.h"
#include "digikam_debug.h"

namespace QtAV
{

VideoRendererId VideoRendererId_Widget          = mkid::id32base36_6<'W', 'i', 'd', 'g', 'e', 't'>::value;
VideoRendererId VideoRendererId_OpenGLWidget    = mkid::id32base36_6<'Q', 'O', 'G', 'L', 'W', 't'>::value;
VideoRendererId VideoRendererId_GLWidget2       = mkid::id32base36_6<'Q', 'G', 'L', 'W', 't', '2'>::value;
VideoRendererId VideoRendererId_GLWidget        = mkid::id32base36_6<'Q', 'G', 'L', 'W', 't', '1'>::value;
VideoRendererId VideoRendererId_GraphicsItem    = mkid::id32base36_6<'Q', 'G', 'r', 'a', 'p', 'h'>::value;
VideoRendererId VideoRendererId_GDI             = mkid::id32base36_3<'G', 'D', 'I'>::value;
VideoRendererId VideoRendererId_Direct2D        = mkid::id32base36_3<'D', '2', 'D'>::value;
VideoRendererId VideoRendererId_XV              = mkid::id32base36_6<'X', 'V', 'i', 'd', 'e', 'o'>::value;
VideoRendererId VideoRendererId_X11             = mkid::id32base36_3<'X', '1', '1'>::value;

// QPainterRenderer is abstract. So can not register(operator new will needed)

#if AUTO_REGISTER

FACTORY_REGISTER(VideoRenderer, Widget,         "QWidegt")
FACTORY_REGISTER(VideoRenderer, GraphicsItem,   "QGraphicsItem")

#   if QTAV_HAVE(GL)

FACTORY_REGISTER(VideoRenderer, GLWidget,       "QGLWidegt")
FACTORY_REGISTER(VideoRenderer, GLWidget2,      "QGLWidegt2")

#   endif

#   ifndef QT_NO_OPENGL

FACTORY_REGISTER(VideoRenderer, OpenGLWidget,   "OpenGLWidget")

#   endif
#endif

extern void RegisterVideoRendererGDI_Man();
extern void RegisterVideoRendererDirect2D_Man();
extern void RegisterVideoRendererXV_Man();
extern void RegisterVideoRendererX11_Man();

namespace Widgets
{

void registerRenderers()
{

#if !defined(QT_NO_DEBUG)

    qCDebug(DIGIKAM_QTAVWIDGETS_LOG).noquote() << QString::asprintf("Register QtAV Renderers");

#endif

    // check whether it is called

    static bool initialized = false;

    if (initialized)
        return;

    initialized = true;

    // factory.h does not check whether an id is registered

    if (VideoRenderer::name(VideoRendererId_Widget))
        return;

#ifndef QT_NO_OPENGL

    VideoRenderer::Register<OpenGLWidgetRenderer>(VideoRendererId_OpenGLWidget, "OpenGLWidget");

#endif

#if QTAV_HAVE(GL)

    VideoRenderer::Register<GLWidgetRenderer>(VideoRendererId_GLWidget,         "QGLWidget");
    VideoRenderer::Register<GLWidgetRenderer2>(VideoRendererId_GLWidget2,       "QGLWidget2");

#endif

    VideoRenderer::Register<WidgetRenderer>(VideoRendererId_Widget,             "Widget");

#if QTAV_HAVE(GDIPLUS)

    RegisterVideoRendererGDI_Man();

#endif

#if QTAV_HAVE(DIRECT2D)

    RegisterVideoRendererDirect2D_Man();

#endif

#if QTAV_HAVE(XV)

    RegisterVideoRendererXV_Man();

#endif

#if QTAV_HAVE(X11)

    RegisterVideoRendererX11_Man();

#endif

    VideoRenderer::Register<GraphicsItemRenderer>(VideoRendererId_GraphicsItem, "GraphicsItem");
}

} // namespace Widgets

namespace
{
    static const struct Q_DECL_HIDDEN register_renderers
    {
        inline register_renderers()
        {
            QtAV::Widgets::registerRenderers();
        }
    } sRegisterVO;
}

void about()
{
    // we should use new because a qobject will delete it's children

    QTextBrowser* const viewQtAV   = new QTextBrowser;
    QTextBrowser* const viewFFmpeg = new QTextBrowser;
    viewQtAV->setOpenExternalLinks(true);
    viewFFmpeg->setOpenExternalLinks(true);
    viewQtAV->setHtml(aboutQtAV_HTML());
    viewFFmpeg->setHtml(aboutFFmpeg_HTML());
    QTabWidget* const tab          = new QTabWidget;
    tab->addTab(viewQtAV,   QLatin1String("QtAV"));
    tab->addTab(viewFFmpeg, QLatin1String("FFmpeg"));
    QPushButton* const qbtn        = new QPushButton(i18n("About Qt"));
    QPushButton* const btn         = new QPushButton(i18n("Ok"));
    QHBoxLayout* const btnLayout   = new QHBoxLayout;
    btnLayout->addWidget(btn);
    btnLayout->addStretch();
    btnLayout->addWidget(qbtn);
    btn->setFocus();
    QDialog dialog;
    dialog.setWindowTitle(i18n("About QtAV"));
    QVBoxLayout* const layout      = new QVBoxLayout;
    dialog.setLayout(layout);
    layout->addWidget(tab);
    layout->addLayout(btnLayout);

    QObject::connect(qbtn, SIGNAL(clicked()),
                     qApp, SLOT(aboutQt()));

    QObject::connect(btn, SIGNAL(clicked()),
                     &dialog, SLOT(accept()));

    dialog.exec();
}

void aboutFFmpeg()
{
    QMessageBox::about(nullptr, i18n("About FFmpeg"), aboutFFmpeg_HTML());
}

void aboutQtAV()
{
    QMessageBox::about(nullptr, i18n("About QtAV"), aboutQtAV_HTML());
}

} // namespace QtAV
