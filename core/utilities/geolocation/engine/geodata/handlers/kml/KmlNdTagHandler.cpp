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

#include "KmlNdTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataPoint.h"
#include "OsmPlacemarkData.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_MX(nd)

GeoNode* KmlndTagHandler::parse(GeoParser& parser) const
{
    int ndIndex = parser.attribute("index").toInt();

    /* Case 1: node of a line placemark:
     *...
     * <Placemark>
     *      <ExtendedData>
     *          <mx:OsmPlacemarkData>
     *              <mx:nd index="0">...</nd>
     *              <mx:nd index="1">...</nd>
     * ...
     */
    if (parser.parentElement().represents(kmlTag_OsmPlacemarkData) && parser.parentElement(2).is<GeoDataPlacemark>())
    {
        GeoDataPlacemark* placemark = parser.parentElement(2).nodeAs<GeoDataPlacemark>();

        if (auto lineString = geodata_cast<GeoDataLineString>(placemark->geometry()))
        {
            // Using GeoDataPoint because GeoDataCoordinates is not a GeoNode, so it can't be returned.
            GeoDataPoint* point = new GeoDataPoint(lineString->at(ndIndex));
            return point;
        }

        return nullptr;
    }

    /* Case 2: node of a polygon's boundary
    *...
    * <Placemark>
    *      <ExtendedData>
    *          <mx:OsmPlacemarkData>
    *              <mx:member>
    *                   <mx:OsmPlacemarkData>
    *                       <mx:nd index="0">...</nd>
    *                       <mx:nd index="1">...</nd>
    * ...
    */
    else if (parser.parentElement().represents(kmlTag_OsmPlacemarkData) && parser.parentElement(1).is<GeoDataLinearRing>())
    {
        GeoDataLinearRing* linearRing = parser.parentElement(1).nodeAs<GeoDataLinearRing>();

        // Using GeoDataPoint because GeoDataCoordinates is not a GeoNode, so it can't be returned.
        GeoDataPoint* point = new GeoDataPoint(linearRing->at(ndIndex));
        return point;
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
