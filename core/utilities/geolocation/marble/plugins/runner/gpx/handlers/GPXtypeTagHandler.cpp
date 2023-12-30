// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Anders Lund <anders@alweb.dk>
//

#include "GPXtypeTagHandler.h"

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

#include "digikam_debug.h"

namespace Marble
{
namespace gpx
{
GPX_DEFINE_TAG_HANDLER(type)

// type is used in wpt, rte and trk to provide some sort of keyword, like "Geocache/traditional cache",
// and in link to provide a mimetype

GeoNode* GPXtypeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_type)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(gpxTag_wpt)
        || parentItem.represents(gpxTag_trk)
        || parentItem.represents(gpxTag_rtept))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        placemark->setRole(parser.readElementText().trimmed());
    }
    else if (parentItem.represents(gpxTag_rte))
    {
        GeoDataFeature* route = parentItem.nodeAs<GeoDataFeature>();
        route->setRole(parser.readElementText().trimmed());
    }
    // TODO: tracks, link
    return nullptr;
}

} // namespace gpx

} // namespace Marble
