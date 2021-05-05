/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to apply distortion effects.
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

#include "distortionfxtoolplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "editorwindow.h"
#include "distortionfxtool.h"

namespace DigikamEditorDistortionFxToolPlugin
{

DistortionFXToolPlugin::DistortionFXToolPlugin(QObject* const parent)
    : DPluginEditor(parent)
{
}

DistortionFXToolPlugin::~DistortionFXToolPlugin()
{
}

QString DistortionFXToolPlugin::name() const
{
    return i18nc("@title", "Distortion Effects");
}

QString DistortionFXToolPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DistortionFXToolPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("draw-spiral"));
}

QString DistortionFXToolPlugin::description() const
{
    return i18nc("@info", "A tool to apply distortion effects to an image");
}

QString DistortionFXToolPlugin::details() const
{
    return i18nc("@info", "This Image Editor tool can apply distortion effects to an image.");
}

QList<DPluginAuthor> DistortionFXToolPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Marcel Wiesweg"),
                             QString::fromUtf8("marcel dot wiesweg at gmx dot de"),
                             QString::fromUtf8("(C) 2006-2012"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void DistortionFXToolPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Distortion Effects..."));
    ac->setObjectName(QLatin1String("editorwindow_filter_distortionfx"));
    ac->setActionCategory(DPluginAction::EditorFilters);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotDistortionFX()));

    addAction(ac);
}

void DistortionFXToolPlugin::slotDistortionFX()
{
    EditorWindow* const editor = dynamic_cast<EditorWindow*>(sender()->parent());

    if (editor)
    {
        DistortionFXTool* const tool = new DistortionFXTool(editor);
        tool->setPlugin(this);
        editor->loadTool(tool);
    }
}

} // namespace DigikamEditorDistortionFxToolPlugin
