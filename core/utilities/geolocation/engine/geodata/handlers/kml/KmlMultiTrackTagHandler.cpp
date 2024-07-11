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

#include "KmlMultiTrackTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataPlacemark.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(MultiTrack)

GeoNode* KmlMultiTrackTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_MultiTrack)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataMultiTrack* geom = new GeoDataMultiTrack;
    KmlObjectTagHandler::parseIdentifiers(parser, geom);

    if (parentItem.represents(kmlTag_Placemark))
    {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(geom);
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    }

    else if (parentItem.represents(kmlTag_MultiGeometry))
    {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(geom);
        return geom;
    }

    else
    {
        delete geom;
        return nullptr;
    }
}

} // namespace kml

} // namespace Marble
