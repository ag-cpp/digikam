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

#include "TileLayer.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoPainter;
class GeoDataDocument;
class GeoSceneGroup;
class GeoSceneVectorTileDataset;
class GeoDataTreeModel;
class PluginManager;
class HttpDownloadManager;
class ViewportParams;
class TileId;

class DIGIKAM_EXPORT VectorTileLayer : public TileLayer
{
    Q_OBJECT

public:

    VectorTileLayer(HttpDownloadManager* downloadManager,
                    const PluginManager* pluginManager,
                    GeoDataTreeModel* treeModel);

    ~VectorTileLayer() override;

    RenderState renderState() const override;

    int tileZoomLevel() const;

    QString runtimeTrace() const override;

    bool render(GeoPainter* painter, ViewportParams* viewport,
                const QString& renderPos = QLatin1String("NONE"),
                GeoSceneLayer* layer = nullptr) override;

    void reload();

    QSize tileSize() const;
    const GeoSceneAbstractTileProjection* tileProjection() const;

    int tileColumnCount(int level) const;
    int tileRowCount(int level) const;

    int layerCount() const;

    void downloadTile(const TileId& stackedTileId);

Q_SIGNALS:

    void tileLevelChanged(int tileLevel);

public Q_SLOTS:

    void setMapTheme(const QVector<const GeoSceneVectorTileDataset*>& textures, const GeoSceneGroup* textureLayerSettings);

    void reset();

private:

    Q_PRIVATE_SLOT(d, void updateLayerSettings())
    Q_PRIVATE_SLOT(d, void updateTile(const TileId& tileId, GeoDataDocument* document))

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
