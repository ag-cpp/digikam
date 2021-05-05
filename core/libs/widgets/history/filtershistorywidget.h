/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-03
 * Description : Widget displaying filters history used on an image
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

#ifndef DIGIKAM_FILTERS_HISTORY_WIDGET_H
#define DIGIKAM_FILTERS_HISTORY_WIDGET_H

// Qt includes

#include <QWidget>
#include <QAction>
#include <QModelIndex>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dmetadata.h"

namespace Digikam
{

class DImageHistory;

class RemoveFilterAction : public QAction
{
    Q_OBJECT

public:

    explicit RemoveFilterAction(const QString& label, const QModelIndex& index, QObject* const parent = nullptr);
    ~RemoveFilterAction() override;

    void setIndex(const QModelIndex& index);

public Q_SLOTS:

    void triggerSlot();

Q_SIGNALS:

    void actionTriggered(QModelIndex index);

private:

    QModelIndex m_index;
};

// -------------------------------------------------------------------------------------

class FiltersHistoryWidget : public QWidget
{
    Q_OBJECT

public:

    explicit FiltersHistoryWidget(QWidget* const parent);
    ~FiltersHistoryWidget() override;

    void setCurrentURL(const QUrl& url = QUrl());

public Q_SLOTS:

    void showCustomContextMenu(const QPoint& position);
    void setHistory(const DImageHistory& history);
    void clearData();
    void setEnabledEntries(int count);
    void disableEntries(int count);
    void enableEntries(int count);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FILTERS_HISTORY_WIDGET_H
