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

#include "KmlLatLonAltBoxTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataRegion.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(LatLonAltBox)

GeoNode* KmlLatLonAltBoxTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_LatLonAltBox)));

    GeoDataLatLonAltBox box;
    KmlObjectTagHandler::parseIdentifiers(parser, &box);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Region))
    {
        parentItem.nodeAs<GeoDataRegion>()->setLatLonAltBox(box);
        return const_cast<GeoDataLatLonAltBox*>(&parentItem.nodeAs<GeoDataRegion>()->latLonAltBox());
    }

    else
    {
        return nullptr;
    }
}

} // namespace kml

} // namespace Marble
