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

#include "GeoDataGeometry_p.h"
#include "GeoDataTypes.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataLineStringPrivate : public GeoDataGeometryPrivate
{
public:

    explicit GeoDataLineStringPrivate(TessellationFlags f)
        :  m_rangeCorrected(nullptr),
           m_dirtyRange(true),
           m_dirtyBox(true),
           m_tessellationFlags(f),
           m_previousResolution(-1),
           m_level(-1)
    {
    }

    GeoDataLineStringPrivate()
        : m_rangeCorrected(nullptr),
          m_dirtyRange(true),
          m_dirtyBox(true)
    {
    }

    ~GeoDataLineStringPrivate() override
    {
        delete m_rangeCorrected;
    }

    GeoDataLineStringPrivate& operator=(const GeoDataLineStringPrivate& other)
    {
        GeoDataGeometryPrivate::operator=(other);
        m_vector            = other.m_vector;
        m_rangeCorrected    = nullptr;
        m_dirtyRange        = true;
        m_dirtyBox          = other.m_dirtyBox;
        m_tessellationFlags = other.m_tessellationFlags;

        return *this;
    }

    GeoDataGeometryPrivate* copy() const override
    {
        GeoDataLineStringPrivate* copy = new GeoDataLineStringPrivate;
        *copy                          = *this;

        return copy;
    }

    void toPoleCorrected(const GeoDataLineString& q, GeoDataLineString& poleCorrected) const;

    void toDateLineCorrected(const GeoDataLineString& q,
                             QVector<GeoDataLineString*>& lineStrings) const;

    void interpolateDateLine(const GeoDataCoordinates& previousCoords,
                             const GeoDataCoordinates& currentCoords,
                             GeoDataCoordinates& previousAtDateline,
                             GeoDataCoordinates& currentAtDateline,
                             TessellationFlags f) const;

    GeoDataCoordinates findDateLine(const GeoDataCoordinates& previousCoords,
                                    const GeoDataCoordinates& currentCoords,
                                    int recursionCounter) const;

    quint8 levelForResolution(qreal resolution) const;
    static qreal resolutionForLevel(int level);
    void optimize(GeoDataLineString& lineString) const;

public:

    QVector<GeoDataCoordinates> m_vector;

    mutable GeoDataLineString*  m_rangeCorrected = nullptr;
    mutable bool                m_dirtyRange;

    mutable bool                m_dirtyBox; // tells whether there have been changes to the
    // GeoDataPoints since the LatLonAltBox has
    // been calculated. Saves performance.
    TessellationFlags           m_tessellationFlags;
    mutable qreal               m_previousResolution;
    mutable quint8              m_level;
};

} // namespace Marble
