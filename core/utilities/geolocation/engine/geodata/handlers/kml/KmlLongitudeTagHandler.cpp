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

#include "KmlLongitudeTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"
#include "GeoDataLocation.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(longitude)

GeoNode* KmllongitudeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_longitude)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataLookAt>())
    {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLookAt>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    }

    else if (parentItem.is<GeoDataCamera>())
    {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    }

    else if (parentItem.is<GeoDataLocation>())
    {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLocation>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
