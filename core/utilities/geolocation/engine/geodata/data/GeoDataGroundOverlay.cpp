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

#include "GeoDataGroundOverlay.h"
#include "GeoDataGroundOverlay_p.h"

namespace Marble
{

GeoDataGroundOverlay::GeoDataGroundOverlay()
    : GeoDataOverlay(new GeoDataGroundOverlayPrivate)
{
    // nothing to do
}

GeoDataGroundOverlay::GeoDataGroundOverlay(const GeoDataGroundOverlay& other)
    : GeoDataOverlay(other, new GeoDataGroundOverlayPrivate(*other.d_func()))
{
    // nothing to do
}

GeoDataGroundOverlay& GeoDataGroundOverlay::operator=(const GeoDataGroundOverlay& other)
{
    if (this != &other)
    {
        Q_D(GeoDataGroundOverlay);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataGroundOverlay::operator==(const GeoDataGroundOverlay& other) const
{
    Q_D(const GeoDataGroundOverlay);
    const GeoDataGroundOverlayPrivate* const other_d = other.d_func();

    return equals(other) &&
           d->m_altitude == other_d->m_altitude &&
           d->m_altitudeMode == other_d->m_altitudeMode &&
           d->m_latLonBox == other_d->m_latLonBox &&
           d->m_latLonQuad == other_d->m_latLonQuad;
}

bool GeoDataGroundOverlay::operator!=(const GeoDataGroundOverlay& other) const
{
    return !this->operator==(other);
}

GeoDataGroundOverlay::~GeoDataGroundOverlay()
{
}

GeoDataFeature* GeoDataGroundOverlay::clone() const
{
    return new GeoDataGroundOverlay(*this);
}

const char* GeoDataGroundOverlay::nodeType() const
{
    return GeoDataTypes::GeoDataGroundOverlayType;
}

double GeoDataGroundOverlay::altitude() const
{
    Q_D(const GeoDataGroundOverlay);
    return d->m_altitude;
}

void GeoDataGroundOverlay::setAltitude(double altitude)
{
    Q_D(GeoDataGroundOverlay);
    d->m_altitude = altitude;
}

AltitudeMode GeoDataGroundOverlay::altitudeMode() const
{
    Q_D(const GeoDataGroundOverlay);
    return d->m_altitudeMode;
}

void GeoDataGroundOverlay::setAltitudeMode(const AltitudeMode altitudeMode)
{
    Q_D(GeoDataGroundOverlay);
    d->m_altitudeMode = altitudeMode;
}

GeoDataLatLonBox& GeoDataGroundOverlay::latLonBox()
{
    Q_D(GeoDataGroundOverlay);
    return d->m_latLonBox;
}

const GeoDataLatLonBox& GeoDataGroundOverlay::latLonBox() const
{
    Q_D(const GeoDataGroundOverlay);
    return d->m_latLonBox;
}

void GeoDataGroundOverlay::setLatLonBox(const GeoDataLatLonBox& box)
{
    Q_D(GeoDataGroundOverlay);
    d->m_latLonBox = box;
}

GeoDataLatLonQuad& GeoDataGroundOverlay::latLonQuad()
{
    Q_D(GeoDataGroundOverlay);
    return d->m_latLonQuad;
}

const GeoDataLatLonQuad& GeoDataGroundOverlay::latLonQuad() const
{
    Q_D(const GeoDataGroundOverlay);
    return d->m_latLonQuad;
}

void GeoDataGroundOverlay::setLatLonQuad(const GeoDataLatLonQuad& quad)
{
    Q_D(GeoDataGroundOverlay);
    d->m_latLonQuad = quad;
}

} // namespace Marble
