/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-06
 * Description : thumbnail bar for items - the delegate
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_THUMBNAIL_DELEGATE_H
#define DIGIKAM_ITEM_THUMBNAIL_DELEGATE_H

// Qt includes

#include <QListView>

// Local includes

#include "itemdelegate.h"

namespace Digikam
{

class ItemCategoryDrawer;
class ItemThumbnailDelegatePrivate;

class ItemThumbnailDelegate : public ItemDelegate
{
    Q_OBJECT

public:

    explicit ItemThumbnailDelegate(ItemCategorizedView* const parent);
    ~ItemThumbnailDelegate()                            override;

    void setFlow(QListView::Flow flow);

    /** Returns the minimum or maximum viewport size in the limiting dimension,
     *  width or height, depending on current flow.
     */
    int maximumSize()                             const;
    int minimumSize()                             const;

    void setDefaultViewOptions(const QStyleOptionViewItem& option) override;
    bool acceptsActivation(const QPoint& pos,
                           const QRect& visualRect,
                           const QModelIndex& index,
                           QRect* activationRect) const override;

protected:

    void updateContentWidth()                           override;
    void updateRects()                                  override;

private:

    Q_DECLARE_PRIVATE(ItemThumbnailDelegate)
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_THUMBNAIL_DELEGATE_H
