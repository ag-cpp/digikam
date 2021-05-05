/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-30
 * Description : GPS search sidebar tab contents.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C)      2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_GPS_SEARCH_VIEW_H
#define DIGIKAM_GPS_SEARCH_VIEW_H

// Qt includes

#include <QWidget>

// Local includes

#include "geomodelhelper.h"
#include "mapwidget.h"
#include "statesavingobject.h"
#include "itemfiltermodel.h"

namespace Digikam
{

class Album;
class SAlbum;
class SearchModel;
class SearchModificationHelper;

class GPSSearchView : public QWidget, public StateSavingObject
{
    Q_OBJECT

public:

    explicit GPSSearchView(QWidget* const parent,
                  SearchModel* const searchModel,
                  SearchModificationHelper* const searchModificationHelper,
                  ItemFilterModel* const imageFilterModel,
                  QItemSelectionModel* const itemSelectionModel);
    ~GPSSearchView()                                override;

    void setActive(bool state);

    void changeAlbumFromHistory(SAlbum* const album);

    void doLoadState()                              override;
    void doSaveState()                              override;

    void setConfigGroup(const KConfigGroup& group)  override;

public Q_SLOTS:

    void slotRefreshMap();
    void slotClearImages();

private:

    bool checkName(QString& name);
    bool checkAlbum(const QString& name) const;

    void createNewGPSSearchAlbum(const QString& name);

private Q_SLOTS:

    void slotAlbumSelected(Album*);

    void slotSaveGPSSAlbum();
    void slotCheckNameEditGPSConditions();

    void slotRegionSelectionChanged();
    void slotRemoveCurrentFilter();
    void slotMapSoloItems(const QList<qlonglong>& idList);

    void showNonGeolocatedItems();

Q_SIGNALS:

    void signalMapSoloItems(const QList<qlonglong>& idList, const QString& id);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_GPS_SEARCH_VIEW_H
