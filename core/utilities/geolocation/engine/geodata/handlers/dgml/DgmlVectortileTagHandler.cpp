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

#include "DgmlVectortileTagHandler.h"

// C++ includes

#include <limits>

// Local includes

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneLayer.h"
#include "GeoSceneVectorTileDataset.h"
#include "digikam_debug.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Vectortile)

GeoNode* DgmlVectortileTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Vectortile)));

    const QString name = parser.attribute(dgmlAttr_name).trimmed();

    const QString expireStr = parser.attribute(dgmlAttr_expire).trimmed();
    int expire = std::numeric_limits<int>::max();

    if (!expireStr.isEmpty())
    {
        expire = expireStr.toInt();
    }

    GeoSceneTileDataset* texture = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type
    // matches the backend of the parent layer
    if (parentItem.represents(dgmlTag_Layer)
        && parentItem.nodeAs<GeoSceneLayer>()->backend() == QString::fromUtf8(dgmlValue_vectortile))
    {

        texture = new GeoSceneVectorTileDataset(name);
        texture->setExpire(expire);
        parentItem.nodeAs<GeoSceneLayer>()->addDataset(texture);
    }

    return texture;
}

} // namespace dgml

} // namespace Marble
