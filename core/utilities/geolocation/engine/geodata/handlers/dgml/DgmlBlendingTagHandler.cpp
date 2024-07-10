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

#include "DgmlBlendingTagHandler.h"

// Local includes

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"
#include "digikam_debug.h"

namespace Marble
{

namespace dgml
{

static GeoTagHandlerRegistrar registrar(GeoParser::QualifiedName(QString::fromUtf8(dgmlTag_Blending),
                                                                 QString::fromUtf8(dgmlTag_nameSpace20)),
                                        new DgmlBlendingTagHandler);

GeoNode* DgmlBlendingTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Blending)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile))
    {
        return nullptr;
    }

    // Attribute name, default to ""
    const QString name = parser.attribute(dgmlAttr_name).trimmed();
    qCDebug(DIGIKAM_MARBLE_LOG) << "DgmlBlendingTagHandler::parse" << name;
    parentItem.nodeAs<GeoSceneTileDataset>()->setBlending(name);
    return nullptr;
}

} // namespace dgml

} // namespace Marble
