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

#include "AbstractProjection_p.h"

namespace Marble
{

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;

class AzimuthalProjection;

class Q_DECL_HIDDEN AzimuthalProjectionPrivate : public AbstractProjectionPrivate
{
public:

    explicit AzimuthalProjectionPrivate(AzimuthalProjection* parent)
        : AbstractProjectionPrivate(parent),
          q_ptr(parent)
    {
    }

    ~AzimuthalProjectionPrivate() override {};

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    // In order to ensure proper performance the current algorithm
    // determines whether the linestring disappears
    // behind the horizon by evaluating the actual node coordinates of the
    // linestring.
    // However in some cases only a tessellated portion of the linestring
    // disappears behind the globe. In this case non-closed linestrings
    // can still be cut into separate polygons without problems.
    // But for linearrings the horizon detection at this stage happens too
    // late already to be taken into account for rendering.
    // The allowLatePolygonCut parameter allows to split at least
    // non-closed linestrings properly at this point.

    void tessellateLineSegment(const GeoDataCoordinates& aCoords,
                               qreal ax, qreal ay,
                               const GeoDataCoordinates& bCoords,
                               qreal bx, qreal by,
                               QVector<QPolygonF*>& polygons,
                               const ViewportParams* viewport,
                               TessellationFlags f = TessellationFlags(),
                               bool allowLatePolygonCut = false) const;

    void processTessellation(const GeoDataCoordinates& previousCoords,
                             const GeoDataCoordinates& currentCoords,
                             int count,
                             QVector<QPolygonF*>& polygons,
                             const ViewportParams* viewport,
                             TessellationFlags f = TessellationFlags(),
                             bool allowLatePolygonCut = false) const;

    void crossHorizon(const GeoDataCoordinates& bCoord,
                      QVector<QPolygonF*>& polygons,
                      const ViewportParams* viewport,
                      bool allowLatePolygonCut = false
                     ) const;

    virtual bool lineStringToPolygon(const GeoDataLineString& lineString,
                                     const ViewportParams* viewport,
                                     QVector<QPolygonF*>& polygons) const;

    void horizonToPolygon(const ViewportParams* viewport,
                          const GeoDataCoordinates& disappearCoords,
                          const GeoDataCoordinates& reappearCoords,
                          QPolygonF*) const;

    GeoDataCoordinates findHorizon(const GeoDataCoordinates& previousCoords,
                                   const GeoDataCoordinates& currentCoords,
                                   const ViewportParams* viewport,
                                   TessellationFlags f = TessellationFlags()) const;

    GeoDataCoordinates doFindHorizon(const GeoDataCoordinates& previousCoords,
                                     const GeoDataCoordinates& currentCoords,
                                     const ViewportParams* viewport,
                                     TessellationFlags f,
                                     bool currentHide,
                                     int recursionCounter) const;

    bool globeHidesPoint(const GeoDataCoordinates& coordinates,
                         const ViewportParams* viewport) const;

    Q_DECLARE_PUBLIC(AzimuthalProjection)

public:

    AzimuthalProjection* const q_ptr = nullptr;
};

} // namespace Marble
