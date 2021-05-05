/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-21
 * Description : widget to display a list of items
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2010 by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2009-2010 by Luka Renko <lure at kubuntu dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ditemslist.h"

// Qt includes

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QMimeData>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QUrl>
#include <QTimer>
#include <QFile>
#include <QPointer>
#include <QXmlStreamAttributes>
#include <QStringRef>
#include <QString>
#include <QStandardPaths>
#include <QIcon>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "drawdecoder.h"
#include "imagedialog.h"
#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "dfiledialog.h"
#include "thumbnailloadthread.h"
#include "dworkingpixmap.h"

namespace Digikam
{

const int DEFAULTSIZE = 48;

class Q_DECL_HIDDEN DItemsListViewItem::Private
{
public:

    explicit Private()
      : hasThumb(false),
        rating  (-1),
        view    (nullptr),
        state   (Waiting)
    {
    }

    bool             hasThumb;       ///< True if thumbnails is a real photo thumbs

    int              rating;         ///< Image Rating from host.
    QString          comments;       ///< Image comments from host.
    QStringList      tags;           ///< List of keywords from host.
    QUrl             url;            ///< Image url provided by host.
    QPixmap          thumb;          ///< Image thumbnail.
    DItemsListView*  view;
    State            state;
};

DItemsListViewItem::DItemsListViewItem(DItemsListView* const view, const QUrl& url)
    : QTreeWidgetItem(view),
      d              (new Private)
{
    setUrl(url);
    setRating(-1);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);

    d->view      = view;
    int iconSize = d->view->iconSize().width();
    setThumb(QIcon::fromTheme(QLatin1String("view-preview")).pixmap(iconSize, iconSize, QIcon::Disabled), false);
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "Creating new ImageListViewItem with url " << d->url
                                 << " for list view " << d->view;
*/
}

DItemsListViewItem::~DItemsListViewItem()
{
    delete d;
}

bool DItemsListViewItem::hasValidThumbnail() const
{
    return d->hasThumb;
}

void DItemsListViewItem::updateInformation()
{
    if (d->view->iface())
    {
        DItemInfo info(d->view->iface()->itemInfo(d->url));

        setComments(info.comment());
        setTags(info.keywords());
        setRating(info.rating());
    }
}

void DItemsListViewItem::setUrl(const QUrl& url)
{
    d->url = url;
    setText(DItemsListView::Filename, d->url.fileName());
}

QUrl DItemsListViewItem::url() const
{
    return d->url;
}

void DItemsListViewItem::setComments(const QString& comments)
{
    d->comments = comments;
}

QString DItemsListViewItem::comments() const
{
    return d->comments;
}

void DItemsListViewItem::setTags(const QStringList& tags)
{
    d->tags = tags;
}

QStringList DItemsListViewItem::tags() const
{
    return d->tags;
}

void DItemsListViewItem::setRating(int rating)
{
    d->rating = rating;
}

int DItemsListViewItem::rating() const
{
    return d->rating;
}

void DItemsListViewItem::setPixmap(const QPixmap& pix)
{
    QIcon icon = QIcon(pix);

    // We make sure the preview icon stays the same regardless of the role.

    icon.addPixmap(pix, QIcon::Selected, QIcon::On);
    icon.addPixmap(pix, QIcon::Selected, QIcon::Off);
    icon.addPixmap(pix, QIcon::Active,   QIcon::On);
    icon.addPixmap(pix, QIcon::Active,   QIcon::Off);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::On);
    icon.addPixmap(pix, QIcon::Normal,   QIcon::Off);
    setIcon(DItemsListView::Thumbnail, icon);
}

void DItemsListViewItem::setThumb(const QPixmap& pix, bool hasThumb)
{
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "Received new thumbnail for url " << d->url
                                 << ". My view is " << d->view;
*/
    if (!d->view)
    {
        qCCritical(DIGIKAM_GENERAL_LOG) << "This item do not have a tree view. "
                                        << "This should never happen!";
        return;
    }

    int iconSize = qMax<int>(d->view->iconSize().width(), d->view->iconSize().height());
    QPixmap pixmap(iconSize + 2, iconSize + 2);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.drawPixmap((pixmap.width()  / 2) - (pix.width()  / 2),
                 (pixmap.height() / 2) - (pix.height() / 2), pix);
    d->thumb     = pixmap;
    setPixmap(d->thumb);

    d->hasThumb  = hasThumb;
}

