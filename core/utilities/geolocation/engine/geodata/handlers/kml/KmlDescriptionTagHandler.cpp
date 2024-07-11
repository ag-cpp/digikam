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

#include "KmlDescriptionTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataFeature.h"
#include "GeoParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(description)

GeoNode* KmldescriptionTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_description)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>())
    {

        QString result;
        bool finished = false;
        bool isCDATA = false;

        while (!finished)
        {
            switch (parser.readNext())
            {
                case QXmlStreamReader::Characters:
                case QXmlStreamReader::EntityReference:
                {
                    result.append(parser.text());

                    if (parser.isCDATA())
                    {
                        isCDATA = true;
                    }

                    break;
                }

                case QXmlStreamReader::EndElement:
                    finished = true;
                    break;

                case QXmlStreamReader::ProcessingInstruction:
                case QXmlStreamReader::Comment:
                    break;

                default:
                    finished = true;
                    break;
            }
        }

        QString description = result.trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setDescription(description);
        parentItem.nodeAs<GeoDataFeature>()->setDescriptionCDATA(isCDATA);
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
