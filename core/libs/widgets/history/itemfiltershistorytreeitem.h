/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-23
 * Description : widget for displaying an item in view with used filters on current image
 *
 * Copyright (C) 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_ITEM_FILTERS_HISTORY_TREE_ITEM_H
#define DIGIKAM_ITEM_FILTERS_HISTORY_TREE_ITEM_H

// Qt includes

#include <QList>
#include <QVariant>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ItemFiltersHistoryTreeItem
{
public:

    explicit ItemFiltersHistoryTreeItem(const QList<QVariant>& data, ItemFiltersHistoryTreeItem* const parent = nullptr);
    explicit ItemFiltersHistoryTreeItem(const QString& data, ItemFiltersHistoryTreeItem* const parent = nullptr);
    ~ItemFiltersHistoryTreeItem();

    void appendChild(ItemFiltersHistoryTreeItem* const child);
    void removeChild(int row);

    ItemFiltersHistoryTreeItem* child(int row)  const;
    int childCount()                            const;
    int columnCount()                           const;
    QVariant data(int column)                   const;
    int row()                                   const;
    ItemFiltersHistoryTreeItem* parent()        const;
    void setDisabled(bool disabled)             const;
    bool isDisabled()                           const;

private:

    // Disable
    ItemFiltersHistoryTreeItem(const ItemFiltersHistoryTreeItem&);
    ItemFiltersHistoryTreeItem& operator=(const ItemFiltersHistoryTreeItem&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_FILTERS_HISTORY_TREE_ITEM_H
