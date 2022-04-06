/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to apply local contrast
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "localcontrastplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "localcontrast.h"

namespace DigikamBqmLocalContrastPlugin
{

LocalContrastPlugin::LocalContrastPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

LocalContrastPlugin::~LocalContrastPlugin()
{
}

QString LocalContrastPlugin::name() const
{
    return i18nc("@title", "Local Contrast");
}

QString LocalContrastPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon LocalContrastPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("contrast"));
}

QString LocalContrastPlugin::description() const
{
    return i18nc("@info", "A tool to emulate tone mapping");
}

QString LocalContrastPlugin::details() const
{
    return xi18nc("@info", "<para>This Batch Queue Manager tool can emulate tone mapping over images.</para>");
}

QList<DPluginAuthor> LocalContrastPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2022"))
            ;
}

void LocalContrastPlugin::setup(QObject* const parent)
{
    LocalContrast* const tool = new LocalContrast(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmLocalContrastPlugin
