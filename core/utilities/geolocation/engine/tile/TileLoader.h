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

// Qt includes

#include <QObject>

// Local includes

#include "PluginManager.h"
#include "MarbleGlobal.h"

class QByteArray;
class QImage;
class QUrl;
class QString;

namespace Marble
{

class TileId;
class HttpDownloadManager;
class GeoDataDocument;
class GeoSceneTileDataset;
class GeoSceneTextureTileDataset;
class GeoSceneVectorTileDataset;

class TileLoader: public QObject
{
    Q_OBJECT

public:

    enum TileStatus
    {
        Missing,
        Expired,
        Available
    };

    explicit TileLoader(HttpDownloadManager* const, const PluginManager*);
    ~TileLoader() override;

    QImage loadTileImage(GeoSceneTextureTileDataset const* textureData, TileId const& tileId, DownloadUsage const);
    GeoDataDocument* loadTileVectorData(GeoSceneVectorTileDataset const* vectorData, TileId const& tileId, DownloadUsage const usage);
    void downloadTile(GeoSceneTileDataset const* tileData, TileId const&, DownloadUsage const);

    static int maximumTileLevel(GeoSceneTileDataset const& tileData);

    /**
     * Returns whether the mandatory most basic tile level is fully available for
     * the given @p layer.
     */
    static bool baseTilesAvailable(GeoSceneTileDataset const& tileData);

    /**
      * Returns the status of the downloaded tile file:
      * - Missing when it has not been downloaded
      * - Expired when it has been downloaded, but is too old (as per .dgml expiration time)
      * - Available when it has been downloaded and is not expired
      */
    static TileStatus tileStatus(GeoSceneTileDataset const* tileData, const TileId& tileId);

private Q_SLOTS:

    void updateTile(QByteArray const& imageData, QString const& tileId);
    void updateTile(QString const& fileName, QString const& idStr);

Q_SIGNALS:

    void downloadTile(QUrl const& sourceUrl, QString const& destinationFileName,
                      QString const& id, DownloadUsage);

    void tileCompleted(TileId const& tileId, QImage const& tileImage);

    void tileCompleted(TileId const& tileId, GeoDataDocument* document);

private:

    static QString tileFileName(GeoSceneTileDataset const* tileData, TileId const&);
    void triggerDownload(GeoSceneTileDataset const* tileData, TileId const&, DownloadUsage const);
    static QImage scaledLowerLevelTile(GeoSceneTextureTileDataset const* textureData, TileId const&);
    GeoDataDocument* openVectorFile(const QString& filename) const;

private:

    // For vectorTile parsing
    PluginManager const* m_pluginManager = nullptr;
};

} // namespace Marble
