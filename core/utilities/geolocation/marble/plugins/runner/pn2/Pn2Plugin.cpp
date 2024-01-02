// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>

#include "Pn2Plugin.h"

#include <klocalizedstring.h>

#include "Pn2Runner.h"

namespace Marble
{

Pn2Plugin::Pn2Plugin(QObject* const parent)
    : ParseRunnerPlugin( parent )
{
}

QString Pn2Plugin::name() const
{
    return i18n( "Pn2 File Parser" );
}

QString Pn2Plugin::nameId() const
{
    return QStringLiteral("Pn2");
}

QString Pn2Plugin::version() const
{
    return QStringLiteral("1.0");
}

QString Pn2Plugin::description() const
{
    return i18n( "A plugin to create Geolocation Data Document from Pn2 Files" );
}

QString Pn2Plugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> Pn2Plugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Cezar Mocan"), QStringLiteral("mocancezar@gmail.com"));
}

QString Pn2Plugin::fileFormatDescription() const
{
    return i18n( "Compressed Pnt" );
}

QStringList Pn2Plugin::fileExtensions() const
{
    return QStringList(QStringLiteral("pn2"));
}

ParsingRunner* Pn2Plugin::newRunner() const
{
    return new Pn2Runner;
}

} // namespace Marble

#include "moc_Pn2Plugin.cpp"