void DItemsListViewItem::setProgressAnimation(const QPixmap& pix)
{
    QPixmap overlay = d->thumb;
    QPixmap mask(overlay.size());
    mask.fill(QColor(128, 128, 128, 192));
    QPainter p(&overlay);
    p.drawPixmap(0, 0, mask);
    p.drawPixmap((overlay.width()  / 2) - (pix.width()  / 2),
                 (overlay.height() / 2) - (pix.height() / 2), pix);
    setPixmap(overlay);
}

void DItemsListViewItem::setProcessedIcon(const QIcon& icon)
{
    setIcon(DItemsListView::Filename, icon);

    // reset thumbnail back to no animation pix.

    setPixmap(d->thumb);
}

void DItemsListViewItem::setState(State state)
{
    d->state = state;
}

DItemsListViewItem::State DItemsListViewItem::state() const
{
    return d->state;
}

DItemsListView* DItemsListViewItem::view() const
{
    return d->view;
}

// ---------------------------------------------------------------------------

DItemsListView::DItemsListView(DItemsList* const parent)
    : QTreeWidget (parent),
      m_itemDraged(nullptr)
{
    setup(DEFAULTSIZE);
}

DItemsListView::DItemsListView(int iconSize, DItemsList* const parent)
    : QTreeWidget (parent),
      m_itemDraged(nullptr)
{
    setup(iconSize);
}

DItemsListView::~DItemsListView()
{
}

DInfoInterface* DItemsListView::iface() const
{
    DItemsList* const p = dynamic_cast<DItemsList*>(parent());

    if (p)
    {
        return p->iface();
    }

    return nullptr;
}

void DItemsListView::setup(int iconSize)
{
    m_iconSize = iconSize;
    setIconSize(QSize(m_iconSize, m_iconSize));
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    enableDragAndDrop(true);

    setSortingEnabled(false);
    setAllColumnsShowFocus(true);
    setRootIsDecorated(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setColumnCount(8);
    setHeaderLabels(QStringList() << i18nc("@title", "Thumbnail")
                                  << i18nc("@title", "File Name")
                                  << i18nc("@title", "User1")
                                  << i18nc("@title", "User2")
                                  << i18nc("@title", "User3")
                                  << i18nc("@title", "User4")
                                  << i18nc("@title", "User5")
                                  << i18nc("@title", "User6"));
    hideColumn(User1);
    hideColumn(User2);
    hideColumn(User3);
    hideColumn(User4);
    hideColumn(User5);
    hideColumn(User6);

    header()->setSectionResizeMode(User1, QHeaderView::Interactive);
    header()->setSectionResizeMode(User2, QHeaderView::Interactive);
    header()->setSectionResizeMode(User3, QHeaderView::Interactive);
    header()->setSectionResizeMode(User4, QHeaderView::Interactive);
    header()->setSectionResizeMode(User5, QHeaderView::Interactive);
    header()->setSectionResizeMode(User6, QHeaderView::Stretch);

    connect(this, &DItemsListView::itemClicked,
            this, &DItemsListView::slotItemClicked);
}

void DItemsListView::enableDragAndDrop(const bool enable)
{
    setDragEnabled(enable);
    viewport()->setAcceptDrops(enable);
    setDragDropMode(enable ? QAbstractItemView::InternalMove : QAbstractItemView::NoDragDrop);
    setDragDropOverwriteMode(enable);
    setDropIndicatorShown(enable);
}

void DItemsListView::drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(itemFromIndex(index));

    if (item && !item->hasValidThumbnail())
    {
        DItemsList* const view = dynamic_cast<DItemsList*>(parent());

        if (view)
        {
            view->updateThumbnail(item->url());
        }
    }

    QTreeWidget::drawRow(p, opt, index);
}

void DItemsListView::slotItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (!item)
    {
        return;
    }

    emit signalItemClicked(item);
}

void DItemsListView::setColumnLabel(ColumnType column, const QString& label)
{
    headerItem()->setText(column, label);
}

void DItemsListView::setColumnEnabled(ColumnType column, bool enable)
{
    if (enable)
    {
        showColumn(column);
    }
    else
    {
        hideColumn(column);
    }
}

