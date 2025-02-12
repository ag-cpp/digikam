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

#include "GPXurlTagHandler.h"

// Local includes

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "digikam_debug.h"

namespace Marble
{

namespace gpx
{

GPX_DEFINE_TAG_HANDLER_10(url)

// Gpx 1.0 url element provides a link related to a waypoint, and is
// inserted into the waypoint description as this seems to be the
// simplest means to make it available to the user.
// In addition, url properties are saved to extendedData.
// The insertion is done in the urlname element, which is the link text.
GeoNode* GPXurlTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_url)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(gpxTag_wpt))
    {
        GeoDataPlacemark* placemark = parentItem.nodeAs<GeoDataPlacemark>();

        QString url = parser.readElementText().trimmed();

        GeoDataExtendedData extendedData = placemark->extendedData();
        extendedData.addValue(GeoDataData(QStringLiteral("url"), url));
        placemark->setExtendedData(extendedData);
    }

    return nullptr;
}

} // namespace gpx

} // namespace Marble
