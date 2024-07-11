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

#include "KmlVisibilityTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(visibility)

GeoNode* KmlvisibilityTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_visibility)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>())
    {
        QString visibility = parser.readElementText().trimmed();
        const bool visible = (visibility == QLatin1String("1"));
        parentItem.nodeAs<GeoDataFeature>()->setVisible(visible);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
