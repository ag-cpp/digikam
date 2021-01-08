/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-17
 * Description : A marker tiler operating on item models
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ITEM_MARKER_TILER_H
#define ITEM_MARKER_TILER_H

// Qt includes

#include <QItemSelection>

// Local includes

#include "digikam_export.h"
#include "abstractmarkertiler.h"

namespace Digikam
{
class GeoModelHelper;

class DIGIKAM_EXPORT ItemMarkerTiler : public AbstractMarkerTiler
{
    Q_OBJECT

public:

    explicit ItemMarkerTiler(GeoModelHelper* const modelHelper, QObject* const parent = nullptr);
    ~ItemMarkerTiler()                                                                                    override;

    TilerFlags tilerFlags()                                                                         const override;
    Tile* tileNew()                                                                                       override;
    void tileDeleteInternal(Tile* const tile)                                                             override;
    void prepareTiles(const GeoCoordinates& upperLeft, const GeoCoordinates& lowerRight, int level)       override;
    void regenerateTiles()                                                                                override;
    Tile* getTile(const TileIndex& tileIndex, const bool stopIfEmpty = false)                             override;
    int getTileMarkerCount(const TileIndex& tileIndex)                                                    override;
    int getTileSelectedCount(const TileIndex& tileIndex)                                                  override;

    QVariant getTileRepresentativeMarker(const TileIndex& tileIndex, const int sortKey)                   override;
    QVariant bestRepresentativeIndexFromList(const QList<QVariant>& indices, const int sortKey)           override;
    QPixmap pixmapFromRepresentativeIndex(const QVariant& index, const QSize& size)                       override;
    bool indicesEqual(const QVariant& a, const QVariant& b)                                         const override;
    GeoGroupState getTileGroupState(const TileIndex& tileIndex)                                           override;
    GeoGroupState getGlobalGroupState()                                                                   override;

    void onIndicesClicked(const ClickInfo& clickInfo)                                                     override;
    void onIndicesMoved(const TileIndex::List& tileIndicesList,
                                const GeoCoordinates& targetCoordinates,
                                const QPersistentModelIndex& targetSnapIndex)                                     override;

    void setMarkerGeoModelHelper(GeoModelHelper* const modelHelper);
    void removeMarkerIndexFromGrid(const QModelIndex& markerIndex, const bool ignoreSelection = false);
    void addMarkerIndexToGrid(const QPersistentModelIndex& markerIndex);

    void setActive(const bool state)                                                                              override;

private Q_SLOTS:

    void slotSourceModelRowsInserted(const QModelIndex& parentIndex, int start, int end);
    void slotSourceModelRowsAboutToBeRemoved(const QModelIndex& parentIndex, int start, int end);
    void slotSourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void slotSourceModelReset();
    void slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void slotThumbnailAvailableForIndex(const QPersistentModelIndex& index, const QPixmap& pixmap);
    void slotSourceModelLayoutChanged();

private:

    QList<QPersistentModelIndex> getTileMarkerIndices(const TileIndex& tileIndex);

private:

    class MyTile;

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // ITEM_MARKER_TILER_H
