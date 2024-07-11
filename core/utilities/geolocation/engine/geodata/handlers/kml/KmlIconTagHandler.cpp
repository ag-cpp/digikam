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

#include "KmlIconTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(Icon)

GeoNode* KmlIconTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Icon)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_IconStyle))
    {
        return parentItem.nodeAs<GeoDataIconStyle>();
    }

    else if (parentItem.represents(kmlTag_GroundOverlay))
    {
        return parentItem.nodeAs<GeoDataGroundOverlay>();
    }

    else if (parentItem.represents(kmlTag_PhotoOverlay))
    {
        return parentItem.nodeAs<GeoDataPhotoOverlay>();
    }

    else if (parentItem.represents(kmlTag_ScreenOverlay))
    {
        return parentItem.nodeAs<GeoDataScreenOverlay>();
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
