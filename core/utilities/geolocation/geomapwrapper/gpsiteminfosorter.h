/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-06
 * Description : Helper class for geomap interaction
 *
 * Copyright (C) 2011      by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_GPS_ITEM_INFO_SORTER_H
#define DIGIKAM_GPS_ITEM_INFO_SORTER_H

// Qt includes

#include <QDateTime>
#include <QObject>
#include <QSize>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "geocoordinates.h"
#include "geogroupstate.h"
#include "gpsiteminfo.h"
#include "mapwidget.h"

namespace Digikam
{

class DIGIKAM_EXPORT GPSItemInfoSorter : public QObject
{
    Q_OBJECT

public:

    enum SortOption
    {
        SortYoungestFirst = 0,
        SortOldestFirst   = 1,
        SortRating        = 2
    };
    Q_DECLARE_FLAGS(SortOptions, SortOption)

public:

    explicit GPSItemInfoSorter(QObject* const parent);
    ~GPSItemInfoSorter() override;

    void addToMapWidget(MapWidget* const mapWidget);

    void setSortOptions(const SortOptions sortOptions);
    SortOptions getSortOptions() const;

public:

    static bool fitsBetter(const GPSItemInfo& oldInfo,
                           const GeoGroupState oldState,
                           const GPSItemInfo& newInfo,
                           const GeoGroupState newState,
                           const GeoGroupState globalGroupState,
                           const SortOptions sortOptions);

private Q_SLOTS:

    void slotSortOptionTriggered();

private:

    void initializeSortMenu();

private:

     // Disable
    GPSItemInfoSorter() = delete;

    class Private;
    Private* const d;
};

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::GPSItemInfoSorter::SortOptions)

#endif // DIGIKAM_GPS_ITEM_INFO_SORTER_H
