/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to resize images.
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

#include "resizeplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "resize.h"

namespace DigikamBqmResizePlugin
{

ResizePlugin::ResizePlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ResizePlugin::~ResizePlugin()
{
}

QString ResizePlugin::name() const
{
    return i18n("Resize");
}

QString ResizePlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ResizePlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("transform-scale"));
}

QString ResizePlugin::description() const
{
    return i18n("A tool to resize images with a customized length");
}

QString ResizePlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can resize images.</p>");
}

QList<DPluginAuthor> ResizePlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Sambhav Dusad"),
                             QString::fromUtf8("sambhavdusad24 at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2021"))
            ;
}

void ResizePlugin::setup(QObject* const parent)
{
    Resize* const tool = new Resize(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmResizePlugin
