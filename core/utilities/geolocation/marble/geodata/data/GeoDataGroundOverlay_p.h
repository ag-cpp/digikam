// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#pragma once

#include "GeoDataOverlay_p.h"

#include "GeoDataTypes.h"
#include "GeoDataLatLonQuad.h"

namespace Marble {

class GeoDataGroundOverlayPrivate : public GeoDataOverlayPrivate
{
public:
    double m_altitude;

    AltitudeMode m_altitudeMode;

    GeoDataLatLonBox m_latLonBox;

    GeoDataLatLonQuad m_latLonQuad;

    GeoDataGroundOverlayPrivate();
};

GeoDataGroundOverlayPrivate::GeoDataGroundOverlayPrivate()
    : m_altitude(0.0),
      m_altitudeMode(ClampToGround)
{
}

} // namespace Marble
