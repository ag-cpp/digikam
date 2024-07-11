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

#include "KmlPairTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataStyleMap.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(Pair)

GeoNode* KmlPairTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Pair)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_StyleMap))
    {
        return parentItem.nodeAs<GeoDataStyleMap>();
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
