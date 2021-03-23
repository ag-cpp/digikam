/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : Qt model-view for face item - the delegate
 *
 * Copyright (C) 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_ITEM_FACE_DELEGATE_H
#define DIGIKAM_ITEM_FACE_DELEGATE_H

// Local includes

#include "digikamitemdelegate.h"

namespace Digikam
{

class ItemCategoryDrawer;
class FaceTagsIface;
class ItemFaceDelegatePrivate;

class ItemFaceDelegate : public DigikamItemDelegate
{
    Q_OBJECT

public:

    explicit ItemFaceDelegate(ItemCategorizedView* const parent);
    ~ItemFaceDelegate()                                       override;

    QRect faceRect(const QModelIndex& index)            const;
    QRect largerFaceRect(const QModelIndex& index)      const;

    static FaceTagsIface face(const QModelIndex& index);

protected:

    QPixmap thumbnailPixmap(const QModelIndex& index)   const override;
    void updateRects()                                        override;
    void clearModelDataCaches()                               override;

private:

    // Disable
    ItemFaceDelegate(QObject*) = delete;

private:

    Q_DECLARE_PRIVATE(ItemFaceDelegate)
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_FACE_DELEGATE_H
