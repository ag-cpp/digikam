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

#include "KmlAddressTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(address)

GeoNode* KmladdressTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_address)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>())
    {
        const auto address = parser.readElementText().trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setAddress(address);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
