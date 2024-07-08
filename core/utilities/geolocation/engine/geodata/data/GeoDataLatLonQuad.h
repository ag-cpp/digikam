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

#include "GeoDataCoordinates.h"
#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataLatLonQuadPrivate;

class DIGIKAM_EXPORT GeoDataLatLonQuad: public GeoDataObject
{
public:
    GeoDataLatLonQuad();

    GeoDataLatLonQuad(const GeoDataLatLonQuad& other);

    GeoDataLatLonQuad& operator=(const GeoDataLatLonQuad& other);
    bool operator==(const GeoDataLatLonQuad& other) const;
    bool operator!=(const GeoDataLatLonQuad& other) const;

    ~GeoDataLatLonQuad() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    qreal bottomLeftLatitude(GeoDataCoordinates::Unit unit) const;
    qreal bottomRightLatitude(GeoDataCoordinates::Unit unit) const;
    qreal topRightLatitude(GeoDataCoordinates::Unit unit) const;
    qreal topLeftLatitude(GeoDataCoordinates::Unit unit) const;

    qreal bottomLeftLongitude(GeoDataCoordinates::Unit unit) const;
    qreal bottomRightLongitude(GeoDataCoordinates::Unit unit) const;
    qreal topRightLongitude(GeoDataCoordinates::Unit unit) const;
    qreal topLeftLongitude(GeoDataCoordinates::Unit unit) const;

    void setBottomLeftLatitude(qreal latitude, GeoDataCoordinates::Unit unit);
    void setBottomRightLatitude(qreal latitude, GeoDataCoordinates::Unit unit);
    void setTopRightLatitude(qreal latitude, GeoDataCoordinates::Unit unit);
    void setTopLeftLatitude(qreal latitude, GeoDataCoordinates::Unit unit);

    void setBottomLeftLongitude(qreal longitude, GeoDataCoordinates::Unit unit);
    void setBottomRightLongitude(qreal longitude, GeoDataCoordinates::Unit unit);
    void setTopRightLongitude(qreal longitude, GeoDataCoordinates::Unit unit);
    void setTopLeftLongitude(qreal longitude, GeoDataCoordinates::Unit unit);

    GeoDataCoordinates& bottomLeft() const;
    void setBottomLeft(const GeoDataCoordinates& coordinates);

    GeoDataCoordinates& bottomRight() const;
    void setBottomRight(const GeoDataCoordinates& coordinates);

    GeoDataCoordinates& topRight() const;
    void setTopRight(const GeoDataCoordinates& coordinates);

    GeoDataCoordinates& topLeft() const;
    void setTopLeft(const GeoDataCoordinates& coordinates);

    bool isValid() const;

private:

    GeoDataLatLonQuadPrivate* const d = nullptr;
};

} // namespace Marble
