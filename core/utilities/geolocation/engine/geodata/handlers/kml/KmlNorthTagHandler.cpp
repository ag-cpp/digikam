/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlNorthTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(north)

GeoNode* KmlnorthTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_north)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const north = parser.readElementText().trimmed().toDouble();

    if (parentItem.represents(kmlTag_LatLonAltBox))
    {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setNorth(north, GeoDataCoordinates::Degree);
    }

    else if (parentItem.represents(kmlTag_LatLonBox))
    {
        parentItem.nodeAs<GeoDataLatLonBox>()->setNorth(north, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
