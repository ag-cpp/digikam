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

#include "KmlAltitudeModeTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataGeometry.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"
#include "GeoDataAbstractView.h"
#include "GeoDataModel.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(altitudeMode)

GeoNode* KmlaltitudeModeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_altitudeMode)));

    QString content = parser.readElementText().trimmed();

    AltitudeMode mode;

    if (content == QLatin1String("relativeToGround"))
    {
        mode = RelativeToGround;
    }

    else if (content == QLatin1String("absolute"))
    {
        mode = Absolute;
    }

    else     // clampToGround is Standard
    {
        mode = ClampToGround;
    }

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlacemark>())
    {
        parentItem.nodeAs<GeoDataPlacemark>()->geometry()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataPoint>())
    {
        parentItem.nodeAs<GeoDataPoint>()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataLatLonAltBox>())
    {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataTrack>())
    {
        parentItem.nodeAs<GeoDataTrack>()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataGroundOverlay>())
    {
        parentItem.nodeAs<GeoDataGroundOverlay>()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataAbstractView>())
    {
        parentItem.nodeAs<GeoDataAbstractView>()->setAltitudeMode(mode);
    }

    else if (parentItem.is<GeoDataModel>())
    {
        parentItem.nodeAs<GeoDataModel>()->setAltitudeMode(mode);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
