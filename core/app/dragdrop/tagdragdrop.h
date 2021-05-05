/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-21
 * Description : Qt Model for Tags - drag and drop handling
 *
 * Copyright (C) 2009      by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_TAG_DRAG_DROP_H
#define DIGIKAM_TAG_DRAG_DROP_H

// Local includes

#include "albummodeldragdrophandler.h"
#include "albummodel.h"

namespace Digikam
{

class TagDragDropHandler : public AlbumModelDragDropHandler
{
    Q_OBJECT

public:

    explicit TagDragDropHandler(TagModel* const model);

    TagModel* model()       const;

    bool dropEvent(QAbstractItemView* view,
                   const QDropEvent* e,
                   const QModelIndex& droppedOn)         override;

    Qt::DropAction accepts(const QDropEvent* e,
                           const QModelIndex& dropIndex) override;

    QStringList mimeTypes() const                        override;
    QMimeData* createMimeData(const QList<Album*>&)      override;

Q_SIGNALS:

    void assignTags(const QList<qlonglong>& imageIDs, const QList<int>& tagIDs);
};

} // namespace Digikam

#endif // DIGIKAM_TAG_DRAG_DROP_H
