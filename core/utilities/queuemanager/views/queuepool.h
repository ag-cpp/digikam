/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-13
 * Description : tabbed queue items list.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_QUEUE_POOL_H
#define DIGIKAM_BQM_QUEUE_POOL_H

// Qt includes

#include <QMap>
#include <QTabBar>
#include <QWidget>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTabWidget>

// Local includes

#include "iteminfoset.h"

namespace Digikam
{

class DRawDecoding;
class AssignedBatchTools;
class QueueSettings;
class QueueListView;


class QueuePoolBar : public QTabBar
{
    Q_OBJECT

public:

    explicit QueuePoolBar(QWidget* const parent);
    ~QueuePoolBar() override;

Q_SIGNALS:

    void signalTestCanDecode(const QDragMoveEvent*, bool&);

private:

    void dragEnterEvent(QDragEnterEvent* e) override;
    void dragMoveEvent(QDragMoveEvent* e)   override;
};

// --------------------------------------------------------------------------------------

class QueuePool : public QTabWidget
{
    Q_OBJECT

public:

    explicit QueuePool(QWidget* const parent);
    ~QueuePool() override;

    QueueListView*     currentQueue()                    const;
    QString            currentTitle()                    const;
    QueueListView*     findQueueByIndex(int index)       const;
    QueuePoolItemsList queueItemsList(int index)         const;
    int                totalPendingItems()               const;
    int                totalPendingTasks()               const;
    QMap<int, QString> queuesMap()                       const;
    QString            queueTitle(int index)             const;
    bool               customRenamingRulesAreValid()     const;
    bool               assignedBatchToolsListsAreValid() const;
    bool               saveWorkflow()                    const;

    void setBusy(bool b);
    void setItemBusy(qlonglong id);

    /** Apply settings changes to all queues settings container when something have been changed in
     *  digiKam setup dialog.
     */
    void applySettings();

Q_SIGNALS:

    void signalItemSelectionChanged();
    void signalQueuePoolChanged();
    void signalQueueSelected(int id, const QueueSettings&, const AssignedBatchTools&);
    void signalQueueContentsChanged();

public Q_SLOTS:

    void removeTab(int index);

    void slotAddQueue();
    void slotRemoveCurrentQueue();
    void slotClearList();
    void slotRemoveSelectedItems();
    void slotRemoveItemsDone();
    void slotAddItems(const ItemInfoList&, int queueId);
    void slotAssignedToolsChanged(const AssignedBatchTools&);
    void slotSettingsChanged(const QueueSettings&);
    void slotQueueSelected(int);

protected:

    void keyPressEvent(QKeyEvent* event) override;

private :

    QueueListView* findQueueByItemId(qlonglong id) const;

private Q_SLOTS:

    void slotFileChanged(const QString&);
    void slotCloseQueueRequest(int);
    void slotTestCanDecode(const QDragMoveEvent*, bool&);
};

} // namespace Digikam

#endif // DIGIKAM_BQM_QUEUE_POOL_H
