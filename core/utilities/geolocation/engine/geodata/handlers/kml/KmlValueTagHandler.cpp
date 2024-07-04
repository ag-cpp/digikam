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

#include "KmlValueTagHandler.h"

#include <QVariant>

#include "KmlElementDictionary.h"
#include "GeoDataData.h"
#include "GeoDataSimpleArrayData.h"
#include "GeoParser.h"

#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER( value )
KML_DEFINE_TAG_HANDLER_GX22( value )

GeoNode* KmlvalueTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_value)));

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_Data ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setValue( QVariant( value ) );
    } else if( parentItem.represents( kmlTag_SimpleArrayData ) ) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleArrayData>()->append( QVariant( value ) );
    }
    return nullptr;
}

} // namespace kml

} // namespace Marble
