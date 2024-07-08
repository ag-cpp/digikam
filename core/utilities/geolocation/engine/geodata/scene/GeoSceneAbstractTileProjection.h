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

#include <QRect>
#include <QScopedPointer>

// Local includes

#include "digikam_export.h"

namespace Marble
{

class GeoSceneAbstractTileProjectionPrivate;

class GeoDataLatLonBox;
class TileId;

/**
 * @short A base class for projections between tile indizes and geo coordinates in Marble.
 *
 * For map tiling with indizes in x and y dimensions and 1 or multiple zoomlevels.
 * The lowest zoomlevel is 0.
 */
class DIGIKAM_EXPORT GeoSceneAbstractTileProjection
{
public:

    enum Type
    {
        Equirectangular,
        Mercator
    };

    /**
     * @brief Construct a new GeoSceneAbstractTileProjection.
     */
    GeoSceneAbstractTileProjection();

    virtual ~GeoSceneAbstractTileProjection();

public:

    virtual GeoSceneAbstractTileProjection::Type type() const = 0;

    /**
     * @return the number of tiles on level 0 in x dimension
     */
    int levelZeroColumns() const;
    /**
     * @brief Sets the number of tiles on level 0 in x dimension
     *
     * @param levelZeroColumns new number of tiles on level 0 in x dimension
     *
     * Default value of the levelZeroColumns property is 1.
     */
    void setLevelZeroColumns(int levelZeroColumns);

    /**
     * @return the number of tiles on level 0 in y dimension
     */
    int levelZeroRows() const;
    /**
     * @brief Sets the number of tiles on level 0 in y dimension
     *
     * @param levelZeroRows new number of tiles on level 0 in y dimension
     *
     * Default value of the levelZeroRows property is 1.
     */
    void setLevelZeroRows(int levelZeroRows);

    /**
     * @brief Get the tile indexes which cover the given geographical box.
     * If @p latLonBox or @p zoomLevel have values out-of-bounds, the behaviour is undefined.
     *
     * @param latLonBox the geo coordinates of the requested tiles
     * @param zoomLevel the zoomlevel of the requested tiles
     *
     * @return range of tile indexes covering given geographical box at given zoom level
     */
    virtual QRect tileIndexes(const GeoDataLatLonBox& latLonBox, int zoomLevel) const = 0;

    /**
     * @brief Get the boundary geo coordinates corresponding to a tile.
     * If @p x, @p y or @p zoomLevel have values out-of-bounds, the behaviour is undefined.
     *
     * @param zoomLevel the zoomlevel of the tile
     * @param x         the x index of the tile
     * @param y         the y index of the tile
     *
     * @return geographic bounding box covered by the given tile
     */
    virtual GeoDataLatLonBox geoCoordinates(int zoomLevel, int x, int y) const = 0;

    /**
     * @brief Get the boundary geo coordinates corresponding to a tile.
     * If @p tildId  has values out-of-bounds, the behaviour is undefined.
     *
     * @param tileId    the id of the tile
     *
     * @return geographic bounding box covered by the given tile
     */
    GeoDataLatLonBox geoCoordinates(const TileId& tileId) const;

private:

    Q_DISABLE_COPY(GeoSceneAbstractTileProjection)

    const QScopedPointer<GeoSceneAbstractTileProjectionPrivate> d_ptr;
};

} // namespace Marble