void DItemsListView::setColumn(ColumnType column, const QString& label, bool enable)
{
    setColumnLabel(column, label);
    setColumnEnabled(column, enable);
}

DItemsListViewItem* DItemsListView::findItem(const QUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        DItemsListViewItem* const lvItem = dynamic_cast<DItemsListViewItem*>(*it);

        if (lvItem && (lvItem->url() == url))
        {
            return lvItem;
        }

        ++it;
    }

    return nullptr;
}

QModelIndex DItemsListView::indexFromItem(DItemsListViewItem* item, int column) const
{
    return QTreeWidget::indexFromItem(item, column);
}

void DItemsListView::contextMenuEvent(QContextMenuEvent* e)
{
    QTreeWidget::contextMenuEvent(e);

    emit signalContextMenuRequested();
}

void DItemsListView::dragEnterEvent(QDragEnterEvent* e)
{
    m_itemDraged = QTreeWidget::currentItem();

    QTreeWidget::dragEnterEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void DItemsListView::dragMoveEvent(QDragMoveEvent* e)
{
    QTreeWidget::dragMoveEvent(e);

    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void DItemsListView::dropEvent(QDropEvent* e)
{
    QTreeWidget::dropEvent(e);
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach (const QUrl& url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
        {
            urls.append(url);
        }
    }

    if (!urls.isEmpty())
    {
        emit signalAddedDropedItems(urls);
    }

    scrollToItem(m_itemDraged);
}

// ---------------------------------------------------------------------------

CtrlButton::CtrlButton(const QIcon& icon, QWidget* const parent)
    : QPushButton(parent)
{
    const int btnSize = 32;

    setMinimumSize(btnSize, btnSize);
    setMaximumSize(btnSize, btnSize);
    setIcon(icon);
}

CtrlButton::~CtrlButton()
{
}

// ---------------------------------------------------------------------------

class Q_DECL_HIDDEN DItemsList::Private
{
public:

    explicit Private()
      : allowRAW             (true),
        allowDuplicate       (false),
        controlButtonsEnabled(true),
        iconSize             (DEFAULTSIZE),
        addButton            (nullptr),
        removeButton         (nullptr),
        moveUpButton         (nullptr),
        moveDownButton       (nullptr),
        clearButton          (nullptr),
        loadButton           (nullptr),
        saveButton           (nullptr),
        progressPix          (nullptr),
        progressCount        (0),
        progressTimer        (nullptr),
        listView             (nullptr),
        iface                (nullptr)
    {
        thumbLoadThread = ThumbnailLoadThread::defaultThread();
    }

    bool                       allowRAW;
    bool                       allowDuplicate;
    bool                       controlButtonsEnabled;
    int                        iconSize;

    CtrlButton*                addButton;
    CtrlButton*                removeButton;
    CtrlButton*                moveUpButton;
    CtrlButton*                moveDownButton;
    CtrlButton*                clearButton;
    CtrlButton*                loadButton;
    CtrlButton*                saveButton;

    QList<QUrl>                processItems;
    DWorkingPixmap*            progressPix;
    int                        progressCount;
    QTimer*                    progressTimer;

    DItemsListView*            listView;
    ThumbnailLoadThread*       thumbLoadThread;

    DInfoInterface*            iface;
};

DItemsList::DItemsList(QWidget* const parent, int iconSize)
    : QWidget(parent),
      d      (new Private)
{
    if (iconSize != -1)  // default = ICONSIZE
    {
        setIconSize(iconSize);
    }

    // --------------------------------------------------------

    d->progressPix    = new DWorkingPixmap(this);
    d->listView       = new DItemsListView(d->iconSize, this);
    d->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // --------------------------------------------------------

    d->addButton      = new CtrlButton(QIcon::fromTheme(QLatin1String("list-add")).pixmap(16, 16),      this);
    d->removeButton   = new CtrlButton(QIcon::fromTheme(QLatin1String("list-remove")).pixmap(16, 16),   this);
    d->moveUpButton   = new CtrlButton(QIcon::fromTheme(QLatin1String("go-up")).pixmap(16, 16),         this);
    d->moveDownButton = new CtrlButton(QIcon::fromTheme(QLatin1String("go-down")).pixmap(16, 16),       this);
    d->clearButton    = new CtrlButton(QIcon::fromTheme(QLatin1String("edit-clear")).pixmap(16, 16),    this);
    d->loadButton     = new CtrlButton(QIcon::fromTheme(QLatin1String("document-open")).pixmap(16, 16), this);
    d->saveButton     = new CtrlButton(QIcon::fromTheme(QLatin1String("document-save")).pixmap(16, 16), this);

    d->addButton->setToolTip(i18nc("@info", "Add new images to the list"));
    d->removeButton->setToolTip(i18nc("@info", "Remove selected images from the list"));
    d->moveUpButton->setToolTip(i18nc("@info", "Move current selected image up in the list"));
    d->moveDownButton->setToolTip(i18nc("@info", "Move current selected image down in the list"));
    d->clearButton->setToolTip(i18nc("@info", "Clear the list."));
    d->loadButton->setToolTip(i18nc("@info", "Load a saved list."));
    d->saveButton->setToolTip(i18nc("@info", "Save the list."));

    d->progressTimer  = new QTimer(this);

    // --------------------------------------------------------

    setControlButtons(Add | Remove | MoveUp | MoveDown | Clear | Save | Load ); // add all buttons      (default)
    setControlButtonsPlacement(ControlButtonsBelow);                            // buttons on the bottom (default)
    enableDragAndDrop(true);                                                    // enable drag and drop (default)

    // --------------------------------------------------------

    connect(d->listView, &DItemsListView::signalAddedDropedItems,
            this, &DItemsList::slotAddImages);

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription,QPixmap)),
            this, SLOT(slotThumbnail(LoadingDescription,QPixmap)));

    connect(d->listView, &DItemsListView::signalItemClicked,
            this, &DItemsList::signalItemClicked);

    connect(d->listView, &DItemsListView::signalContextMenuRequested,
            this, &DItemsList::signalContextMenuRequested);

    // queue this connection because itemSelectionChanged is emitted
    // while items are deleted, and accessing selectedItems at that
    // time causes a crash ...

    connect(d->listView, &DItemsListView::itemSelectionChanged,
            this, &DItemsList::slotImageListChanged, Qt::QueuedConnection);

    connect(this, &DItemsList::signalImageListChanged,
            this, &DItemsList::slotImageListChanged);

    // --------------------------------------------------------

    connect(d->addButton, &CtrlButton::clicked,
            this, &DItemsList::slotAddItems);

    connect(d->removeButton, &CtrlButton::clicked,
            this, &DItemsList::slotRemoveItems);

    connect(d->moveUpButton, &CtrlButton::clicked,
            this, &DItemsList::slotMoveUpItems);

    connect(d->moveDownButton, &CtrlButton::clicked,
            this, &DItemsList::slotMoveDownItems);

    connect(d->clearButton, &CtrlButton::clicked,
            this, &DItemsList::slotClearItems);

    connect(d->loadButton, &CtrlButton::clicked,
            this, &DItemsList::slotLoadItems);

    connect(d->saveButton, &CtrlButton::clicked,
            this, &DItemsList::slotSaveItems);

    connect(d->progressTimer, &QTimer::timeout,
            this, &DItemsList::slotProgressTimerDone);

    // --------------------------------------------------------

    QTimer::singleShot(1000, this, SIGNAL(signalImageListChanged()));
}

