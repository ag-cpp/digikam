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

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace kml
{

namespace gx
{

class KmlTimeSpanTagHandler : public GeoTagHandler
{
public:

    GeoNode* parse(GeoParser&) const override;
};

} // namespace gx

} // namespace kml

} // namespace Marble
