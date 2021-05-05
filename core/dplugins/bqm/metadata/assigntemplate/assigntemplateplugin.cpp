/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to assign template
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

#include "assigntemplateplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "assigntemplate.h"

namespace DigikamBqmAssignTemplatePlugin
{

AssignTemplatePlugin::AssignTemplatePlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

AssignTemplatePlugin::~AssignTemplatePlugin()
{
}

QString AssignTemplatePlugin::name() const
{
    return i18n("Apply Metadata Template");
}

QString AssignTemplatePlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon AssignTemplatePlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("text-xml"));
}

QString AssignTemplatePlugin::description() const
{
    return i18n("A tool to apply metadata template to images");
}

QString AssignTemplatePlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can apply metadata template over images.</p>");
}

QList<DPluginAuthor> AssignTemplatePlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2021"))
            ;
}

void AssignTemplatePlugin::setup(QObject* const parent)
{
    AssignTemplate* const tool = new AssignTemplate(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmAssignTemplatePlugin
