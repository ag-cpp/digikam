// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include <QString>

#include "OsmPlacemarkData.h"
#include "GeoDataPlacemark.h"

class QXmlStreamAttributes;

namespace Marble {

class GeoDataDocument;

class OsmNode {
public:
    OsmPlacemarkData & osmData();
    void parseCoordinates(const QXmlStreamAttributes &attributes);
    void setCoordinates(const GeoDataCoordinates &coordinates);

    const GeoDataCoordinates & coordinates() const;
    const OsmPlacemarkData & osmData() const;

    GeoDataPlacemark* create() const;

private:
    int populationIndex(qint64 population) const;

    OsmPlacemarkData m_osmData;
    GeoDataCoordinates m_coordinates;
};

typedef QHash<qint64,OsmNode> OsmNodes;

}
