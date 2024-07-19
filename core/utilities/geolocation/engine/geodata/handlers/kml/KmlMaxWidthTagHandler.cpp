/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "KmlMaxWidthTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(maxWidth)

GeoNode* KmlmaxWidthTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_maxWidth)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ImagePyramid))
    {
        int maxWidth = parser.readElementText().toInt();

        parentItem.nodeAs<GeoDataImagePyramid>()->setMaxWidth(maxWidth);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
