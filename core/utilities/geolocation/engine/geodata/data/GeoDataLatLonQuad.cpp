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

#include "GeoDataLatLonQuad.h"

// Local includes

#include "GeoDataTypes.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataLatLonQuadPrivate
{
public:

    GeoDataCoordinates m_bottomLeft;
    GeoDataCoordinates m_bottomRight;
    GeoDataCoordinates m_topRight;
    GeoDataCoordinates m_topLeft;

    GeoDataLatLonQuadPrivate();
};

GeoDataLatLonQuadPrivate::GeoDataLatLonQuadPrivate() :
    m_bottomLeft(), m_bottomRight(), m_topRight(), m_topLeft()
{
    // nothing to do
}

GeoDataLatLonQuad::GeoDataLatLonQuad() : GeoDataObject(), d(new GeoDataLatLonQuadPrivate)
{
    // nothing to do
}

GeoDataLatLonQuad::GeoDataLatLonQuad(const Marble::GeoDataLatLonQuad& other) :
    GeoDataObject(other), d(new GeoDataLatLonQuadPrivate(*other.d))
{
    // nothing to do
}

GeoDataLatLonQuad& GeoDataLatLonQuad::operator=(const GeoDataLatLonQuad& other)
{
    *d = *other.d;
    return *this;
}

bool GeoDataLatLonQuad::operator==(const GeoDataLatLonQuad& other) const
{
    return equals(other)
           && d->m_bottomLeft == other.d->m_bottomLeft
           && d->m_bottomRight == other.d->m_bottomRight
           && d->m_topLeft == other.d->m_topLeft
           && d->m_topRight == other.d->m_topRight;
}

bool GeoDataLatLonQuad::operator!=(const GeoDataLatLonQuad& other) const
{
    return !this->operator==(other);
}

GeoDataLatLonQuad::~GeoDataLatLonQuad()
{
    delete d;
}

const char* GeoDataLatLonQuad::nodeType() const
{
    return GeoDataTypes::GeoDataLatLonQuadType;
}

qreal GeoDataLatLonQuad::bottomLeftLatitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_bottomLeft.latitude(unit);
}

void GeoDataLatLonQuad::setBottomLeftLatitude(qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_bottomLeft.setLatitude(latitude, unit);
}

qreal GeoDataLatLonQuad::bottomLeftLongitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_bottomLeft.longitude(unit);
}

void GeoDataLatLonQuad::setBottomLeftLongitude(qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_bottomLeft.setLongitude(longitude, unit);
}

qreal GeoDataLatLonQuad::bottomRightLatitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_bottomRight.latitude(unit);
}

void GeoDataLatLonQuad::setBottomRightLatitude(qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_bottomRight.setLatitude(latitude, unit);
}

qreal GeoDataLatLonQuad::bottomRightLongitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_bottomRight.longitude(unit);
}

void GeoDataLatLonQuad::setBottomRightLongitude(qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_bottomRight.setLongitude(longitude, unit);
}

qreal GeoDataLatLonQuad::topRightLatitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_topRight.latitude(unit);
}

void GeoDataLatLonQuad::setTopRightLatitude(qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_topRight.setLatitude(latitude, unit);
}

qreal GeoDataLatLonQuad::topRightLongitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_topRight.longitude(unit);
}

void GeoDataLatLonQuad::setTopRightLongitude(qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_topRight.setLongitude(longitude, unit);
}

qreal GeoDataLatLonQuad::topLeftLatitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_topLeft.latitude(unit);
}

void GeoDataLatLonQuad::setTopLeftLatitude(qreal latitude, GeoDataCoordinates::Unit unit)
{
    d->m_topLeft.setLatitude(latitude, unit);
}

qreal GeoDataLatLonQuad::topLeftLongitude(GeoDataCoordinates::Unit unit) const
{
    return d->m_topLeft.longitude(unit);
}

void GeoDataLatLonQuad::setTopLeftLongitude(qreal longitude, GeoDataCoordinates::Unit unit)
{
    d->m_topLeft.setLongitude(longitude, unit);
}


GeoDataCoordinates& GeoDataLatLonQuad::bottomLeft() const
{
    return d->m_bottomLeft;
}

void GeoDataLatLonQuad::setBottomLeft(const GeoDataCoordinates& coordinates)
{
    d->m_bottomLeft = coordinates;
}
GeoDataCoordinates& GeoDataLatLonQuad::bottomRight() const
{
    return d->m_bottomRight;
}

void GeoDataLatLonQuad::setBottomRight(const GeoDataCoordinates& coordinates)
{
    d->m_bottomRight = coordinates;
}

GeoDataCoordinates& GeoDataLatLonQuad::topRight() const
{
    return d->m_topRight;
}

void GeoDataLatLonQuad::setTopRight(const GeoDataCoordinates& coordinates)
{
    d->m_topRight = coordinates;
}

GeoDataCoordinates& GeoDataLatLonQuad::topLeft() const
{
    return d->m_topLeft;
}

void GeoDataLatLonQuad::setTopLeft(const GeoDataCoordinates& coordinates)
{
    d->m_topLeft = coordinates;
}

bool GeoDataLatLonQuad::isValid() const
{
    return d->m_bottomLeft.isValid() && d->m_bottomRight.isValid()
           && d->m_topLeft.isValid() && d->m_topRight.isValid();
}

} // namespace Marble