DItemsList::~DItemsList()
{
    delete d;
}

void DItemsList::enableControlButtons(bool enable)
{
    d->controlButtonsEnabled = enable;
    slotImageListChanged();
}

void DItemsList::enableDragAndDrop(const bool enable)
{
    d->listView->enableDragAndDrop(enable);
}

void DItemsList::setControlButtonsPlacement(ControlButtonPlacement placement)
{
    delete layout();

    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addWidget(d->listView, 1, 1, 1, 1);
    mainLayout->setRowStretch(1, 10);
    mainLayout->setColumnStretch(1, 10);
    mainLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    mainLayout->setSpacing(spacing);

    // --------------------------------------------------------

    QHBoxLayout* const hBtnLayout = new QHBoxLayout;
    hBtnLayout->addStretch(10);
    hBtnLayout->addWidget(d->moveUpButton);
    hBtnLayout->addWidget(d->moveDownButton);
    hBtnLayout->addWidget(d->addButton);
    hBtnLayout->addWidget(d->removeButton);
    hBtnLayout->addWidget(d->loadButton);
    hBtnLayout->addWidget(d->saveButton);
    hBtnLayout->addWidget(d->clearButton);
    hBtnLayout->addStretch(10);

    // --------------------------------------------------------

    QVBoxLayout* const vBtnLayout = new QVBoxLayout;
    vBtnLayout->addStretch(10);
    vBtnLayout->addWidget(d->moveUpButton);
    vBtnLayout->addWidget(d->moveDownButton);
    vBtnLayout->addWidget(d->addButton);
    vBtnLayout->addWidget(d->removeButton);
    vBtnLayout->addWidget(d->loadButton);
    vBtnLayout->addWidget(d->saveButton);
    vBtnLayout->addWidget(d->clearButton);
    vBtnLayout->addStretch(10);

    // --------------------------------------------------------

    switch (placement)
    {
        case ControlButtonsAbove:
        {
            mainLayout->addLayout(hBtnLayout, 0, 1, 1, 1);
            delete vBtnLayout;
            break;
        }

        case ControlButtonsBelow:
        {
            mainLayout->addLayout(hBtnLayout, 2, 1, 1, 1);
            delete vBtnLayout;
            break;
        }

        case ControlButtonsLeft:
        {
            mainLayout->addLayout(vBtnLayout, 1, 0, 1, 1);
            delete hBtnLayout;
            break;
        }

        case ControlButtonsRight:
        {
            mainLayout->addLayout(vBtnLayout, 1, 2, 1, 1);
            delete hBtnLayout;
            break;
        }

        case NoControlButtons:
        default:
        {
            delete vBtnLayout;
            delete hBtnLayout;

            // set all buttons invisible

            setControlButtons(ControlButtons());
            break;
        }
    }

    setLayout(mainLayout);
}

