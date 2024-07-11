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

#include "KmlWestTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(west)

GeoNode* KmlwestTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_west)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const west = parser.readElementText().trimmed().toDouble();

    if (parentItem.represents(kmlTag_LatLonAltBox))
    {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setWest(west, GeoDataCoordinates::Degree);
    }

    else if (parentItem.represents(kmlTag_LatLonBox))
    {
        parentItem.nodeAs<GeoDataLatLonBox>()->setWest(west, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
