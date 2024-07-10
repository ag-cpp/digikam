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

#include "DgmlValueTagHandler.h"

// Local includes

#include "DgmlElementDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneProperty.h"
#include "digikam_debug.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Value)

GeoNode* DgmlValueTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Value)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(dgmlTag_Property))
    {
        QString parsedText = parser.readElementText().toLower().trimmed();
        parentItem.nodeAs<GeoSceneProperty>()->setDefaultValue(parsedText == QString::fromUtf8(dgmlValue_true) || parsedText == QString::fromUtf8(dgmlValue_on));
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble
