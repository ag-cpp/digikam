/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to fix hot pixels
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "hotpixelstoolplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "editorwindow.h"
#include "hotpixelstool.h"

namespace DigikamEditorHotPixelsToolPlugin
{

HotPixelsToolPlugin::HotPixelsToolPlugin(QObject* const parent)
    : DPluginEditor(parent)
{
}

HotPixelsToolPlugin::~HotPixelsToolPlugin()
{
}

QString HotPixelsToolPlugin::name() const
{
    return i18nc("@title", "Hot Pixels");
}

QString HotPixelsToolPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon HotPixelsToolPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("hotpixels"));
}

QString HotPixelsToolPlugin::description() const
{
    return i18nc("@info", "A tool to fix hot pixels");
}

QString HotPixelsToolPlugin::details() const
{
    return i18nc("@info", "This Image Editor tool can fix hot pixels from an image.");
}

QList<DPluginAuthor> HotPixelsToolPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Unai Garro"),
                             QString::fromUtf8("ugarro at users dot sourceforge dot net"),
                             QString::fromUtf8("(C) 2005-2006"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void HotPixelsToolPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Hot Pixels..."));
    ac->setObjectName(QLatin1String("editorwindow_enhance_hotpixels"));
    ac->setActionCategory(DPluginAction::EditorEnhance);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotHotPixels()));

    addAction(ac);

    HotPixelsTool::registerFilter();
}

void HotPixelsToolPlugin::slotHotPixels()
{
    EditorWindow* const editor = dynamic_cast<EditorWindow*>(sender()->parent());

    if (editor)
    {
        HotPixelsTool* const tool = new HotPixelsTool(editor);
        tool->setPlugin(this);
        editor->loadTool(tool);
    }
}

} // namespace DigikamEditorHotPixelsToolPlugin
