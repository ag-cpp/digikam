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

#include "KmlDisplayNameTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataData.h"
#include "GeoDataSimpleField.h"
#include "GeoParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( displayName )

GeoNode* KmldisplayNameTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_displayName)));

    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Data ) ) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setDisplayName( displayName );
    }
    else if ( parentItem.represents( kmlTag_SimpleField ) ) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleField>()->setDisplayName( displayName );
    }
    return nullptr;
}

} // namespace kml

} // namespace Marble
