// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlDeleteTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "GeoDataParser.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( Delete )

GeoNode* KmlDeleteTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Delete)));

    GeoDataDelete *dataDelete = new GeoDataDelete;
    KmlObjectTagHandler::parseIdentifiers( parser, dataDelete );
    GeoStackItem parentItem = parser.parentElement();

    if ( parentItem.represents( kmlTag_Update ) ) {
        parentItem.nodeAs<GeoDataUpdate>()->setDelete( dataDelete );
        return dataDelete;
    } else {
        delete dataDelete;
        return nullptr;
    }
}

}
}
