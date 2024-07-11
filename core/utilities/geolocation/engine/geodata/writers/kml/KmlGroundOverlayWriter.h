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

// Local includes

#include "GeoTagWriter.h"
#include "KmlOverlayTagWriter.h"
#include "MarbleGlobal.h"

namespace Marble
{

class KmlGroundOverlayWriter : public KmlOverlayTagWriter
{
public:

    KmlGroundOverlayWriter();

    bool writeMid(const GeoNode* node, GeoWriter& writer) const override;

    static QString altitudeModeToString(AltitudeMode mode);

    static void writeAltitudeMode(GeoWriter& writer, AltitudeMode altitudeMode);
};

} // namespace Marble

