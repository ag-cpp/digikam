/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-21
 * Description : A simple model to hold a tree structure.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2014 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "simpletreemodel.h"

namespace Digikam
{

class Q_DECL_HIDDEN SimpleTreeModel::Private
{
public:

    Private() = default;

    SimpleTreeModel::Item* rootItem     = nullptr;
    int                    columnCount  = 1;
};


SimpleTreeModel::SimpleTreeModel(const int columnCount, QObject* const parent)
    : QAbstractItemModel(parent),
      d                 (new Private)
{
    d->columnCount = columnCount;
    d->rootItem    = new Item();
}

SimpleTreeModel::~SimpleTreeModel()
{
    delete d->rootItem;
    delete d;
}

int SimpleTreeModel::columnCount(const QModelIndex& parent) const
{
    const Item* const item = indexToItem(parent);

    if (!item)
    {
        return 0;
    }

    return d->columnCount;
}

bool SimpleTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Item* const item = indexToItem(index);

    if (!item)
    {
        return false;
    }

    const int column = index.column();

    if (column < 0)
    {
        return false;
    }

    while (item->dataColumns.count()<column)
    {
        item->dataColumns.append(QMap<int, QVariant>());
    }

    item->dataColumns[column].insert(role, value);

    Q_EMIT dataChanged(index, index);

    return true;
}

QVariant SimpleTreeModel::data(const QModelIndex& index, int role) const
{
    const Item* const item = indexToItem(index);

    if (!item)
    {
        return QVariant();
    }

    if (index.row() > 0)
    {
        return QVariant();
    }

    const int column = index.column();

    if ((column < 0) || (column >= item->dataColumns.count()))
    {
        return QVariant();
    }

    return item->dataColumns.at(column).value(role);
}

QModelIndex SimpleTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    Item* const item = indexToItem(parent);

    if (!item)
    {
        return QModelIndex();
    }

    if (parent.isValid() && (parent.column() != 0))
    {
        return QModelIndex();
    }

    if ((row < 0) || (column != 0))
    {
        return QModelIndex();
    }

    if (row >= item->children.count())
    {
        return QModelIndex();
    }

    return createIndex(row, column, item);
}

QModelIndex SimpleTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    Item* const item = indexToItem(index);

    if (!item)
    {
        return QModelIndex();
    }

    if ((item->parent == nullptr) || (item->parent == d->rootItem))
    {
        return QModelIndex();
    }

    return itemToIndex(item->parent);
}

int SimpleTreeModel::rowCount(const QModelIndex& parent) const
{
    const Item* const item = indexToItem(parent);

    if (!item)
    {
        return 0;
    }

    return item->children.count();
}

bool SimpleTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

QVariant SimpleTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

Qt::ItemFlags SimpleTreeModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

SimpleTreeModel::Item* SimpleTreeModel::addItem(SimpleTreeModel::Item* const parentItem, const int rowNumber)
{
    Item* const myParent    = parentItem ? parentItem : d->rootItem;
    Item* const newItem     = new Item();
    newItem->parent         = myParent;

    const int childrenCount = myParent->children.count();
    int targetRow           = rowNumber;

    if ((rowNumber < 0) || (rowNumber > childrenCount))
    {
        targetRow = childrenCount;
    }

    beginInsertRows(itemToIndex(myParent), targetRow, targetRow);
    myParent->children.insert(targetRow, newItem);
    endInsertRows();

    return newItem;
}

SimpleTreeModel::Item* SimpleTreeModel::indexToItem(const QModelIndex& itemIndex) const
{
    if (!itemIndex.isValid())
    {
        return d->rootItem;
    }

    Item* const item = static_cast<Item*>(itemIndex.internalPointer());
    const int row    = itemIndex.row();

    if ((row < 0) || (row >= item->children.count()))
    {
        return nullptr;
    }

    return item->children.at(row);
}

SimpleTreeModel::Item* SimpleTreeModel::rootItem() const
{
    return d->rootItem;
}

QModelIndex SimpleTreeModel::itemToIndex(const Item* const item) const
{
    if ((!item) || (item == d->rootItem))
    {
        return QModelIndex();
    }

    Item* const parentItem = item->parent;
    const int rowNumber    = parentItem->children.indexOf(const_cast<Item*>(item));

    if (rowNumber < 0)
    {
        return QModelIndex();
    }

    return createIndex(rowNumber, 0, parentItem);
}

} // namespace Digikam

#include "moc_simpletreemodel.cpp"
