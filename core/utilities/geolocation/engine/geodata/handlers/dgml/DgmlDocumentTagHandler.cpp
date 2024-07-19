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

#include "DgmlDocumentTagHandler.h"

// Local includes

#include "DgmlElementDictionary.h"
#include "GeoSceneDocument.h"
#include "GeoSceneParser.h"
#include "digikam_debug.h"

namespace Marble
{

namespace dgml
{

DGML_DEFINE_TAG_HANDLER(Document)

GeoNode* DgmlDocumentTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Document)));
    return geoSceneDoc(parser);
}

} // namespace dgml

} // namespace Marble
