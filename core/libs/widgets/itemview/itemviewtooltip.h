/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-24
 * Description : A DItemToolTip prepared for use in QAbstractItemViews
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_ITEM_VIEW_TOOL_TIP_H
#define DIGIKAM_ITEM_VIEW_TOOL_TIP_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "ditemtooltip.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ItemViewToolTip : public DItemToolTip
{
    Q_OBJECT

public:

    explicit ItemViewToolTip(QAbstractItemView* const view);
    ~ItemViewToolTip()                      override;

    QAbstractItemView* view()   const;
    QAbstractItemModel* model() const;
    QModelIndex currentIndex()  const;

    /**
     * Show the tooltip for the given item.
     * The rect of the given option is taken as area for which
     * the tooltip is shown.
     */
    void show(const QStyleOptionViewItem& option, const QModelIndex& index);

    void setTipContents(const QString& tipContents);

    /**
     * Default implementation is based on setTipContents().
     * Reimplement if you dynamically provide the contents.
     */
    QString tipContents()                   override;

protected:

    QRect repositionRect()                  override;

    bool eventFilter(QObject* o, QEvent* e) override;
    void hideEvent(QHideEvent*)             override;
    void mouseMoveEvent(QMouseEvent* e)     override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_VIEW_TOOL_TIP_H
