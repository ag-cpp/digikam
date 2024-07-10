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

#include "DgmlTileSizeTagHandler.h"

// Local includes

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(TileSize)

GeoNode* DgmlTileSizeTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_TileSize)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile))
    {
        return nullptr;
    }

    int width = parser.attribute(dgmlAttr_width).toInt();
    int height = parser.attribute(dgmlAttr_height).toInt();
    QSize const size(width, height);

    if (!size.isEmpty())
    {
        parentItem.nodeAs<GeoSceneTileDataset>()->setTileSize(size);
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble
