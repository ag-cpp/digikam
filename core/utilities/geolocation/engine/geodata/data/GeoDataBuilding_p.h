/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "GeoDataGeometry_p.h"
#include "GeoDataMultiGeometry.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataBuildingPrivate : public GeoDataGeometryPrivate
{
public:

    GeoDataBuildingPrivate()
        : m_height(0.0),
          m_minLevel(0),
          m_maxLevel(0)
    {
    }

    GeoDataGeometryPrivate* copy() const override
    {
        GeoDataBuildingPrivate* copy = new GeoDataBuildingPrivate;
        *copy = *this;
        return copy;
    }

public:

    double                               m_height;
    int                                  m_minLevel;
    int                                  m_maxLevel;
    QVector<int>                         m_nonExistentLevels;
    GeoDataMultiGeometry                 m_multiGeometry;
    QString                              m_name;
    QVector<GeoDataBuilding::NamedEntry> m_entries;
};

} // namespace Marble
