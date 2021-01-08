/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-27
 * Description : batch tools list assigned to an queued item.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_ASSIGNED_LIST_H
#define DIGIKAM_BQM_ASSIGNED_LIST_H

// Qt includes

#include <QTreeWidget>
#include <QWidget>
#include <QIcon>

// Local includes

#include "batchtool.h"
#include "batchtoolutils.h"

namespace Digikam
{

class QueueSettings;

class AssignedListViewItem : public QTreeWidgetItem
{

public:

    explicit AssignedListViewItem(QTreeWidget* const parent);
    AssignedListViewItem(QTreeWidget* const parent, QTreeWidgetItem* const preceding);
    ~AssignedListViewItem()                                             override;

    void setIndex(int index);

    void setToolSet(const BatchToolSet& set);
    BatchToolSet toolSet();

private:

    BatchToolSet m_set;

private:

    Q_DISABLE_COPY(AssignedListViewItem)
};

// -------------------------------------------------------------------------

class AssignedListView : public QTreeWidget
{
    Q_OBJECT

public:

    explicit AssignedListView(QWidget* const parent);
    ~AssignedListView()                                                 override;

    int                assignedCount();
    AssignedBatchTools assignedList();

    AssignedListViewItem* insertTool(AssignedListViewItem* const preceding, const BatchToolSet& set);
    AssignedListViewItem* moveTool(AssignedListViewItem* const preceding, const BatchToolSet& set);
    AssignedListViewItem* addTool(const BatchToolSet& set);

    bool removeTool(const BatchToolSet& set);

    void setBusy(bool b);

Q_SIGNALS:

    void signalToolSelected(const BatchToolSet&);
    void signalAssignedToolsChanged(const AssignedBatchTools&);

public Q_SLOTS:

    void slotMoveCurrentToolUp();
    void slotMoveCurrentToolDown();
    void slotRemoveCurrentTool();
    void slotClearToolsList();
    void slotQueueSelected(int, const QueueSettings&, const AssignedBatchTools&);
    void slotSettingsChanged(const BatchToolSet&);
    void slotAssignTools(const QMultiMap<int, QString>&);

protected:

    void keyPressEvent(QKeyEvent*)                                      override;

private Q_SLOTS:

    void slotSelectionChanged();
    void slotContextMenu();

private:

    AssignedListViewItem* findTool(const BatchToolSet& set);
    void assignTools(const QMultiMap<int, QString>& map, AssignedListViewItem* const preceding);
    void refreshIndex();

    Qt::DropActions supportedDropActions()                        const override;
    QStringList     mimeTypes()                                   const override;
    QMimeData*      mimeData(const QList<QTreeWidgetItem*> items) const override;

    void dragEnterEvent(QDragEnterEvent*)                               override;
    void dragMoveEvent(QDragMoveEvent*)                                 override;
    void dropEvent(QDropEvent*)                                         override;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_ASSIGNED_LIST_H
