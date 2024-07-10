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

#include "OsmNode.h"

// Qt includes

#include <QXmlStreamAttributes>

// Local includes

#include "OsmObjectManager.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataDocument.h"
#include "MarbleDirs.h"
#include "StyleBuilder.h"

namespace Marble
{

void OsmNode::parseCoordinates(const QXmlStreamAttributes& attributes)
{
    qreal const lon = attributes.value(QLatin1String("lon")).toDouble();
    qreal const lat = attributes.value(QLatin1String("lat")).toDouble();
    setCoordinates(GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree));
}

void OsmNode::setCoordinates(const GeoDataCoordinates& coordinates)
{
    m_coordinates = coordinates;
}

GeoDataPlacemark* OsmNode::create() const
{
    GeoDataPlacemark::GeoDataVisualCategory const category = StyleBuilder::determineVisualCategory(m_osmData);

    if (category == GeoDataPlacemark::None && m_osmData.isEmpty())
    {
        return nullptr;
    }

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    auto coordinates = m_coordinates;
    coordinates.setAltitude(m_osmData.tagValue(QString::fromUtf8("ele")).toDouble());
    placemark->setCoordinate(coordinates);

    QHash<QString, QString>::const_iterator tagIter;

    if ((category == GeoDataPlacemark::TransportCarShare || category == GeoDataPlacemark::MoneyAtm)
        && (tagIter = m_osmData.findTag(QStringLiteral("operator"))) != m_osmData.tagsEnd())
    {
        placemark->setName(tagIter.value());
    }

    else
    {
        placemark->setName(m_osmData.tagValue(QStringLiteral("name")));
    }

    if (category == GeoDataPlacemark::AerialwayStation && coordinates.altitude() != 0.0)
    {
        if (placemark->name().isEmpty())
        {
            placemark->setName(QStringLiteral("%1 m").arg(coordinates.altitude()));
        }

        else
        {
            placemark->setName(QStringLiteral("%1 (%2 m)").arg(placemark->name()).arg(coordinates.altitude()));
        }
    }

    if (placemark->name().isEmpty())
    {
        placemark->setName(m_osmData.tagValue(QStringLiteral("ref")));
    }

    placemark->setVisualCategory(category);
    placemark->setZoomLevel(StyleBuilder::minimumZoomLevel(category));
    placemark->setPopularity(StyleBuilder::popularity(placemark));

    if (category >= GeoDataPlacemark::PlaceCity && category <= GeoDataPlacemark::PlaceVillageNationalCapital)
    {
        int const population = m_osmData.tagValue(QStringLiteral("population")).toInt();
        placemark->setPopulation(qMax(0, population));

        if (population > 0)
        {
            placemark->setZoomLevel(populationIndex(population));
            placemark->setPopularity(population);
        }
    }

    if (m_osmData.containsTagKey(QLatin1String("marbleZoomLevel")))
    {
        int const zoomLevel = m_osmData.tagValue(QLatin1String("marbleZoomLevel")).toInt();
        placemark->setZoomLevel(zoomLevel);
    }

    OsmObjectManager::registerId(m_osmData.id());
    return placemark;
}

int OsmNode::populationIndex(qint64 population) const
{
    int popidx = 3;

    if (population < 2500)
    {
        popidx = 10;
    }

    else if (population < 5000)
    {
        popidx = 9;
    }

    else if (population < 25000)
    {
        popidx = 8;
    }

    else if (population < 75000)
    {
        popidx = 7;
    }

    else if (population < 250000)
    {
        popidx = 6;
    }

    else if (population < 750000)
    {
        popidx = 5;
    }

    else if (population < 2500000)
    {
        popidx = 4;
    }

    return popidx;
}

const GeoDataCoordinates& OsmNode::coordinates() const
{
    return m_coordinates;
}

OsmPlacemarkData& OsmNode::osmData()
{
    return m_osmData;
}

const OsmPlacemarkData& OsmNode::osmData() const
{
    return m_osmData;
}

} // namespace Marble
