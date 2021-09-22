/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : List-view for the Showfoto folder view.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewlist.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QFileInfo>
#include <QHeaderView>
#include <QDir>
#include <QTimer>
#include <QMenu>
#include <QModelIndex>
#include <QMimeData>
#include <QScrollBar>
#include <QContextMenuEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "showfotofolderviewbar.h"
#include "showfotofolderviewmodel.h"
#include "showfotofolderviewtooltip.h"
#include "dfileoperations.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewList::Private
{

public:

    explicit Private()
      : view        (nullptr),
        bar         (nullptr),
        showToolTips(false),
        toolTipTimer(nullptr),
        toolTip     (nullptr)
    {
    }

    ShowfotoFolderViewSideBar* view;
    ShowfotoFolderViewBar*     bar;
    bool                       showToolTips;
    QTimer*                    toolTipTimer;
    ShowfotoFolderViewToolTip* toolTip;
    QModelIndex                toolTipIndex;
};

ShowfotoFolderViewList::ShowfotoFolderViewList(ShowfotoFolderViewSideBar* const view,
                                               ShowfotoFolderViewBar* const bar)
    : QTreeView(view),
      d        (new Private)
{
    d->view   = view;
    d->bar    = bar;

    setObjectName(QLatin1String("ShowfotoFolderViewList"));
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setExpandsOnDoubleClick(false);
    setAlternatingRowColors(true);
    setIconSize(QSize(32, 32));
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAllColumnsShowFocus(true);

    resizeColumnToContents(0);

    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    viewport()->setMouseTracking(true);

    d->toolTip       = new ShowfotoFolderViewToolTip(this);
    d->toolTipTimer  = new QTimer(this);

    connect(d->toolTipTimer, SIGNAL(timeout()),
            this, SLOT(slotToolTip()));
}

ShowfotoFolderViewList::~ShowfotoFolderViewList()
{
    delete d->toolTip;
    delete d;
}

void ShowfotoFolderViewList::slotOpenInFileManager()
{
    QModelIndex index  = currentIndex();
    QList<QUrl> urls;

    if (index.isValid())
    {
        ShowfotoFolderViewModel* const model = dynamic_cast<ShowfotoFolderViewModel*>(this->model());

        if (model)
        {
            urls << QUrl::fromLocalFile(model->filePath(index));
        }
    }

    if (urls.empty())
    {
        urls << QUrl::fromLocalFile(d->view->currentFolder());
    }

    DFileOperations::openInFileManager(urls);
}

void ShowfotoFolderViewList::slotIconSizeChanged(int size)
{
    setIconSize(QSize(size, size));
}

void ShowfotoFolderViewList::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu* const fsmenu = new QMenu(this);
    fsmenu->setTitle(i18nc("@title", "Folder-View Options"));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoPrevious")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoNext")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoHome")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("GoUp")));
    fsmenu->addSeparator();
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("ShortView")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("DetailledView")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("ShowBookmarks")));
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("MoreSettings")));
    fsmenu->addSeparator();
    fsmenu->addAction(d->bar->toolBarAction(QLatin1String("LoadContents")));

    foreach (QAction* const act, d->bar->toolBarActions())
    {
        if (act->data() == QLatin1String("DPlugin::Generic::View"))
        {
            fsmenu->addAction(act);
        }
    }

    fsmenu->addSeparator();

    QAction* const addBookmark  = new QAction(QIcon::fromTheme(QLatin1String("list-add")),
                                              i18nc("@action: context menu", "Add Bookmark"), this);
    fsmenu->addAction(addBookmark);

    connect(addBookmark, SIGNAL(triggered()),
            this, SIGNAL(signalAddBookmark()));

    QAction* const openFileMngr = new QAction(QIcon::fromTheme(QLatin1String("folder-open")),
                                              i18nc("@action: context menu", "Open in File Manager"), this);
    fsmenu->addAction(openFileMngr);

    connect(openFileMngr, SIGNAL(triggered()),
            this, SLOT(slotOpenInFileManager()));

    fsmenu->exec(e->globalPos());

    delete fsmenu;

    QTreeView::contextMenuEvent(e);
}

void ShowfotoFolderViewList::mouseDoubleClickEvent(QMouseEvent* e)
{
    d->view->loadContents(currentIndex());

    QTreeView::mouseDoubleClickEvent(e);
}

void ShowfotoFolderViewList::setEnableToolTips(bool val)
{
    d->showToolTips = val;

    if (!val)
    {
        hideToolTip();
    }
}

void ShowfotoFolderViewList::hideToolTip()
{
    d->toolTipIndex = QModelIndex();
    d->toolTipTimer->stop();
    slotToolTip();
}

void ShowfotoFolderViewList::slotToolTip()
{
    d->toolTip->setIndex(d->toolTipIndex);
}

bool ShowfotoFolderViewList::acceptToolTip(const QModelIndex& index) const
{
    ShowfotoFolderViewModel* const model = dynamic_cast<ShowfotoFolderViewModel*>(this->model());

    if (model)
    {
        QFileInfo info(model->filePath(index));

        if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
        {
            return true;
        }
    }

    return false;
}

void ShowfotoFolderViewList::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() == Qt::NoButton)
    {
        QModelIndex index = indexAt(e->pos());

        if (d->showToolTips)
        {
            if (!isActiveWindow())
            {
                hideToolTip();
                return;
            }

            if (index != d->toolTipIndex)
            {
                hideToolTip();

                if (acceptToolTip(index))
                {
                    d->toolTipIndex = index;
                    d->toolTipTimer->setSingleShot(true);
                    d->toolTipTimer->start(500);
                }
            }

            if ((index == d->toolTipIndex) && !acceptToolTip(index))
            {
                hideToolTip();
            }
        }

        return;
    }

    hideToolTip();
    QTreeView::mouseMoveEvent(e);
}

void ShowfotoFolderViewList::wheelEvent(QWheelEvent* e)
{
    hideToolTip();
    QTreeView::wheelEvent(e);
}

void ShowfotoFolderViewList::keyPressEvent(QKeyEvent* e)
{
    hideToolTip();
    QTreeView::keyPressEvent(e);
}

void ShowfotoFolderViewList::focusOutEvent(QFocusEvent* e)
{
    hideToolTip();
    QTreeView::focusOutEvent(e);
}

void ShowfotoFolderViewList::leaveEvent(QEvent* e)
{
    hideToolTip();
    QTreeView::leaveEvent(e);
}

} // namespace ShowFoto
