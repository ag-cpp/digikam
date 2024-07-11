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

#include "KmlScaleTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"
#include "GeoDataScale.h"
#include "GeoDataGeometry.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(Scale)

GeoNode* KmlScaleTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Scale)));

    GeoDataScale scale;
    KmlObjectTagHandler::parseIdentifiers(parser, &scale);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Model))
    {
        parentItem.nodeAs<GeoDataModel>()->setScale(scale);
        return &parentItem.nodeAs<GeoDataModel>()->scale();
    }

    else
    {
        return nullptr;
    }
}

} // namespace kml

} // namespace Marble