void DItemsList::setControlButtons(ControlButtons buttonMask)
{
    d->addButton->setVisible(buttonMask & Add);
    d->removeButton->setVisible(buttonMask & Remove);
    d->moveUpButton->setVisible(buttonMask & MoveUp);
    d->moveDownButton->setVisible(buttonMask & MoveDown);
    d->clearButton->setVisible(buttonMask & Clear);
    d->loadButton->setVisible(buttonMask & Load);
    d->saveButton->setVisible(buttonMask & Save);
}

void DItemsList::setIface(DInfoInterface* const iface)
{
    d->iface = iface;
}

DInfoInterface* DItemsList::iface() const
{
    return d->iface;
}

void DItemsList::setAllowDuplicate(bool allow)
{
  d->allowDuplicate = allow;
}

void DItemsList::setAllowRAW(bool allow)
{
    d->allowRAW = allow;
}

void DItemsList::setIconSize(int size)
{
    if      (size < 16)
    {
        d->iconSize = 16;
    }
    else if (size > 128)
    {
        d->iconSize = 128;
    }
    else
    {
        d->iconSize = size;
    }
}

int DItemsList::iconSize() const
{
    return d->iconSize;
}

void DItemsList::loadImagesFromCurrentSelection()
{
    bool selection = checkSelection();

    if (selection == true)
    {
        if (!d->iface)
        {
            return;
        }

        QList<QUrl> images = d->iface->currentSelectedItems();

        if (!images.isEmpty())
        {
            slotAddImages(images);
        }
    }
    else
    {
        loadImagesFromCurrentAlbum();
    }
}

void DItemsList::loadImagesFromCurrentAlbum()
{
    if (!d->iface)
    {
        return;
    }

    QList<QUrl> images = d->iface->currentAlbumItems();

    if (!images.isEmpty())
    {
        slotAddImages(images);
    }
}

bool DItemsList::checkSelection()
{
    if (!d->iface)
    {
        return false;
    }

    QList<QUrl> images = d->iface->currentSelectedItems();

    return (!images.isEmpty());
}

