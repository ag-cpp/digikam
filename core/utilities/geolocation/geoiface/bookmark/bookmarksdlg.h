/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-15
 * Description : Managemenet dialogs for GPS bookmarks
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BOOKMARKS_DLG_H
#define DIGIKAM_BOOKMARKS_DLG_H

// Qt includes

#include <QDialog>
#include <QTreeView>
#include <QComboBox>
#include <QAbstractItemModel>

// Local includes

#include "searchtextbar.h"
#include "bookmarksmngr.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT AddBookmarkDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddBookmarkDialog(const QString& url,
                               const QString& title,
                               QWidget* const parent = nullptr,
                               BookmarksManager* const mngr = nullptr);
    ~AddBookmarkDialog() override;

private Q_SLOTS:

    void accept()         override;

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------------------------

class DIGIKAM_EXPORT BookmarksDialog : public QDialog
{
    Q_OBJECT

public:

    explicit BookmarksDialog(QWidget* const parent = nullptr, BookmarksManager* const mngr = nullptr);
    ~BookmarksDialog()              override;

private Q_SLOTS:

    void slotCustomContextMenuRequested(const QPoint&);
    void accept() override;
    void slotOpenInMap(const QModelIndex&);
    void slotNewFolder();
    void slotRemoveOne();

protected:

    void closeEvent(QCloseEvent*)   override;

private:

    void expandNodes(BookmarkNode* const node);
    bool saveExpandedNodes(const QModelIndex& parent);
    void readSettings();
    void saveSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BOOKMARKS_DLG_H
