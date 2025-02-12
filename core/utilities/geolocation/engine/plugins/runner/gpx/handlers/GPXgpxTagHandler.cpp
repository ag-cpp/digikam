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

#include "GPXgpxTagHandler.h"

// Local includes

#include "GPXElementDictionary.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataHotSpot.h"
#include "MarbleColors.h"
#include "MarbleDirs.h"
#include "MarbleGlobal.h"
#include "digikam_debug.h"

namespace Marble
{

namespace gpx
{

GPX_DEFINE_TAG_HANDLER(gpx)

GeoNode* GPXgpxTagHandler::parse(GeoParser& parser) const
{
    GeoDataDocument* doc = geoDataDoc(parser);

    GeoDataStyle::Ptr style(new GeoDataStyle);
    GeoDataLineStyle lineStyle;
    QColor transparentRed = Oxygen::brickRed6;
    transparentRed.setAlpha(200);
    lineStyle.setColor(transparentRed);
    lineStyle.setWidth(4);
    style->setLineStyle(lineStyle);
    style->setId(QStringLiteral("track"));

    GeoDataStyleMap styleMap;
    styleMap.setId(QStringLiteral("map-track"));
    styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
    doc->addStyleMap(styleMap);
    doc->addStyle(style);

    // create a style for routes
    GeoDataStyle::Ptr routestyle(new GeoDataStyle);
    GeoDataLineStyle routeLineStyle;
    QColor skyBlue = Oxygen::skyBlue6;
    skyBlue.setAlpha(200);
    routeLineStyle.setColor(skyBlue);
    routeLineStyle.setWidth(5);
    routestyle->setLineStyle(routeLineStyle);
    routestyle->setId(QStringLiteral("route"));

    GeoDataStyleMap routeStyleMap;
    routeStyleMap.setId(QStringLiteral("map-route"));
    routeStyleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + routestyle->id());
    doc->addStyleMap(routeStyleMap);
    doc->addStyle(routestyle);

    // create a default style for waypoint icons
    GeoDataStyle::Ptr waypointStyle(new GeoDataStyle);
    waypointStyle->setId(QStringLiteral("waypoint"));
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/flag.png")));
    iconStyle.setHotSpot(QPointF(0.12, 0.03), GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction);
    waypointStyle->setIconStyle(iconStyle);

    GeoDataLabelStyle waypointLabelStyle;
    waypointLabelStyle.setAlignment(GeoDataLabelStyle::Corner);
    waypointStyle->setLabelStyle(waypointLabelStyle);

    GeoDataStyleMap waypointStyleMap;
    waypointStyleMap.setId(QStringLiteral("map-waypoint"));
    waypointStyleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + waypointStyle->id());
    doc->addStyleMap(waypointStyleMap);
    doc->addStyle(waypointStyle);

    return doc;
}

} // namespace gpx

} // namespace Marble
