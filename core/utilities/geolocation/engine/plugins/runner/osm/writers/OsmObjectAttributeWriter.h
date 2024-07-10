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

namespace Marble
{

class OsmPlacemarkData;
class GeoWriter;

class OsmObjectAttributeWriter
{
public:

    /**
     * Writes the standard osm attributes: id, action, changeset, visible, timestamp, uid, user, version
     */
    static void writeAttributes(const OsmPlacemarkData& osmData, GeoWriter& writer);
};

} // namespace Marble

