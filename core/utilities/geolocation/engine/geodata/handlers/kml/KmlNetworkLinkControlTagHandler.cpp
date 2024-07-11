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

#include "KmlNetworkLinkControlTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(NetworkLinkControl)

GeoNode* KmlNetworkLinkControlTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_NetworkLinkControl)));

    GeoStackItem parentItem = parser.parentElement();
    GeoDataNetworkLinkControl* networkLinkControl = new GeoDataNetworkLinkControl;

    if (parentItem.qualifiedName().first == QString::fromUtf8(kmlTag_kml))
    {
        GeoDataDocument* doc = geoDataDoc(parser);
        doc->append(networkLinkControl);
        return networkLinkControl;
    }

    else
    {
        delete networkLinkControl;
        return nullptr;
    }
}

} // namespace kml

} // namespace Marble
