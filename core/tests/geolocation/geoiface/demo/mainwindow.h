/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : main-window of the demo application
 *
 * Copyright (C) 2009-2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C)      2014 by Justus Schwartz <justus at gmx dot li>
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

#ifndef DIGIKAM_GEO_MAIN_WINDOW_H
#define DIGIKAM_GEO_MAIN_WINDOW_H

// Qt includes

#include <QItemSelection>
#include <QMainWindow>

// Local includes

#include "geoifacetypes.h"
#include "geomodelhelper.h"
#include "trackmanager.h"

class QCommandLineParser;

using namespace Digikam;

class MarkerModelHelper : public GeoModelHelper
{
    Q_OBJECT

public:

    explicit MarkerModelHelper(QAbstractItemModel* const itemModel,
                               QItemSelectionModel* const itemSelectionModel);
    ~MarkerModelHelper()                                                        override;

    QAbstractItemModel*  model()                                          const override;
    QItemSelectionModel* selectionModel()                                 const override;
    PropertyFlags        modelFlags()                                     const override;
    bool itemCoordinates(const QModelIndex& index,
                         GeoCoordinates* const coordinates)               const override;
    void onIndicesMoved(const QList<QPersistentModelIndex>& movedIndices,
                        const GeoCoordinates& targetCoordinates,
                        const QPersistentModelIndex& targetSnapIndex)           override;

private:

    QAbstractItemModel* const  m_itemModel;
    QItemSelectionModel* const m_itemSelectionModel;

Q_SIGNALS:

    void signalMarkersMoved(const QList<QPersistentModelIndex>& movedIndices);
};

// ------------------------------------------------------------------------------------------------

class MyTrackModelHelper : public QObject
{
    Q_OBJECT

public:

    explicit MyTrackModelHelper(QAbstractItemModel* const imageItemsModel);

    virtual TrackManager::Track::List getTracks() const;

Q_SIGNALS:

    void signalModelChanged();

public Q_SLOTS:

    void slotTrackModelChanged();

private:

    TrackManager::Track::List m_tracks;
    QAbstractItemModel*       m_itemModel;
};

// ------------------------------------------------------------------------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QCommandLineParser* const cmdLineArgs,
                        QWidget* const parent = nullptr);
    ~MainWindow()                       override;

public Q_SLOTS:

    void slotScheduleImagesForLoading(const QList<QUrl>& imagesToSchedule);

protected:

    void readSettings();
    void saveSettings();
    void closeEvent(QCloseEvent* e)     override;
    void createMenus();

private Q_SLOTS:

    void slotFutureResultsReadyAt(int startIndex, int endIndex);
    void slotImageLoadingBunchReady();
    void slotMarkersMoved(const QList<QPersistentModelIndex>& markerIndices);
    void slotAltitudeRequestsReady(const QList<int>& readyRequests);
    void slotAltitudeLookupDone();
    void slotAddImages();

private:

    class Private;
    Private* const d;
};

#endif // DIGIKAM_GEO_MAIN_WINDOW_H
