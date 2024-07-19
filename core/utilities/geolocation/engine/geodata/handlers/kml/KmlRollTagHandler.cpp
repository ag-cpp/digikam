/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlRollTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataOrientation.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(roll)

GeoNode* KmlrollTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_roll)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataCamera>())
    {
        qreal roll = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setRoll(roll);
    }

    else if (parentItem.is<GeoDataOrientation>())
    {
        qreal roll = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataOrientation>()->setRoll(roll);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
