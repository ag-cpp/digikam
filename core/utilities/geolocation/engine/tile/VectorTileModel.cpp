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

#include "VectorTileModel.h"

// Qt includes

#include <qmath.h>
#include <QThreadPool>

// Local includes

#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "GeoSceneVectorTileDataset.h"
#include "MarbleGlobal.h"
#include "MathHelper.h"
#include "TileLoader.h"
#include "digikam_debug.h"

namespace Marble
{

TileRunner::TileRunner(TileLoader* loader, const GeoSceneVectorTileDataset* tileDataset, const TileId& id)
    : m_loader(loader),
      m_tileDataset(tileDataset),
      m_id(id)
{
}

void TileRunner::run()
{
    GeoDataDocument* const document = m_loader->loadTileVectorData(m_tileDataset, m_id, DownloadBrowse);

    Q_EMIT documentLoaded(m_id, document);
}

VectorTileModel::CacheDocument::CacheDocument(GeoDataDocument* doc, VectorTileModel* vectorTileModel,
                                              const GeoDataLatLonBox& boundingBox)
    : m_document(doc),
      m_vectorTileModel(vectorTileModel),
      m_boundingBox(boundingBox)
{
    // nothing to do
}

VectorTileModel::CacheDocument::~CacheDocument()
{
    m_vectorTileModel->removeTile(m_document);
}

VectorTileModel::VectorTileModel(TileLoader* loader, const GeoSceneVectorTileDataset* layer,
                                 GeoDataTreeModel* treeModel, QThreadPool* threadPool)
    : m_loader(loader),
      m_layer(layer),
      m_treeModel(treeModel),
      m_threadPool(threadPool),
      m_tileLoadLevel(-1),
      m_tileZoomLevel(-1),
      m_deleteDocumentsLater(false)
{
    Q_UNUSED(m_treeModel);

    connect(this, SIGNAL(tileAdded(GeoDataDocument*)),
            treeModel, SLOT(addDocument(GeoDataDocument*)));

    connect(this, SIGNAL(tileRemoved(GeoDataDocument*)),
            treeModel, SLOT(removeDocument(GeoDataDocument*)));

    connect(treeModel, SIGNAL(removed(GeoDataObject*)),
            this, SLOT(cleanupTile(GeoDataObject*)));
}

void VectorTileModel::setViewport(const GeoDataLatLonBox& latLonBox)
{
    bool const smallScreen   = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const nTiles         = smallScreen ? 12 : 20;
    qreal const viewportArea = latLonBox.width() * latLonBox.height();
    qreal const level        = log((nTiles * 2.0 * M_PI * M_PI) / viewportArea) / log(4);
    m_tileZoomLevel          = qFloor(level);
    int tileLoadLevel        = m_tileZoomLevel;

    // Determine available tile levels in the layer and thereby
    // select the tileZoomLevel that is actually used:

    QVector<int> tileLevels  = m_layer->tileLevels();

    if (tileLevels.isEmpty() /* || tileZoomLevel < tileLevels.first() */)
    {
        // if there is no (matching) tile level then show nothing
        // and bail out.

        m_documents.clear();

        return;
    }

    int tileLevel = tileLevels.first();

    for (int i = 1, n = tileLevels.size() ; i < n ; ++i)
    {
        if (tileLevels[i] > tileLoadLevel)
        {
            break;
        }

        tileLevel = tileLevels[i];
    }

    tileLoadLevel = tileLevel;

    // if zoom level has changed, empty vectortile cache

    if (tileLoadLevel != m_tileLoadLevel)
    {
        m_deleteDocumentsLater = (m_tileLoadLevel >= 0);
        m_tileLoadLevel        = tileLoadLevel;
    }

    /** LOGIC FOR DOWNLOADING ALL THE TILES THAT ARE INSIDE THE SCREEN AT THE CURRENT ZOOM LEVEL **/

    // New tiles X and Y for moved screen coordinates
    // More info: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Subtiles
    // More info: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B

    const QRect rect = m_layer->tileProjection()->tileIndexes(latLonBox, tileLoadLevel);

    // Download tiles and send them to VectorTileLayer
    // When changing zoom, download everything inside the screen
    // TODO: hardcodes assumption about tiles indexing also ends at dateline
    // TODO: what about crossing things in y direction?

    if (!latLonBox.crossesDateLine())
    {
        queryTiles(tileLoadLevel, rect);
    }
    else     // When only moving screen, just download the new tiles
    {
        // TODO: maxTileX (calculation knowledge) should be a property of tileProjection or m_layer

        const int maxTileX = (1 << tileLoadLevel) * m_layer->levelZeroColumns() - 1;

        queryTiles(tileLoadLevel, QRect(QPoint(0, rect.top()), rect.bottomRight()));
        queryTiles(tileLoadLevel, QRect(rect.topLeft(), QPoint(maxTileX, rect.bottom())));
    }

    removeTilesOutOfView(latLonBox);
}

void VectorTileModel::removeTilesOutOfView(const GeoDataLatLonBox& boundingBox)
{
    GeoDataLatLonBox const extendedViewport = boundingBox.scaled(2.0, 2.0);

    for (auto iter = m_documents.begin() ; iter != m_documents.end() ; )
    {
        bool const isOutOfView = !extendedViewport.intersects(iter.value()->latLonBox());

        if (isOutOfView)
        {
            iter = m_documents.erase(iter);
        }

        else
        {
            ++iter;
        }
    }
}

QString VectorTileModel::name() const
{
    return m_layer->name();
}

const GeoSceneVectorTileDataset* VectorTileModel::layer() const
{
    return m_layer;
}

void VectorTileModel::removeTile(GeoDataDocument* document)
{
    Q_EMIT tileRemoved(document);
}

int VectorTileModel::tileZoomLevel() const
{
    return m_tileZoomLevel;
}

int VectorTileModel::cachedDocuments() const
{
    return m_documents.size();
}

void VectorTileModel::reload()
{
    const auto keys = m_documents.keys();

    for (auto const& tile : keys)
    {
        m_loader->downloadTile(m_layer, tile, DownloadBrowse);
    }
}

void VectorTileModel::updateTile(const TileId& idWithMapThemeHash, GeoDataDocument* document)
{
    TileId const id(0, idWithMapThemeHash.zoomLevel(), idWithMapThemeHash.x(), idWithMapThemeHash.y());
    m_pendingDocuments.removeAll(id);

    if (!document)
    {
        return;
    }

    if (m_tileLoadLevel != id.zoomLevel())
    {
        delete document;
        return;
    }

    document->setName(QString::fromUtf8("%1/%2/%3").arg(id.zoomLevel()).arg(id.x()).arg(id.y()));
    m_garbageQueue << document;

    if (m_documents.contains(id))
    {
        m_documents.remove(id);
    }

    if (m_deleteDocumentsLater)
    {
        m_deleteDocumentsLater = false;
        m_documents.clear();
    }

    const GeoDataLatLonBox boundingBox = m_layer->tileProjection()->geoCoordinates(id);
    m_documents[id]                    = QSharedPointer<CacheDocument>(new CacheDocument(document, this, boundingBox));

    Q_EMIT tileAdded(document);
}

void VectorTileModel::clear()
{
    m_documents.clear();
}

void VectorTileModel::queryTiles(int tileZoomLevel, const QRect& rect)
{
    // Download all the tiles inside the given indexes

    for (int x = rect.left() ; x <= rect.right() ; ++x)
    {
        for (int y = rect.top() ; y <= rect.bottom() ; ++y)
        {
            const TileId tileId = TileId(0, tileZoomLevel, x, y);

            if (!m_documents.contains(tileId) && !m_pendingDocuments.contains(tileId))
            {
                m_pendingDocuments << tileId;
                TileRunner* job = new TileRunner(m_loader, m_layer, tileId);

                connect(job, SIGNAL(documentLoaded(TileId,GeoDataDocument*)),
                        this, SLOT(updateTile(TileId,GeoDataDocument*)));

                m_threadPool->start(job);
            }
        }
    }
}

void VectorTileModel::cleanupTile(GeoDataObject* object)
{
    if (GeoDataDocument* document = geodata_cast<GeoDataDocument>(object))
    {
        if (m_garbageQueue.contains(document))
        {
            m_garbageQueue.removeAll(document);
            delete document;
        }
    }
}

} // namespace Marble

#include "moc_VectorTileModel.cpp"
