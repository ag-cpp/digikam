/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-04-03
 * Description : a BQM plugin to convert to WEBP.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "converttowebpplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttowebp.h"

namespace DigikamBqmConvertToWebpPlugin
{

ConvertToWebpPlugin::ConvertToWebpPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToWebpPlugin::~ConvertToWebpPlugin()
{
}

QString ConvertToWebpPlugin::name() const
{
    return i18nc("@title", "Convert To WEBP");
}

QString ConvertToWebpPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToWebpPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString ConvertToWebpPlugin::description() const
{
    return i18nc("@info", "A tool to convert images to WEBP format");
}

QString ConvertToWebpPlugin::details() const
{
    return xi18nc("@info", "<para>This Batch Queue Manager tool can convert images data to WEBP format.</para>"
                  "<para>WEBP is an image file format that Google has developed as a replacement for JPEG, PNG, and GIF file formats. WEBP yields files that are smaller for the same quality, or of higher quality for the same size. It supports both lossy and lossless compression, as well as animation and alpha transparency.</para>"
                  "<para>See details about this format from <a href='https://en.wikipedia.org/wiki/WebP'>this page</a>.</para>");
}

QList<DPluginAuthor> ConvertToWebpPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2022"))
            ;
}

void ConvertToWebpPlugin::setup(QObject* const parent)
{
    ConvertToWEBP* const tool = new ConvertToWEBP(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToWebpPlugin
