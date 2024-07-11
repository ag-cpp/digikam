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

#include "KmldelayedStartTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataSoundCue.h"
#include "GeoDataAnimatedUpdate.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(delayedStart)

GeoNode* KmldelayedStartTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_delayedStart)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataSoundCue>())
    {
        double const delay = parser.readElementText().toDouble();
        parentItem.nodeAs<GeoDataSoundCue>()->setDelayedStart(delay);
    }

    else if (parentItem.is<GeoDataAnimatedUpdate>())
    {
        double const delay = parser.readElementText().toDouble();
        parentItem.nodeAs<GeoDataAnimatedUpdate>()->setDelayedStart(delay);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
