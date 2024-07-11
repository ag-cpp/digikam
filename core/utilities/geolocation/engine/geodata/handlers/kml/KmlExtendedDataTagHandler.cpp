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

#include "KmlExtendedDataTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFeature.h"
#include "GeoDataTrack.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(ExtendedData)

GeoNode* KmlExtendedDataTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_ExtendedData)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>())
    {
        GeoDataExtendedData extendedData;
        parentItem.nodeAs<GeoDataFeature>()->setExtendedData(extendedData);
        return static_cast<GeoDataExtendedData*>(&parentItem.nodeAs<GeoDataFeature>()->extendedData());
    }

    else if (parentItem.is<GeoDataTrack>())
    {
        GeoDataExtendedData extendedData;
        parentItem.nodeAs<GeoDataTrack>()->setExtendedData(extendedData);
        return static_cast<GeoDataExtendedData*>(&parentItem.nodeAs<GeoDataTrack>()->extendedData());
    }

    else
    {
        return nullptr;
    }

}

} // namespace kml

} // namespace Marble
