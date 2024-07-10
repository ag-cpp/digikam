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

#include "DgmlHeadingTagHandler.h"

// Local includes

#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneSection.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Heading)

GeoNode* DgmlHeadingTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Heading)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(dgmlTag_Section))
    {
        parentItem.nodeAs<GeoSceneSection>()->setHeading(parser.readElementText().trimmed());
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble
