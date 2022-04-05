/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to convert to DNG.
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

#include "converttodngplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttodng.h"

namespace DigikamBqmConvertToDngPlugin
{

ConvertToDngPlugin::ConvertToDngPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToDngPlugin::~ConvertToDngPlugin()
{
}

QString ConvertToDngPlugin::name() const
{
    return i18nc("@title", "Convert RAW To DNG");
}

QString ConvertToDngPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToDngPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString ConvertToDngPlugin::description() const
{
    return i18nc("@info", "A tool to convert RAW images to DNG container");
}

QString ConvertToDngPlugin::details() const
{
    return xi18nc("@info", "<p>This Generic tool can convert RAW images data to DNG format.</p>"
                  "<p>The Digital Negative is a lossless RAW image format created by Adobe.</p>"
                  "<p>See details about this format from <a href='https://en.wikipedia.org/wiki/Digital_Negative'>this page</a>.</p>");
}

QList<DPluginAuthor> ConvertToDngPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2022"))
            << DPluginAuthor(QString::fromUtf8("Jens Mueller"),
                             QString::fromUtf8("tschenser at gmx dot de"),
                             QString::fromUtf8("(C) 2010-2011"))
            << DPluginAuthor(QString::fromUtf8("Smit Mehta"),
                             QString::fromUtf8("smit dot meh at gmail dot com"),
                             QString::fromUtf8("(C) 2012"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2020-2022"))
            ;
}

void ConvertToDngPlugin::setup(QObject* const parent)
{
    ConvertToDNG* const tool = new ConvertToDNG(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToDngPlugin
