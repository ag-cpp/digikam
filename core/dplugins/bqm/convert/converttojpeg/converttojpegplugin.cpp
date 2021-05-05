/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a BQM plugin to convert to jpeg.
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

#include "converttojpegplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttojpeg.h"

namespace DigikamBqmConvertToJpegPlugin
{

ConvertToJpegPlugin::ConvertToJpegPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToJpegPlugin::~ConvertToJpegPlugin()
{
}

QString ConvertToJpegPlugin::name() const
{
    return i18n("Convert To JPEG");
}

QString ConvertToJpegPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToJpegPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-jpeg"));
}

QString ConvertToJpegPlugin::description() const
{
    return i18n("A tool to convert images to JPEG format");
}

QString ConvertToJpegPlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can convert images data to JPEG format.</p>"
                "<p>The Joint Photographic Experts Group format is a commonly used method of lossy compression for digital images, particularly for those images produced by digital photography.</p>"
                "<p>See details about this format from <a href='https://en.wikipedia.org/wiki/JPEG'>this page</a>.</p>");
}

QList<DPluginAuthor> ConvertToJpegPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            ;
}

void ConvertToJpegPlugin::setup(QObject* const parent)
{
    ConvertToJPEG* const tool = new ConvertToJPEG(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToJpegPlugin
