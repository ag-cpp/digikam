/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to fix BCG.
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

#include "bcgtoolplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "editorwindow.h"
#include "bcgtool.h"

namespace DigikamEditorBCGToolPlugin
{

BCGToolPlugin::BCGToolPlugin(QObject* const parent)
    : DPluginEditor(parent)
{
}

BCGToolPlugin::~BCGToolPlugin()
{
}

QString BCGToolPlugin::name() const
{
    return i18nc("@title", "BCG Correction");
}

QString BCGToolPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon BCGToolPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("contrast"));
}

QString BCGToolPlugin::description() const
{
    return i18nc("@info", "A tool to fix Brightness / Contrast / Gamma");
}

QString BCGToolPlugin::details() const
{
    return i18nc("@title", "This Image Editor tool can adjust Brightness / Contrast / Gamma from image.");
}

QList<DPluginAuthor> BCGToolPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2004"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void BCGToolPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Brightness/Contrast/Gamma..."));
    ac->setObjectName(QLatin1String("editorwindow_color_bcg"));
    ac->setActionCategory(DPluginAction::EditorColors);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotBCGTool()));

    addAction(ac);
}

void BCGToolPlugin::slotBCGTool()
{
    EditorWindow* const editor = dynamic_cast<EditorWindow*>(sender()->parent());

    if (editor)
    {
        BCGTool* const tool = new BCGTool(editor);
        tool->setPlugin(this);
        editor->loadTool(tool);
    }
}

} // namespace DigikamEditorBCGToolPlugin
