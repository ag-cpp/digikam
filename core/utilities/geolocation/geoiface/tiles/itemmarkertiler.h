/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-17
 * Description : A marker tiler operating on item models
 *
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
    virtual ~ItemMarkerTiler();

    virtual TilerFlags tilerFlags()                                                                         const override;
    virtual Tile* tileNew()                                                                                       override;
    virtual void tileDeleteInternal(Tile* const tile)                                                             override;
    virtual void prepareTiles(const GeoCoordinates& upperLeft, const GeoCoordinates& lowerRight, int level)       override;
    virtual void regenerateTiles()                                                                                override;
    virtual Tile* getTile(const TileIndex& tileIndex, const bool stopIfEmpty = false)                             override;
    virtual int getTileMarkerCount(const TileIndex& tileIndex)                                                    override;
    virtual int getTileSelectedCount(const TileIndex& tileIndex)                                                  override;

    virtual QVariant getTileRepresentativeMarker(const TileIndex& tileIndex, const int sortKey)                   override;
    virtual QVariant bestRepresentativeIndexFromList(const QList<QVariant>& indices, const int sortKey)           override;
    virtual QPixmap pixmapFromRepresentativeIndex(const QVariant& index, const QSize& size)                       override;
    virtual bool indicesEqual(const QVariant& a, const QVariant& b)                                         const override;
    virtual GeoGroupState getTileGroupState(const TileIndex& tileIndex)                                           override;
    virtual GeoGroupState getGlobalGroupState()                                                                   override;

    virtual void onIndicesClicked(const ClickInfo& clickInfo)                                                     override;
    virtual void onIndicesMoved(const TileIndex::List& tileIndicesList,
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
