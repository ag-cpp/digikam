/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-17
 * Description : Database manager digiKam plugin definition.
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "dplugindb.h"

namespace Digikam
{

class Q_DECL_HIDDEN DPluginDb::Private
{
public:

    explicit Private()
    {
    }
};

DPluginDb::DPluginDb(QObject* const parent = nullptr)
    : DPlugin(parent)
{
}

DPluginDb::~DPluginDb()
{
}

void DPluginDb::setVisible(bool b)
{
    emit signalVisible(b);
}

int DPluginDb::count() const
{
    // TODO count tools
    //return d->tools.count();
    return 0;
}


QStringList DPluginDb::categories() const
{
    QStringList list;

    // TODO retrieve categories
/*
    foreach (BatchTool* const t, d->tools)
    {
        QString cat = t->toolGroupToString();

        if (!list.contains(cat))
        {
            list << cat;
        }
    }
*/
    list.sort();

    return list;
}

bool DPluginDb::hasVisibilityProperty() const
{
    // NOTE: all DB plugins are not yet configurable.
    return false;
}

}
