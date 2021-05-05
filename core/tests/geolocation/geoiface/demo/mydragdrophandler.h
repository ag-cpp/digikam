/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-18
 * Description : Drag-and-drop handler for geolocation interface used in the demo
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_GEO_MY_DRAG_DROP_HANDLER_H
#define DIGIKAM_GEO_MY_DRAG_DROP_HANDLER_H

// Qt includes

#include <QAbstractItemModel>
#include <QMimeData>
#include <QTreeWidgetItem>

// Local includes

#include "geodragdrophandler.h"

using namespace Digikam;

class MyDragData : public QMimeData
{
    Q_OBJECT

public:

    MyDragData()
        : QMimeData(),
          draggedIndices()
    {
    }

    QList<QPersistentModelIndex> draggedIndices;
};

class MyDragDropHandler : public GeoDragDropHandler
{
    Q_OBJECT

public:

    explicit MyDragDropHandler(QAbstractItemModel* const pModel, QObject* const parent = nullptr);
    ~MyDragDropHandler() override;

    Qt::DropAction accepts(const QDropEvent* e) override;
    bool           dropEvent(const QDropEvent* e, const GeoCoordinates& dropCoordinates) override;
    QMimeData*     createMimeData(const QList<QPersistentModelIndex>& modelIndices) override;

private:

    QAbstractItemModel* const model;
};

#endif // DIGIKAM_GEO_MY_DRAG_DROP_HANDLER_H
