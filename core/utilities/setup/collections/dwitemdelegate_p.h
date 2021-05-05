/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-15
 * Description : widget item delegate for setup collection view
 *
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2007-2008 by Rafael Fernández López <ereslibre at kde dot org>
 * Copyright (C) 2008      by Kevin Ottens <ervin at kde dot org>
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

#ifndef DIGIKAM_DW_ITEM_DELEGATE_PRIVATE_H
#define DIGIKAM_DW_ITEM_DELEGATE_PRIVATE_H

#include "dwitemdelegate.h"

// Qt includes

#include <QObject>
#include <QItemSelectionModel>

namespace Digikam
{

class DWItemDelegate;
class DWItemDelegatePool;

class Q_DECL_HIDDEN DWItemDelegatePrivate : public QObject
{
    Q_OBJECT

public:

    explicit DWItemDelegatePrivate(DWItemDelegate* const q, QObject* const parent = nullptr);
    ~DWItemDelegatePrivate()                            override;

    void updateRowRange(const QModelIndex& parent, int start, int end, bool isRemoving);
    QStyleOptionViewItem optionView(const QModelIndex& index);

public Q_SLOTS:

    void initializeModel(const QModelIndex& parent = QModelIndex());

    void slotDWRowsInserted(const QModelIndex& parent, int start, int end);
    void slotDWRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
    void slotDWRowsRemoved(const QModelIndex& parent, int start, int end);
    void slotDWDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void slotDWLayoutChanged();
    void slotDWModelReset();
    void slotDWSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

protected:

    bool eventFilter(QObject* watched, QEvent* event)   override;

public:

    QAbstractItemView*   itemView;
    DWItemDelegatePool*  widgetPool;
    QAbstractItemModel*  model;
    QItemSelectionModel* selectionModel;
    bool                 viewDestroyed;
    DWItemDelegate*      q;
};

} // namespace Digikam

#endif // DIGIKAM_DW_ITEM_DELEGATE_PRIVATE_H
