/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to convert to JPEG2000.
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

#include "converttojp2plugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttojp2.h"

namespace DigikamBqmConvertToJp2Plugin
{

ConvertToJp2Plugin::ConvertToJp2Plugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToJp2Plugin::~ConvertToJp2Plugin()
{
}

QString ConvertToJp2Plugin::name() const
{
    return i18n("Convert To JP2");
}

QString ConvertToJp2Plugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToJp2Plugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-jpeg"));
}

QString ConvertToJp2Plugin::description() const
{
    return i18n("A tool to convert images to JPEG-2000 format");
}

QString ConvertToJp2Plugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can convert images data to JPEG-2000 format.</p>"
                "<p>This is an image compression standard and coding system. It was created by the Joint Photographic Experts Group committee in 2000 with the intention of superseding their original discrete cosine transform-based JPEG standard (created in 1992) with a newly designed, wavelet-based method.</p>"
                "<p>See details about this format from <a href='https://en.wikipedia.org/wiki/JPEG_2000'>this page</a>.</p>");
}

QList<DPluginAuthor> ConvertToJp2Plugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            ;
}

void ConvertToJp2Plugin::setup(QObject* const parent)
{
    ConvertToJP2* const tool = new ConvertToJP2(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToJp2Plugin
