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

#include "GPXwptTagHandler.h"

// Qt includes

#include <QStringView>

// Local includes

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "digikam_debug.h"

namespace Marble
{

namespace gpx
{

GPX_DEFINE_TAG_HANDLER(wpt)

GeoNode* GPXwptTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_wpt)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(gpxTag_gpx))
    {
        GeoDataDocument* doc = parentItem.nodeAs<GeoDataDocument>();
        GeoDataPlacemark* placemark = new GeoDataPlacemark;

        QXmlStreamAttributes attributes = parser.attributes();
        QStringView tmp;
        qreal lat = 0;
        qreal lon = 0;
        tmp = attributes.value(QLatin1String(gpxTag_lat));

        if (!tmp.isEmpty())
        {
            lat = tmp.toString().toFloat();
        }

        tmp = attributes.value(QLatin1String(gpxTag_lon));

        if (!tmp.isEmpty())
        {
            lon = tmp.toString().toFloat();
        }

        placemark->setCoordinate(lon, lat, 0, GeoDataCoordinates::Degree);
        placemark->setRole(QStringLiteral("Waypoint"));

        placemark->setStyle(doc->style(QStringLiteral("waypoint")));

        doc->append(placemark);
        return placemark;
    }

    return nullptr;
}

} // namespace gpx

} // namespace Marble
