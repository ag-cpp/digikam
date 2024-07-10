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

#include <QObject>

// Local includes

#include "RenderState.h"

class QImage;
class QString;
class QSize;

namespace Marble
{

class GeoSceneAbstractTileProjection;
class MergedLayerDecorator;
class StackedTile;
class TileId;

class StackedTileLoaderPrivate;

/**
 * @short Tile loading from a quad tree
 *
 * This class loads tiles into memory. For faster access
 * we keep the tileIDs and their respective pointers to
 * the tiles in a hashtable.
 * The class also contains convenience methods to remove entries
 * from the hashtable and to return more detailed properties
 * about each tile level and their tiles.
 *
 * @author Torsten Rahn <rahn@kde.org>
 **/
class StackedTileLoader : public QObject
{
    Q_OBJECT

public:

    /**
     * Creates a new tile loader.
     *
     * @param mergedLayerDecorator The decorator that shall be used to decorate
     *                        the layer.
     * @param parent The parent widget.
     */
    explicit StackedTileLoader(MergedLayerDecorator* mergedLayerDecorator, QObject* parent = nullptr);
    ~StackedTileLoader() override;

    int tileColumnCount(int level) const;

    int tileRowCount(int level) const;

    const GeoSceneAbstractTileProjection* tileProjection() const;

    QSize tileSize() const;

    /**
     * Loads a tile and returns it.
     *
     * @param stackedTileId The Id of the requested tile, containing the x and y coordinate
     *                      and the zoom level.
     */
    const StackedTile* loadTile(TileId const& stackedTileId);

    /**
     * Resets the internal tile hash.
     */
    void resetTilehash();

    /**
     * Cleans up the internal tile hash.
     *
     * Removes all superfluous tiles from the hash.
     */
    void cleanupTilehash();

    /**
     * @brief  Returns the limit of the volatile (in RAM) cache.
     * @return the cache limit in kilobytes
     */
    quint64 volatileCacheLimit() const;

    /**
     * @brief Reloads the tiles that are currently displayed.
     */
    QList<TileId> visibleTiles() const;

    /**
     * @brief Return the number of tiles in the cache.
     * @return number of tiles in cache
     */
    int tileCount() const;

    /**
     * @brief Set the limit of the volatile (in RAM) cache.
     * @param kiloBytes The limit in kilobytes.
     */
    void setVolatileCacheLimit(quint64 kiloBytes);

    /**
     * Effectively triggers a reload of all tiles that are currently in use
     * and clears the tile cache in physical memory.
     */
    void clear();

    /**
     */
    void updateTile(TileId const& tileId, QImage const& tileImage);

    RenderState renderState() const;

Q_SIGNALS:

    void tileLoaded(TileId const& tileId);
    void cleared();

private:

    Q_DISABLE_COPY(StackedTileLoader)

private:

    friend class StackedTileLoaderPrivate;
    StackedTileLoaderPrivate* const d = nullptr;
};

} // namespace Marble