void DItemsList::slotAddImages(const QList<QUrl>& list)
{
    if (list.count() == 0)
    {
        return;
    }

    QList<QUrl> urls;
    bool raw = false;

    for (QList<QUrl>::ConstIterator it = list.constBegin() ; it != list.constEnd() ; ++it)
    {
        QUrl imageUrl = *it;

        // Check if the new item already exist in the list.

        bool found    = false;

        QTreeWidgetItemIterator iter(d->listView);

        while (*iter)
        {
            DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(*iter);

            if (item && (item->url() == imageUrl))
            {
                found = true;
            }

            ++iter;
        }

        if (d->allowDuplicate || !found)
        {
            // if RAW files are not allowed, skip the image

            if (!d->allowRAW && DRawDecoder::isRawFile(imageUrl))
            {
                raw = true;
                continue;
            }

            new DItemsListViewItem(listView(), imageUrl);
            urls.append(imageUrl);
        }
    }

    emit signalAddItems(urls);
    emit signalImageListChanged();
    emit signalFoundRAWImages(raw);
}

void DItemsList::slotAddItems()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp        = config->group(objectName());
    QUrl lastFileUrl        = QUrl::fromLocalFile(grp.readEntry("Last Image Path",
                                                  QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)));

    ImageDialog dlg(this, lastFileUrl, false);
    QList<QUrl> urls        = dlg.urls();

    if (!urls.isEmpty())
    {
        slotAddImages(urls);
        grp.writeEntry("Last Image Path", urls.first().adjusted(QUrl::RemoveFilename).toLocalFile());
        config->sync();
    }
}

void DItemsList::slotRemoveItems()
{
    QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    QList<int> itemsIndex;

    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItemsList.constBegin() ;
         it != selectedItemsList.constEnd() ; ++it)
    {
        DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(*it);

        if (item)
        {
            itemsIndex.append(d->listView->indexFromItem(item).row());

            if (d->processItems.contains(item->url()))
            {
                d->processItems.removeAll(item->url());
            }

            d->listView->removeItemWidget(*it, 0);
            delete *it;
        }
    }

    emit signalRemovedItems(itemsIndex);
    emit signalImageListChanged();
}

void DItemsList::slotMoveUpItems()
{
    // move above item down, then we don't have to fix the focus

    QModelIndex curIndex   = listView()->currentIndex();

    if (!curIndex.isValid())
    {
        return;
    }

    QModelIndex aboveIndex = listView()->indexAbove(curIndex);

    if (!aboveIndex.isValid())
    {
        return;
    }

    QTreeWidgetItem* const temp  = listView()->takeTopLevelItem(aboveIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);

    // this is a quick fix. We loose the extra tags in flickr upload, but at list we don't get a crash

    DItemsListViewItem* const uw = dynamic_cast<DItemsListViewItem*>(temp);

    if (uw)
    {
        uw->updateItemWidgets();
    }

    emit signalImageListChanged();
    emit signalMoveUpItem();
}

void DItemsList::slotMoveDownItems()
{
    // move below item up, then we don't have to fix the focus

    QModelIndex curIndex   = listView()->currentIndex();

    if (!curIndex.isValid())
    {
        return;
    }

    QModelIndex belowIndex = listView()->indexBelow(curIndex);

    if (!belowIndex.isValid())
    {
        return;
    }

    QTreeWidgetItem* const temp  = listView()->takeTopLevelItem(belowIndex.row());
    listView()->insertTopLevelItem(curIndex.row(), temp);

    // This is a quick fix. We can loose extra tags in uploader, but at least we don't get a crash

    DItemsListViewItem* const uw = dynamic_cast<DItemsListViewItem*>(temp);

    if (uw)
    {
        uw->updateItemWidgets();
    }

    emit signalImageListChanged();
    emit signalMoveDownItem();
}

void DItemsList::slotClearItems()
{
    listView()->selectAll();
    slotRemoveItems();
    listView()->clear();
}

