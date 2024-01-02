// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_OSM_PLUGIN_H
#define MARBLE_OSM_PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class OsmPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OsmPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:

    explicit OsmPlugin(QObject* const parent = nullptr);

    QString name() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const override;

    QStringList fileExtensions() const override;

    ParsingRunner* newRunner() const override;
};

} // namespace Marble

#endif // MARBLE_OSM_PLUGIN_H
