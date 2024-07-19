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

#include "GeoDocument.h"

// C++ includes

#include <cstdio>

// Local includes

#include "GeoSceneTypes.h"

namespace Marble
{

GeoDocument::GeoDocument()
{
}

GeoDocument::~GeoDocument()
{
}

bool GeoDocument::isGeoDataDocument() const
{
    return false;
}

bool GeoDocument::isGeoSceneDocument() const
{
    return false;
}

GeoNode::GeoNode()
{
}

GeoNode::~GeoNode()
{
}

} // namespace Marble
