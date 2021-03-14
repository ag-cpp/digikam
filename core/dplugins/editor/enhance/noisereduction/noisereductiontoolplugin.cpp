/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to reduce noise
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

#include "noisereductiontoolplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "editorwindow.h"
#include "noisereductiontool.h"

namespace DigikamEditorNoiseReductionToolPlugin
{

LocalContrastToolPlugin::LocalContrastToolPlugin(QObject* const parent)
    : DPluginEditor(parent)
{
}

LocalContrastToolPlugin::~LocalContrastToolPlugin()
{
}

QString LocalContrastToolPlugin::name() const
{
    return i18nc("@title", "Noise Reduction");
}

QString LocalContrastToolPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon LocalContrastToolPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("noisereduction"));
}

QString LocalContrastToolPlugin::description() const
{
    return i18nc("@info", "A tool to remove photograph noise using wavelets");
}

QString LocalContrastToolPlugin::details() const
{
    return i18nc("@info", "This Image Editor tool can reduce noise in image.");
}

QList<DPluginAuthor> LocalContrastToolPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2004-2021"))
            ;
}

void LocalContrastToolPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Noise Reduction..."));
    ac->setObjectName(QLatin1String("editorwindow_enhance_noisereduction"));
    ac->setActionCategory(DPluginAction::EditorEnhance);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotNoiseReduction()));

    addAction(ac);
}

void LocalContrastToolPlugin::slotNoiseReduction()
{
    EditorWindow* const editor = dynamic_cast<EditorWindow*>(sender()->parent());

    if (editor)
    {
        NoiseReductionTool* const tool = new NoiseReductionTool(editor);
        tool->setPlugin(this);
        editor->loadTool(tool);
    }
}

} // namespace DigikamEditorNoiseReductionToolPlugin
