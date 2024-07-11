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

#include "KmlplayModeTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataTourControl.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER_GX22(playMode)

GeoNode* KmlplayModeTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_playMode)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataTourControl>())
    {
        GeoDataTourControl* control = parentItem.nodeAs<GeoDataTourControl>();

        if (parser.readElementText().trimmed().toLower() == QLatin1String("play"))
        {
            control->setPlayMode(GeoDataTourControl::Play);
        }

        else
        {
            control->setPlayMode(GeoDataTourControl::Pause);
        }
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
