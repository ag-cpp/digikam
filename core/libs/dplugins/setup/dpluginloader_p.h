/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : manager to load external plugins at run-time: private container
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DPLUGIN_LOADER_P_H
#define DIGIKAM_DPLUGIN_LOADER_P_H

#include "dpluginloader.h"

// Qt includes

#include <QDir>
#include <QPluginLoader>

namespace Digikam
{

class Q_DECL_HIDDEN DPluginLoader::Private
{
public:

    explicit Private();
    ~Private();

    /**
     * Try to find plugin files from Qt5 plugins install dir:
     */
    QFileInfoList pluginEntriesList() const;

    /**
     * Stage to load plugins in memory
     */
    void loadPlugins();

    /**
     * Append object to the given plugins list.
     */
    bool appendPlugin(QObject* const obj,
                      QPluginLoader* const loader);

public:

    bool                  pluginsLoaded;
    QList<DPlugin*>       allPlugins;
    QStringList           blacklist;
    QStringList           whitelist;
    QStringList           DKBlacklist;      ///< Showfoto specific plugins to ignore in digiKam.
};

} // namepace Digikam

#endif // DIGIKAM_DPLUGIN_LOADER_P_H
