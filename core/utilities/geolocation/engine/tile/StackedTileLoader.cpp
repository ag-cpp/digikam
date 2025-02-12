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

#include "StackedTileLoader.h"

// Qt includes

#include <QCache>
#include <QHash>
#include <QReadWriteLock>
#include <QImage>

// Local includes

#include "MergedLayerDecorator.h"
#include "StackedTile.h"
#include "TileLoader.h"
#include "TileLoaderHelper.h"
#include "MarbleGlobal.h"
#include "digikam_debug.h"

namespace Marble
{

class Q_DECL_HIDDEN StackedTileLoaderPrivate
{
public:

    explicit StackedTileLoaderPrivate(MergedLayerDecorator* mergedLayerDecorator)
        : m_layerDecorator(mergedLayerDecorator)
    {
        m_tileCache.setMaxCost(20000 * 1024);   // Cache size measured in bytes
    }

    MergedLayerDecorator* const   m_layerDecorator = nullptr;
    QHash <TileId, StackedTile*>  m_tilesOnDisplay;
    QCache <TileId, StackedTile>  m_tileCache;
    QReadWriteLock                m_cacheLock;
};

StackedTileLoader::StackedTileLoader(MergedLayerDecorator* mergedLayerDecorator, QObject* parent)
    : QObject(parent),
      d(new StackedTileLoaderPrivate(mergedLayerDecorator))
{
}

StackedTileLoader::~StackedTileLoader()
{
    qDeleteAll(d->m_tilesOnDisplay);
    delete d;
}

int StackedTileLoader::tileColumnCount(int level) const
{
    return d->m_layerDecorator->tileColumnCount(level);
}

int StackedTileLoader::tileRowCount(int level) const
{
    return d->m_layerDecorator->tileRowCount(level);
}

const GeoSceneAbstractTileProjection* StackedTileLoader::tileProjection() const
{
    return d->m_layerDecorator->tileProjection();
}

QSize StackedTileLoader::tileSize() const
{
    return d->m_layerDecorator->tileSize();
}

void StackedTileLoader::resetTilehash()
{
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();

    for (; it != end; ++it)
    {
        Q_ASSERT(it.value()->used() && "contained in m_tilesOnDisplay should imply used()");
        it.value()->setUsed(false);
    }
}

void StackedTileLoader::cleanupTilehash()
{
    // Make sure that tiles which haven't been used during the last
    // rendering of the map at all get removed from the tile hash.

    QHashIterator<TileId, StackedTile*> it(d->m_tilesOnDisplay);

    while (it.hasNext())
    {
        it.next();

        if (!it.value()->used())
        {
            // If insert call result is false then the cache is too small to store the tile
            // but the item will get deleted nevertheless and the pointer we have
            // doesn't get set to zero (so don't delete it in this case or it will crash!)
            d->m_tileCache.insert(it.key(), it.value(), it.value()->byteCount());
            d->m_tilesOnDisplay.remove(it.key());
        }
    }
}

const StackedTile* StackedTileLoader::loadTile(TileId const& stackedTileId)
{
    // check if the tile is in the hash
    d->m_cacheLock.lockForRead();
    StackedTile* stackedTile = d->m_tilesOnDisplay.value(stackedTileId, 0);
    d->m_cacheLock.unlock();

    if (stackedTile)
    {
        stackedTile->setUsed(true);
        return stackedTile;
    }

    // here ends the performance critical section of this method

    d->m_cacheLock.lockForWrite();

    // has another thread loaded our tile due to a race condition?
    stackedTile = d->m_tilesOnDisplay.value(stackedTileId, 0);

    if (stackedTile)
    {
        Q_ASSERT(stackedTile->used() && "other thread should have marked tile as used");
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // the tile was not in the hash so check if it is in the cache
    stackedTile = d->m_tileCache.take(stackedTileId);

    if (stackedTile)
    {
        Q_ASSERT(!stackedTile->used() && "tiles in m_tileCache are invisible and should thus be marked as unused");
        stackedTile->setUsed(true);
        d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
        d->m_cacheLock.unlock();
        return stackedTile;
    }

    // tile (valid) has not been found in hash or cache, so load it from disk
    // and place it in the hash from where it will get transferred to the cache

    qCDebug(DIGIKAM_MARBLE_LOG) << "load tile from disk:" << stackedTileId;

    stackedTile = d->m_layerDecorator->loadTile(stackedTileId);
    Q_ASSERT(stackedTile);
    stackedTile->setUsed(true);

    d->m_tilesOnDisplay[ stackedTileId ] = stackedTile;
    d->m_cacheLock.unlock();

    Q_EMIT tileLoaded(stackedTileId);

    return stackedTile;
}

quint64 StackedTileLoader::volatileCacheLimit() const
{
    return d->m_tileCache.maxCost() / 1024;
}

QList<TileId> StackedTileLoader::visibleTiles() const
{
    return d->m_tilesOnDisplay.keys();
}

int StackedTileLoader::tileCount() const
{
    return d->m_tileCache.count() + d->m_tilesOnDisplay.count();
}

void StackedTileLoader::setVolatileCacheLimit(quint64 kiloBytes)
{
    qCDebug(DIGIKAM_MARBLE_LOG) << QString::fromUtf8("Setting tile cache to %1 kilobytes.").arg(kiloBytes);
    d->m_tileCache.setMaxCost(kiloBytes * 1024);
}

void StackedTileLoader::updateTile(TileId const& tileId, QImage const& tileImage)
{
    const TileId stackedTileId(0, tileId.zoomLevel(), tileId.x(), tileId.y());

    StackedTile* displayedTile = d->m_tilesOnDisplay.take(stackedTileId);

    if (displayedTile)
    {
        Q_ASSERT(!d->m_tileCache.contains(stackedTileId));

        StackedTile* const stackedTile = d->m_layerDecorator->updateTile(*displayedTile, tileId, tileImage);
        stackedTile->setUsed(true);
        d->m_tilesOnDisplay.insert(stackedTileId, stackedTile);

        delete displayedTile;
        displayedTile = nullptr;

        Q_EMIT tileLoaded(stackedTileId);
    }

    else
    {
        d->m_tileCache.remove(stackedTileId);
    }
}

RenderState StackedTileLoader::renderState() const
{
    RenderState renderState(QString::fromUtf8("Stacked Tiles"));
    QHash<TileId, StackedTile*>::const_iterator it = d->m_tilesOnDisplay.constBegin();
    QHash<TileId, StackedTile*>::const_iterator const end = d->m_tilesOnDisplay.constEnd();

    for (; it != end; ++it)
    {
        renderState.addChild(d->m_layerDecorator->renderState(it.key()));
    }

    return renderState;
}

void StackedTileLoader::clear()
{
    qDeleteAll(d->m_tilesOnDisplay);
    d->m_tilesOnDisplay.clear();
    d->m_tileCache.clear(); // clear the tile cache in physical memory

    Q_EMIT cleared();
}

} // namespace Marble

#include "moc_StackedTileLoader.cpp"
