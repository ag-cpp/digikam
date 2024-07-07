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

// Qt includes

#include <QList>

// Local includes

#include "GeoDataLatLonBox.h"
#include "MarblePhysics.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataPlacemark;
class GeoDataLookAt;
class MarbleMap;
class MarbleModel;
class ViewportParams;

class DIGIKAM_EXPORT MarbleAbstractPresenter : public QObject
{
    Q_OBJECT

Q_SIGNALS:

    void zoomChanged(int zoom);
    void distanceChanged(const QString& distanceString);

    /**
     * This signal is Q_EMIT when a new rectangle region is selected over the map.
     *
     * @param boundingBox The geographical coordinates of the selected region
     */
    void regionSelected(const GeoDataLatLonBox& boundingBox);

public:

    explicit MarbleAbstractPresenter(MarbleMap* map, QObject* parent = nullptr);
    ~MarbleAbstractPresenter() override;

    qreal moveStep() const;
    int radius() const;

    GeoDataLookAt lookAt() const;

    QString distanceString() const;

    /**
     * @brief Approximated altitude of the camera in km
     */
    qreal distance() const;

    /**
     * @brief An approximate distance from @p radius
     * @param radius radius of planet disc in screen pixels
     */
    qreal distanceFromRadius(qreal radius) const;

    /**
     * @brief The radius of the rendered planet disc derived from the approximate apparent @p distance
     */
    qreal radiusFromDistance(qreal distance) const;

    /**
      * @brief Rotate the globe in the given direction in discrete steps
      * @param stepsRight Number of steps to go right. Negative values go left.
      * @param stepsDown Number of steps to go down. Negative values go up.
      * @param mode Interpolation mode to use when traveling to the target
      */
    void moveByStep(int stepsRight, int stepsDown, FlyToMode mode = Automatic);

    int polarity() const;
    int zoom() const;
    int minimumZoom() const;
    int maximumZoom() const;

    qreal distanceFromZoom(qreal zoom) const;
    qreal zoomFromDistance(qreal distance) const;

    void zoomAt(const QPoint& pos, qreal newDistance);
    void moveTo(const QPoint& pos, qreal factor);

    qreal centerLongitude() const;
    qreal centerLatitude() const;

    ViewContext viewContext() const;

    qreal zoom(qreal radius) const;
    qreal radius(qreal zoom) const;

    MarbleMap* map();
    MarbleModel* model();

    const MarbleMap* map() const;
    const MarbleModel* model() const;

    int logzoom() const;
    void setLogzoom(int value);

    int zoomStep() const;
    qreal viewAngle() const;

    bool animationsEnabled() const;

    ViewportParams* viewport();
    const ViewportParams* viewport() const;

public Q_SLOTS:

    void rotateBy(const qreal deltaLon, const qreal deltaLat, FlyToMode mode = Instant);
    void flyTo(const GeoDataLookAt& newLookAt, FlyToMode mode = Automatic);
    void goHome(FlyToMode mode = Automatic);

    void setZoom(int newZoom, FlyToMode mode = Instant);
    void zoomView(int zoom, FlyToMode mode = Instant);
    void zoomViewBy(int zoomStep, FlyToMode mode = Instant);
    void zoomIn(FlyToMode mode = Automatic);
    void zoomOut(FlyToMode mode = Automatic);
    void zoomAtBy(const QPoint& pos, int zoomStep);

    void setViewContext(ViewContext viewContext);

    void centerOn(const qreal lon, const qreal lat, bool animated = false);
    void centerOn(const GeoDataCoordinates& point, bool animated = false);
    void centerOn(const GeoDataLatLonBox& box, bool animated = false);
    void centerOn(const GeoDataPlacemark& placemark, bool animated = false);
    void headingOn(qreal heading);
    void setCenterLatitude(qreal lat, FlyToMode mode);
    void setCenterLongitude(qreal lon, FlyToMode mode);

    void setAnimationsEnabled(bool enabled);
    void setRadius(int radius);
    void setDistance(qreal newDistance);
    void setSelection(const QRect& region);

private:

    MarbleMap* const m_map = nullptr;
    MarblePhysics    m_physics;

    bool             m_animationsEnabled;
    int              m_logzoom;
    int              m_zoomStep;
    const qreal      m_viewAngle;
};

} // Namespace Marble
