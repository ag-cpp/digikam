// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#pragma once

#include "GeoGraphicsItem.h"
#include "digikam_export.h"

#include <QImage>
#include <QColor>

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPlacemark;
class GeoDataPolygon;
class GeoDataBuilding;

class DIGIKAM_EXPORT AbstractGeoPolygonGraphicsItem : public GeoGraphicsItem
{
protected:
    AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);
    AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataBuilding *building);
    ~AbstractGeoPolygonGraphicsItem() override;

public:
    const GeoDataLatLonAltBox& latLonAltBox() const override;
    void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;
    bool contains(const QPoint &screenPosition, const ViewportParams *viewport) const override;

    void setLinearRing(GeoDataLinearRing* ring);
    void setPolygon(GeoDataPolygon* polygon);

    static const void *s_previousStyle;

protected:
    bool configurePainter(GeoPainter* painter, const ViewportParams &viewport) const;
    inline
    const GeoDataPolygon *polygon() const { return m_polygon; }
    inline
    const GeoDataLinearRing *ring() const { return m_ring; }
    inline
    const GeoDataBuilding *building() const { return m_building; }

    static int extractElevation(const GeoDataPlacemark &placemark);

private:

    QPixmap texture(const QString &path, const QColor &color) const;

    const GeoDataPolygon* m_polygon         = nullptr;
    const GeoDataLinearRing* m_ring         = nullptr;
    const GeoDataBuilding* const m_building = nullptr;
};

} // namespace Marble