void DItemsList::slotLoadItems()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp        = config->group(objectName());
    QUrl lastFileUrl        = QUrl::fromLocalFile(grp.readEntry("Last Images List Path",
                                                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)));
    QUrl loadLevelsFile;
    loadLevelsFile          = DFileDialog::getOpenFileUrl(this,
                                                          i18nc("@info", "Select the image file list to load"), lastFileUrl,
                                                          i18nc("@option", "All Files (*)"));

    if (loadLevelsFile.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "empty url";
        return;
    }

    QFile file(loadLevelsFile.toLocalFile());

    qCDebug(DIGIKAM_GENERAL_LOG) << "file path " << loadLevelsFile.toLocalFile();

    if (!file.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open file";
        return;
    }

    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    while (!xmlReader.atEnd())
    {
        if      (xmlReader.isStartElement() && (xmlReader.name() == QLatin1String("Image")))
        {
            // get all attributes and its value of a tag in attrs variable.

            QXmlStreamAttributes attrs = xmlReader.attributes();

            // get value of each attribute from QXmlStreamAttributes

            QStringRef url             = attrs.value(QLatin1String("url"));

            if (url.isEmpty())
            {
                xmlReader.readNext();
                continue;
            }

            QList<QUrl> urls;
            urls.append(QUrl(url.toString()));

            if (!urls.isEmpty())
            {
                //allow tools to append a new file

                slotAddImages(urls);

                // read tool Image custom attributes and children element

                emit signalXMLLoadImageElement(xmlReader);
            }
        }
        else if (xmlReader.isStartElement() && (xmlReader.name() != QLatin1String("Images")))
        {
            // unmanaged start element (it should be tools one)

            emit signalXMLCustomElements(xmlReader);
        }
        else if (xmlReader.isEndElement() && (xmlReader.name() == QLatin1String("Images")))
        {
            // if EndElement is Images return

            grp.writeEntry("Last Images List Path", loadLevelsFile.adjusted(QUrl::RemoveFilename).toLocalFile());
            config->sync();
            file.close();
            return;
        }

        xmlReader.readNext();
    }

    file.close();
}

void DItemsList::slotSaveItems()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp        = config->group(objectName());
    QUrl lastFileUrl        = QUrl::fromLocalFile(grp.readEntry("Last Images List Path",
                                                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)));
    QUrl saveLevelsFile;
    saveLevelsFile          = DFileDialog::getSaveFileUrl(this,
                                                          i18nc("@info", "Select the image file list to save"),
                                                          lastFileUrl,
                                                          i18nc("@option", "All Files (*)"));

    qCDebug(DIGIKAM_GENERAL_LOG) << "file url " << saveLevelsFile.toDisplayString();

    if (saveLevelsFile.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "empty url";
        return;
    }

    QFile file(saveLevelsFile.toLocalFile());

    if (!file.open(QIODevice::WriteOnly))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open target file";
        return;
    }

    QXmlStreamWriter xmlWriter;
    xmlWriter.setDevice(&file);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement(QLatin1String("Images"));

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        DItemsListViewItem* const lvItem = dynamic_cast<DItemsListViewItem*>(*it);

        if (lvItem)
        {
            xmlWriter.writeStartElement(QLatin1String("Image"));

            xmlWriter.writeAttribute(QLatin1String("url"), lvItem->url().toDisplayString());

            emit signalXMLSaveItem(xmlWriter, listView()->indexFromItem(lvItem).row());

            xmlWriter.writeEndElement(); // Image
        }

        ++it;
    }

    emit signalXMLCustomElements(xmlWriter);

    xmlWriter.writeEndElement();  // Images

    xmlWriter.writeEndDocument(); // end document

    grp.writeEntry("Last Images List Path", saveLevelsFile.adjusted(QUrl::RemoveFilename).toLocalFile());
    config->sync();
    file.close();
}

void DItemsList::removeItemByUrl(const QUrl& url)
{
    bool found;
    QList<int> itemsIndex;

    do
    {
        found = false;
        QTreeWidgetItemIterator it(d->listView);

        while (*it)
        {
            DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(*it);

            if (item && (item->url() == url))
            {
                itemsIndex.append(d->listView->indexFromItem(item).row());

                if (d->processItems.contains(item->url()))
                {
                    d->processItems.removeAll(item->url());
                }

                delete item;
                found = true;
                break;
            }

            ++it;
        }
    }
    while (found);

    emit signalRemovedItems(itemsIndex);
    emit signalImageListChanged();
}

QList<QUrl> DItemsList::imageUrls(bool onlyUnprocessed) const
{
    QList<QUrl> list;
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(*it);

        if (item)
        {
            if ((onlyUnprocessed == false) || (item->state() != DItemsListViewItem::Success))
            {
                list.append(item->url());
            }
        }

        ++it;
    }

    return list;
}

