/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-14
 * Description : a plugin to export data from digikam's database
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_DBEXPORT_PLUGIN_H
#define DIGIKAM_DBEXPORT_PLUGIN_H

// Local includes

#include "dplugindb.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.db.DbExport"

using namespace Digikam;

namespace DigikamDbExportPlugin
{

class DbExportPlugin: public DPluginDb
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginDb)

public:

    explicit DbExportPlugin(QObject* const parent = nullptr);
    ~DbExportPlugin() override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

private:

};

} // namespace DigikamDbExportPlugin

#endif  // DIGIKAM_DBEXPORT_PLUGIN_H
