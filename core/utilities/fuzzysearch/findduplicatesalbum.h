/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates tree-view search album.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_FIND_DUPLICATES_ALBUM_H
#define DIGIKAM_FIND_DUPLICATES_ALBUM_H

// Qt includes

#include <QWidget>
#include <QPixmap>
#include <QTreeWidget>

// Local includes

#include "thumbnailloadthread.h"

namespace Digikam
{

class SAlbum;

class FindDuplicatesAlbum : public QTreeWidget
{
    Q_OBJECT

public:

    explicit FindDuplicatesAlbum(QWidget* const parent = nullptr);
    ~FindDuplicatesAlbum() override;

    void updateDuplicatesAlbumItems(const QList<SAlbum*>& sAlbumsToRebuild,
                                    const QList<qlonglong>& deletedImages);

    void selectFirstItem();
    QTreeWidgetItem* firstItem();
    void removeDuplicates();
private :

    void drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;

private Q_SLOTS:

    void slotThumbnailLoaded(const LoadingDescription&, const QPixmap&);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FIND_DUPLICATES_ALBUM_H
