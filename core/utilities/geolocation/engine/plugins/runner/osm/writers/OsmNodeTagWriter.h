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

// Qt includes

#include <QList>

// Local includes

#include "translators/OsmConverter.h"

namespace Marble
{

class GeoWriter;
class OsmWayData;

class OsmNodeTagWriter
{
public:

    static void writeAllNodes(const OsmConverter::Nodes& osmData, GeoWriter& writer);

private:

    static void writeNode(const OsmConverter::Node& node, GeoWriter& writer);
};

} // namespace Marble
