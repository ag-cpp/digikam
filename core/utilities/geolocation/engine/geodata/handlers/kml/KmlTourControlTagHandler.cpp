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

#include "KmlTourControlTagHandler.h"

// Local includes

#include "KmlPlaylistTagHandler.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoParser.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTourControl.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(TourControl)

GeoNode* KmlTourControlTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_TourControl)));

    GeoStackItem parentItem = parser.parentElement();

    GeoDataTourControl* tourControl = new GeoDataTourControl;
    KmlObjectTagHandler::parseIdentifiers(parser, tourControl);

    if (parentItem.is<GeoDataPlaylist>())
    {
        //parentItem.nodeAs<GeoDataPlaylist>()->append(tourControl);
        parentItem.nodeAs<GeoDataPlaylist>()->addPrimitive(tourControl);
        return tourControl;
    }

    else
    {
        delete tourControl;
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
