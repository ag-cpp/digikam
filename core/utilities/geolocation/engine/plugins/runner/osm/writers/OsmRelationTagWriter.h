/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMap>

namespace Marble
{

class GeoDataPolygon;
class GeoWriter;
class GeoDataPlacemark;
class OsmPlacemarkData;

class OsmRelationTagWriter
{

public:

    static void writeRelation(const QVector<GeoDataPlacemark*>& relation, const QString& id,
                              const QMap<QString, QString>& tags, GeoWriter& writer);

    static void writeMultipolygon(const GeoDataPolygon& polygon,
                                  const OsmPlacemarkData& osmData, GeoWriter& writer);
};

} // namespace Marble
