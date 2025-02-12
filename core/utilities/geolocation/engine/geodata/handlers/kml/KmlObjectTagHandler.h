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

#pragma once

// Local includes

#include "GeoParser.h"
#include "GeoDataObject.h"

namespace Marble
{

class KmlObjectTagHandler
{
public:

    /**
     * Parses the id and targetId attributes and assign their values to the given object
     */
    static void parseIdentifiers(const GeoParser& parser, GeoDataObject* object);

private:

    KmlObjectTagHandler() = delete; // not implemented
};

} // namespace Marble
