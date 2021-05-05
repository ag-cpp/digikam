/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-01-28
 * Description : drag and drop handling for Showfoto
 *
 * Copyright (C) 2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef SHOW_FOTO_DRAG_DROP_HANDLER_H
#define SHOW_FOTO_DRAG_DROP_HANDLER_H

// KDE includes

#include <QMenu>

// Local includes

#include "abstractitemdragdrophandler.h"
#include "showfotoimagemodel.h"
#include "ddragobjects.h"

namespace ShowFoto
{

class ShowfotoDragDropHandler : public AbstractItemDragDropHandler
{
    Q_OBJECT

public:

    explicit ShowfotoDragDropHandler(ShowfotoItemModel* const model);

    ShowfotoItemModel*    model()   const;

    bool           dropEvent(QAbstractItemView* view, const QDropEvent* e, const QModelIndex& droppedOn) override;
    Qt::DropAction accepts(const QDropEvent* e, const QModelIndex& dropIndex)                            override;
    QStringList    mimeTypes()      const                                                                override;
    QMimeData*     createMimeData(const QList<QModelIndex>&)                                             override;

Q_SIGNALS:

    void signalDroppedUrls(const QList<QUrl>& droppedUrls, bool dropped);

private:

    QAction* addGroupAction(QMenu* const menu);
    QAction* addCancelAction(QMenu* const menu);
};

} // namespace Showfoto

#endif // SHOW_FOTO_DRAG_DROP_HANDLER_H
