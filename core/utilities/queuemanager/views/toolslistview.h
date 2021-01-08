/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-24
 * Description : Available batch tools list.
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

#ifndef DIGIKAM_BQM_TOOLS_LIST_VIEW_H
#define DIGIKAM_BQM_TOOLS_LIST_VIEW_H

// Qt includes

#include <QTreeWidget>
#include <QWidget>
#include <QPixmap>
#include <QMap>
#include <QString>
#include <QList>

// Local includes

#include "batchtool.h"
#include "batchtoolsfactory.h"

namespace Digikam
{

class ToolListViewGroup : public QTreeWidgetItem
{

public:

    ToolListViewGroup(QTreeWidget* const parent,
                      BatchTool::BatchToolGroup group);
    ~ToolListViewGroup()                        override;

    BatchTool::BatchToolGroup toolGroup() const;

private:

    BatchTool::BatchToolGroup m_group;

 private:

    Q_DISABLE_COPY(ToolListViewGroup)
};

// -------------------------------------------------------------------------

class ToolListViewItem : public QTreeWidgetItem
{

public:

    ToolListViewItem(ToolListViewGroup* const parent,
                     BatchTool* const tool);
    ~ToolListViewItem()                         override;

    BatchTool* tool()                     const;

private:

    BatchTool* m_tool;

private:

    Q_DISABLE_COPY(ToolListViewItem)
};

// -------------------------------------------------------------------------

class ToolsListView : public QTreeWidget
{
    Q_OBJECT

public:

    explicit ToolsListView(QWidget* const parent);
    ~ToolsListView()                                                               override;

    BatchToolsList toolsList();

    void addTool(BatchTool* const tool);
    bool removeTool(BatchTool* const tool);

Q_SIGNALS:

    void signalAssignTools(const QMultiMap<int, QString>&);

private Q_SLOTS:

    void slotContextMenu();
    void slotAssignTools();
    void slotToolVisible(bool);

private:

    bool hasTool(BatchTool* const tool);
    ToolListViewItem* findTool(BatchTool* const tool);
    ToolListViewGroup* findToolGroup(BatchTool::BatchToolGroup group);

    void        startDrag(Qt::DropActions supportedActions)                        override;
    QStringList mimeTypes()                                                  const override;
    QMimeData*  mimeData(const QList<QTreeWidgetItem*> items)                const override;

    void mouseDoubleClickEvent(QMouseEvent*)                                       override;
    QMultiMap<int, QString> itemsToMap(const QList<QTreeWidgetItem*>& items) const;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_TOOLS_LIST_VIEW_H
