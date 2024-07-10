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

#include "DgmlLicenseTagHandler.h"

// Local includes

#include "GeoSceneLicense.h"
#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "digikam_debug.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(License)

GeoNode* DgmlLicenseTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_License)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(dgmlTag_Head))
    {
        QString const attribution = parser.attribute(dgmlAttr_attribution).trimmed().toLower();

        if (attribution == QLatin1String("never"))
        {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::Never);
        }

        else if (attribution == QLatin1String("opt-in") || attribution == QLatin1String("optin"))
        {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptIn);
        }

        else if (attribution.isEmpty() || attribution == QLatin1String("opt-out") || attribution == QLatin1String("optout"))
        {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptOut);
        }

        else if (attribution == QLatin1String("always"))
        {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::Always);
        }

        else
        {
            qCDebug(DIGIKAM_MARBLE_LOG) << "Unknown license attribution value " << attribution << ", falling back to 'opt-out'.";
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptOut);
        }

        QString const shortLicense = parser.attribute(dgmlAttr_short).trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setShortLicense(shortLicense);
        QString const fullLicense = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setLicense(fullLicense);
    }

    return nullptr;
}

} // namespace dgml

} // namespace Marble
