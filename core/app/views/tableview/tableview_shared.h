/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-02-14
 * Description : Table view shared object
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TABLE_VIEW_SHARED_H
#define DIGIKAM_TABLE_VIEW_SHARED_H

class QItemSelectionModel;

namespace Digikam
{

class ItemFilterModel;
class ItemModel;
class TableView;
class TableViewColumnFactory;
class TableViewCurrentToSortedSyncer;
class TableViewItemDelegate;
class TableViewModel;
class TableViewSelectionModelSyncer;
class TableViewTreeView;
class ThumbnailLoadThread;

class TableViewShared
{
public:

    explicit TableViewShared();
    ~TableViewShared();

public:

    ItemModel*                     imageModel;
    ItemFilterModel*               imageFilterModel;
    QItemSelectionModel*           imageFilterSelectionModel;
    ThumbnailLoadThread*           thumbnailLoadThread;

    TableView*                     tableView;
    TableViewTreeView*             treeView;
    TableViewModel*                tableViewModel;
    QItemSelectionModel*           tableViewSelectionModel;

    TableViewSelectionModelSyncer* tableViewSelectionModelSyncer;
    TableViewColumnFactory*        columnFactory;
    TableViewItemDelegate*         itemDelegate;

    bool                           isActive;
};

} // namespace Digikam

#endif // DIGIKAM_TABLE_VIEW_SHARED_H
