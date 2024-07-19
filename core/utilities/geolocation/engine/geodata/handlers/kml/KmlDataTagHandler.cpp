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

#include "KmlDataTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(Data)

GeoNode* KmlDataTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Data)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ExtendedData))
    {
        GeoDataData data;

        QString name = parser.attribute("name").trimmed();
        data.setName(name);
        parentItem.nodeAs< GeoDataExtendedData >()->addValue(data);
        return static_cast<GeoDataData*>(&parentItem.nodeAs<GeoDataExtendedData>()->valueRef(name));
    }

    else
    {
        return nullptr;
    }

}

} // namespace kml

} // namespace Marble