void DItemsList::slotProgressTimerDone()
{
    if (!d->processItems.isEmpty())
    {
        foreach (const QUrl& url, d->processItems)
        {
            DItemsListViewItem* const item = listView()->findItem(url);

            if (item)
            {
                item->setProgressAnimation(d->progressPix->frameAt(d->progressCount));
            }
        }

        d->progressCount++;

        if (d->progressCount == 8)
        {
            d->progressCount = 0;
        }

        d->progressTimer->start(300);
    }
}

void DItemsList::processing(const QUrl& url)
{
    DItemsListViewItem* const item = listView()->findItem(url);

    if (item)
    {
        d->processItems.append(url);
        d->listView->setCurrentItem(item, true);
        d->listView->scrollToItem(item);
        d->progressTimer->start(300);
    }
}

void DItemsList::processed(const QUrl& url, bool success)
{
    DItemsListViewItem* const item = listView()->findItem(url);

    if (item)
    {
        d->processItems.removeAll(url);
        item->setProcessedIcon(QIcon::fromTheme(success ? QLatin1String("dialog-ok-apply")
                                                        : QLatin1String("dialog-cancel")).pixmap(16, 16));
        item->setState(success ? DItemsListViewItem::Success
                               : DItemsListViewItem::Failed);

        if (d->processItems.isEmpty())
            d->progressTimer->stop();
    }
}

void DItemsList::cancelProcess()
{
    foreach (const QUrl& url, d->processItems)
    {
        processed(url, false);
    }
}

void DItemsList::clearProcessedStatus()
{
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        DItemsListViewItem* const lvItem = dynamic_cast<DItemsListViewItem*>(*it);

        if (lvItem)
        {
            lvItem->setProcessedIcon(QIcon());
        }

        ++it;
    }
}

DItemsListView* DItemsList::listView() const
{
    return d->listView;
}

void DItemsList::slotImageListChanged()
{
    const QList<QTreeWidgetItem*> selectedItemsList = d->listView->selectedItems();
    const bool haveImages                           = !(imageUrls().isEmpty())         && d->controlButtonsEnabled;
    const bool haveSelectedImages                   = !(selectedItemsList.isEmpty())   && d->controlButtonsEnabled;
    const bool haveOnlyOneSelectedImage             = (selectedItemsList.count() == 1) && d->controlButtonsEnabled;

    d->removeButton->setEnabled(haveSelectedImages);
    d->moveUpButton->setEnabled(haveOnlyOneSelectedImage);
    d->moveDownButton->setEnabled(haveOnlyOneSelectedImage);
    d->clearButton->setEnabled(haveImages);

    // All buttons are enabled / disabled now, but the "Add" button should always be
    // enabled, if the buttons are not explicitly disabled with enableControlButtons()

    d->addButton->setEnabled(d->controlButtonsEnabled);

    // TODO: should they be enabled by default now?

    d->loadButton->setEnabled(d->controlButtonsEnabled);
    d->saveButton->setEnabled(d->controlButtonsEnabled);
}

void DItemsList::updateThumbnail(const QUrl& url)
{
    d->thumbLoadThread->find(ThumbnailIdentifier(url.toLocalFile()));
}

void DItemsList::slotThumbnail(const LoadingDescription& desc, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(d->listView);

    while (*it)
    {
        DItemsListViewItem* const item = dynamic_cast<DItemsListViewItem*>(*it);

        if (item && (item->url() == QUrl::fromLocalFile(desc.filePath)))
        {
            if (!pix.isNull())
            {
                item->setThumb(pix.scaled(d->iconSize, d->iconSize, Qt::KeepAspectRatio));
            }

            if (!d->allowDuplicate)
            {
                return;
            }
        }

        ++it;
    }
}

DItemsListViewItem* DItemsListView::getCurrentItem() const
{
    QTreeWidgetItem* const currentTreeItem = currentItem();

    if (!currentTreeItem)
    {
        return nullptr;
    }

    return dynamic_cast<DItemsListViewItem*>(currentTreeItem);
}

QUrl DItemsList::getCurrentUrl() const
{
    DItemsListViewItem* const currentItem = d->listView->getCurrentItem();

    if (!currentItem)
    {
        return QUrl();
    }

    return currentItem->url();
}

} // namespace Digikam
