/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-02
 * Description : delegate for custom painting of used filters view
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

#ifndef DIGIKAM_ITEM_FILTERS_HISTORY_ITEM_DELEGATE_H
#define DIGIKAM_ITEM_FILTERS_HISTORY_ITEM_DELEGATE_H

// Qt includes

#include <QStyledItemDelegate>

namespace Digikam
{

class ItemFiltersHistoryItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit ItemFiltersHistoryItemDelegate(QObject* const parent = nullptr);
    ~ItemFiltersHistoryItemDelegate()                                                                 override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)                const override;
};

} //namespace Digikam

#endif // DIGIKAM_ITEM_FILTERS_HISTORY_ITEM_DELEGATE_H
