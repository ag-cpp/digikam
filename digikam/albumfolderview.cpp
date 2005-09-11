/* ============================================================
 * File  : albumfolderview.cpp
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-05-06
 * Copyright 2005 by Joern Ahrens
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
 * ============================================================ */

#include <qpixmap.h>
#include <qguardedptr.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qdatastream.h>
#include <qvaluelist.h>
#include <qdatetime.h>

#include <klocale.h>
#include <kglobal.h>
#include <kcalendarsystem.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kfiledialog.h>

#include "digikamapp.h"
#include "album.h"
#include "albumfolderview.h"
#include "albumpropsedit.h"
#include "album.h"
#include "albummanager.h"
#include "albummanager.h"
#include "albumsettings.h"
#include "thumbnailjob.h"
#include "thumbnailsize.h"
#include "albumpropsedit.h"
#include "folderitem.h"
#include "dio.h"
#include "dragobjects.h"

extern "C"
{
#include <X11/Xlib.h>
}


//-----------------------------------------------------------------------------
// AlbumFolderViewItem
//-----------------------------------------------------------------------------

class AlbumFolderViewItem : public FolderItem
{
public:
    AlbumFolderViewItem(QListView *parent, PAlbum *album);
    AlbumFolderViewItem(QListViewItem *parent, PAlbum *album);

    // special group item (collection/dates)
    AlbumFolderViewItem(QListViewItem* parent, const QString& name,
                        int year, int month);

    PAlbum* getAlbum() const;
    int id() const;
    bool isGroupItem() const;

    int compare(QListViewItem *i, int col, bool ascending) const;

private:
    PAlbum      *m_album;
    int          m_year;
    int          m_month;
    bool         m_groupItem;
};

AlbumFolderViewItem::AlbumFolderViewItem(QListView *parent, PAlbum *album)
    : FolderItem(parent, album->title())
{
    setDragEnabled(true);
    m_album = album;
    m_groupItem = false;
}

AlbumFolderViewItem::AlbumFolderViewItem(QListViewItem *parent, PAlbum *album)
    : FolderItem(parent, album->title())
{
    setDragEnabled(true);
    m_album = album;
    m_groupItem = false;
}

// special group item (collection/dates)
AlbumFolderViewItem::AlbumFolderViewItem(QListViewItem* parent, const QString& name,
                                         int year, int month)
    : FolderItem(parent, name, true),
      m_album(0), m_year(year), m_month(month), m_groupItem(true)
{
    setDragEnabled(false);
}

PAlbum* AlbumFolderViewItem::getAlbum() const
{
    return m_album;
}

int AlbumFolderViewItem::id() const
{
    if (m_groupItem)
    {
        if (m_year != 0 && m_month != 0)
        {
            return (m_year*(-100) + m_month*(-1));
        }
        else
        {
            return ( - (AlbumSettings::instance()->getAlbumCollectionNames()
                        .findIndex(text(0)) ) );
        }
    }
    else
    {
        return m_album ? m_album->id() : 0;
    }
}

bool AlbumFolderViewItem::isGroupItem() const
{
    return m_groupItem;
}

int AlbumFolderViewItem::compare(QListViewItem *i, int col, bool ascending) const
{
    if (!m_groupItem || m_year == 0 || m_month == 0)
        return QListViewItem::compare(i, col, ascending);

    AlbumFolderViewItem* thatItem = dynamic_cast<AlbumFolderViewItem*>(i);
    if (!thatItem)
        return 0;

    int myWeight  = m_year*100 + m_month;
    int hisWeight = thatItem->m_year*100 + thatItem->m_month;

    if (myWeight == hisWeight)
        return 0;
    else if (myWeight > hisWeight)
        return 1;
    else
        return -1;
}


//-----------------------------------------------------------------------------
// AlbumFolderViewPriv
//-----------------------------------------------------------------------------

class AlbumFolderViewPriv
{
public:

    AlbumManager                     *albumMan;
    ThumbnailJob                     *iconThumbJob;
    QValueList<AlbumFolderViewItem*>  groupItems;
};

//-----------------------------------------------------------------------------
// AlbumFolderView
//-----------------------------------------------------------------------------

AlbumFolderView::AlbumFolderView(QWidget *parent)
    : FolderView(parent, "AlbumFolderView")
{
    d = new AlbumFolderViewPriv();

    d->albumMan = AlbumManager::instance();
    d->iconThumbJob = 0;

    addColumn(i18n("My Albums"));
    setResizeMode(QListView::LastColumn);
    setRootIsDecorated(false);
    setAllColumnsShowFocus(true);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    connect(d->albumMan, SIGNAL(signalAlbumAdded(Album*)),
            SLOT(slotAlbumAdded(Album*)));
    connect(d->albumMan, SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));
    connect(d->albumMan, SIGNAL(signalAlbumsCleared()),
            SLOT(slotAlbumsCleared()));
    connect(d->albumMan, SIGNAL(signalAlbumIconChanged(Album*)),
            this, SLOT(slotAlbumIconChanged(Album*)));
    connect(d->albumMan, SIGNAL(signalAlbumRenamed(Album*)),
            this, SLOT(slotAlbumRenamed(Album*)));

    connect(this, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
            SLOT(slotContextMenu(QListViewItem*, const QPoint&, int)));

    connect(this, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

AlbumFolderView::~AlbumFolderView()
{
    delete d->iconThumbJob;
    delete d;
}

void AlbumFolderView::slotAlbumAdded(Album *album)
{
    if(!album)
        return;

    PAlbum *palbum = dynamic_cast<PAlbum*>(album);
    if(!palbum)
        return;

    bool failed;
    AlbumFolderViewItem* parent = findParent(palbum, failed);
    if (failed)
    {
        kdWarning() << k_funcinfo << " Failed to find parent for Album "
                    << palbum->url() << endl;
        return;
    }

    KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();
    AlbumFolderViewItem *item;
    if (!parent)
    {
        // root album
        item = new AlbumFolderViewItem(this, palbum);
        palbum->setExtraData(this, item);
        item->setPixmap(0, iconLoader->loadIcon("folder_red", KIcon::NoGroup,
                        32, KIcon::DefaultState, 0, true));
    }
    else
    {
        item = new AlbumFolderViewItem(parent, palbum);
        palbum->setExtraData(this, item);

        item->setPixmap(0, iconLoader->loadIcon("folder", KIcon::NoGroup,
                        32, KIcon::DefaultState, 0, true));
    }

    setAlbumThumbnail(palbum);
}

void AlbumFolderView::slotAlbumDeleted(Album *album)
{
    if(!album)
        return;

    PAlbum* palbum = dynamic_cast<PAlbum*>(album);
    if(!palbum)
        return;

    if(!palbum->icon().isEmpty() && !d->iconThumbJob)
        d->iconThumbJob->removeItem(palbum->icon());

    AlbumFolderViewItem* item = (AlbumFolderViewItem*) palbum->extraData(this);
    if(item)
    {
        AlbumFolderViewItem *itemParent =
                dynamic_cast<AlbumFolderViewItem*>(item->parent());

        if(itemParent)
            itemParent->takeItem(item);
        else
            takeItem(item);

        delete item;
        clearEmptyGroupItems();
    }
}

void AlbumFolderView::slotAlbumRenamed(Album *album)
{
    PAlbum* palbum = dynamic_cast<PAlbum*>(album);
    if(!palbum)
        return;

    AlbumFolderViewItem* item = (AlbumFolderViewItem*) palbum->extraData(this);
    if(item)
    {
        item->setText(0, palbum->title());
    }
}

void AlbumFolderView::slotAlbumsCleared()
{
    d->groupItems.clear();
    clear();
}

void AlbumFolderView::setAlbumThumbnail(PAlbum *album)
{
    if(!album)
        return;

    AlbumFolderViewItem* item = (AlbumFolderViewItem*) album->extraData(this);

    if(!item)
        return;

    if(!album->icon().isEmpty())
    {
        if(!d->iconThumbJob)
        {
            d->iconThumbJob = new ThumbnailJob(album->iconKURL(),
                                               (int)ThumbnailSize::Tiny,
                                               true,
                                               AlbumSettings::instance()->getExifRotate());
            connect(d->iconThumbJob,
                    SIGNAL(signalThumbnail(const KURL&, const QPixmap&)),
                    this,
                    SLOT(slotGotThumbnailFromIcon(const KURL&, const QPixmap&)));
            connect(d->iconThumbJob,
                    SIGNAL(signalFailed(const KURL&)),
                    SLOT(slotThumbnailLost(const KURL&)));
        }
        else
        {
            d->iconThumbJob->addItem(album->iconKURL());
        }
    }
    else
    {
        KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();
        if(album->isRoot())
        {
            item->setPixmap(0, iconLoader->loadIcon("folder_image", KIcon::NoGroup,
                                                    32, KIcon::DefaultState,
                                                    0, true));
        }
        else
        {
            item->setPixmap(0, iconLoader->loadIcon("folder", KIcon::NoGroup,
                            32, KIcon::DefaultState,
                            0, true));
        }
    }
}

void AlbumFolderView::slotGotThumbnailFromIcon(const KURL& url,
                                               const QPixmap& thumbnail)
{
    PAlbum* album = d->albumMan->findPAlbum(url.directory());

    if(!album)
        return;

    AlbumFolderViewItem* item = (AlbumFolderViewItem*)album->extraData(this);

    if(!item)
        return;

    item->setPixmap(0, thumbnail);
}

void AlbumFolderView::slotThumbnailLost(const KURL &url)
{
    PAlbum *album = AlbumManager::instance()->findPAlbum(url.directory());
    if(!album)
        return;

    AlbumFolderViewItem *item = (AlbumFolderViewItem*)album->extraData(this);

    if(item)
    {
        KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();
        item->setPixmap(0, iconLoader->loadIcon("folder", KIcon::NoGroup, 32,
                                                KIcon::DefaultState, 0, true));
    }
}

void AlbumFolderView::slotAlbumIconChanged(Album* album)
{
    if(!album || album->type() != Album::PHYSICAL)
        return;

    AlbumFolderViewItem *item = (AlbumFolderViewItem*)album->extraData(this);

    if(item)
        setAlbumThumbnail((PAlbum*)album);
}

void AlbumFolderView::slotSelectionChanged()
{
    if(!active())
        return;

    QListViewItem* selItem = 0;
    QListViewItemIterator it(this);
    while(it.current())
    {
        if(it.current()->isSelected())
        {
            selItem = it.current();
            break;
        }
        ++it;
    }

    if(!selItem)
    {
        d->albumMan->setCurrentAlbum(0);
        return;
    }

    AlbumFolderViewItem *albumitem = dynamic_cast<AlbumFolderViewItem*>(selItem);
    if(!albumitem)
    {
        d->albumMan->setCurrentAlbum(0);
        return;
    }

    d->albumMan->setCurrentAlbum(albumitem->getAlbum());
}

void AlbumFolderView::slotContextMenu(QListViewItem *listitem, const QPoint &, int)
{
    QPopupMenu popmenu(this);
    KActionMenu menuImport(i18n("Import"));
    KActionMenu menuKIPIBatch(i18n("Batch Processes"));

    popmenu.insertItem(SmallIcon("album"), i18n("New Album..."), 10);

    AlbumFolderViewItem *item = dynamic_cast<AlbumFolderViewItem*>(listitem);
    if (item && !item->getAlbum())
    {
        // if collection/date return
        return;
    }

    // Root folder only shows "New Album..."
    if(item && item->parent())
    {
        popmenu.insertItem(SmallIcon("pencil"), i18n("Edit Album Properties..."), 11);

        popmenu.insertSeparator();

        KAction *action;
        // Add KIPI Albums plugins Actions
        const QPtrList<KAction>& albumActions =
                DigikamApp::getinstance()->menuAlbumActions();
        if(!albumActions.isEmpty())
        {
            QPtrListIterator<KAction> it(albumActions);
            while((action = it.current()))
            {
                action->plug(&popmenu);
                ++it;
            }
        }

        // Add All Import Actions
        const QPtrList<KAction> importActions =
                DigikamApp::getinstance()->menuImportActions();
        if(!importActions.isEmpty())
        {
            QPtrListIterator<KAction> it3(importActions);
            while((action = it3.current()))
            {
                menuImport.insert(action);
                ++it3;
            }
            menuImport.plug(&popmenu);
        }

        // Add KIPI Batch processes plugins Actions
        const QPtrList<KAction>& batchActions =
                DigikamApp::getinstance()->menuBatchActions();
        if(!batchActions.isEmpty())
        {
            QPtrListIterator<KAction> it2(batchActions);
            while((action = it2.current()))
            {
                menuKIPIBatch.insert(action);
                ++it2;
            }
            menuKIPIBatch.plug(&popmenu);
        }

        if(!albumActions.isEmpty() || !batchActions.isEmpty() ||
           !importActions.isEmpty())
        {
            popmenu.insertSeparator();
        }

        if(AlbumSettings::instance()->getUseTrash())
        {
            popmenu.insertItem(SmallIcon("edittrash"),
                               i18n("Move Album to Trash"), 12);
        }
        else
        {
            popmenu.insertItem(SmallIcon("editdelete"),
                               i18n("Delete Album"), 12);
        }
    }

    switch(popmenu.exec((QCursor::pos())))
    {
        case 10:
        {
            albumNew(item);
            break;
        }
        case 11:
        {
            albumEdit(item);
            break;
        }
        case 12:
        {
            albumDelete(item);
            break;
        }
        default:
            break;
    }
}

void AlbumFolderView::albumNew()
{
    AlbumFolderViewItem *item = dynamic_cast<AlbumFolderViewItem*>(selectedItem());
    if (!item)
    {
        item = dynamic_cast<AlbumFolderViewItem*>(firstChild());
    }

    if (!item)
        return;

    albumNew(item);
}

void AlbumFolderView::albumNew(AlbumFolderViewItem *item)
{
    AlbumSettings* settings = AlbumSettings::instance();
    if(!settings)
    {
        kdWarning() << "AlbumFolderView: Couldn't get Album Settings" << endl;
        return;
    }

    QDir libraryDir(settings->getAlbumLibraryPath());
    if(!libraryDir.exists())
    {
        KMessageBox::error(0,
                           i18n("The Albums Library has not been set correctly.\n"
                                "Select \"Configure Digikam\" from the Settings "
                                "menu and choose a folder to use for the Albums "
                                "Library."));
        return;
    }

    PAlbum *parent;

    if(!item)
        parent = d->albumMan->findPAlbum(0);
    else
        parent = item->getAlbum();

    if (!parent)
        return;

    QString     title;
    QString     comments;
    QString     collection;
    QDate       date;
    QStringList albumCollections;

    if(!AlbumPropsEdit::createNew(parent, title, comments, date, collection,
                                  albumCollections))
        return;

    QStringList oldAlbumCollections(AlbumSettings::instance()->getAlbumCollectionNames());
    if(albumCollections != oldAlbumCollections)
    {
        AlbumSettings::instance()->setAlbumCollectionNames(albumCollections);
        resort();
    }

    QString errMsg;
    PAlbum* album = d->albumMan->createPAlbum(parent, title, comments,
                                              date, collection, errMsg);
    if (!album)
    {
        KMessageBox::error(0, errMsg);
        return;
    }

    // by this time the signalAlbumAdded has been fired and the appropriate
    // AlbumFolderViewItem has been created. Now make this folderviewitem visible
    AlbumFolderViewItem* newItem = (AlbumFolderViewItem*)album->extraData(this);
    if (newItem)
    {
        if(item)
            item->setOpen(true);
        ensureItemVisible(newItem);
        setSelected(newItem, true);
    }
}

void AlbumFolderView::albumDelete()
{
    AlbumFolderViewItem *item = dynamic_cast<AlbumFolderViewItem*>(selectedItem());
    if(!item)
        return;

    albumDelete(item);
}

void AlbumFolderView::albumDelete(AlbumFolderViewItem *item)
{
    PAlbum *album = item->getAlbum();

    if(!album || album->isRoot())
        return;

    // find number of subalbums
    int children = 0;
    AlbumIterator it(album);
    while(it.current())
    {
        children++;
        ++it;
    }

    int result = KMessageBox::No;
    AlbumSettings* settings = AlbumSettings::instance();
    if (children)
    {
        if(settings->getUseTrash())
        {
            result = KMessageBox::warningYesNo(this,
                        i18n("Album '%1' has one subalbum. "
                             "Moving this to trash will also move the "
                             "subalbum to trash. "
                             "Are you sure you want to continue?",
                             "Album '%1' has %n subalbums. "
                             "Moving this to trash will also move the "
                             "subalbums to trash. "
                             "Are you sure you want to continue?",
                             children)
                             .arg(album->title()));
        }
        else
        {
            result = KMessageBox::warningYesNo(this,
                        i18n("Album '%1' has one subalbum. "
                             "Deleting this will also delete "
                             "the subalbum. "
                             "Are you sure you want to continue?",
                             "Album '%1' has %n subalbums. "
                             "Deleting this will also delete "
                             "the subalbums. "
                             "Are you sure you want to continue?",
                             children)
                             .arg(album->title()));
        }
    }
    else
    {
        result = KMessageBox::warningYesNo(this, settings->getUseTrash() ?
                i18n("Move album '%1' to trash?").arg(album->title()) :
                i18n("Delete album '%1' from disk?").arg(album->title()),
                settings->getUseTrash() ? i18n("Trash Album") : i18n("Delete Album"),
                settings->getUseTrash() ? KGuiItem(i18n("Trash"),"edittrash") : KGuiItem(i18n("Delete"),"editdelete") );
    }

    if(result == KMessageBox::Yes)
    {
        // TODO: currently trash kioslave can handle only full paths.
        // pass full folder path to the trashing job
        KURL u;
        u.setProtocol("file");
        u.setPath(album->folderPath());
        KIO::Job* job = DIO::del(u);
        connect(job, SIGNAL(result(KIO::Job *)),
                this, SLOT(slotDIOResult(KIO::Job *)));
    }
}

void AlbumFolderView::slotDIOResult(KIO::Job* job)
{
    if (job->error())
        job->showErrorDialog(this);
}

void AlbumFolderView::albumEdit()
{
    AlbumFolderViewItem *item = dynamic_cast<AlbumFolderViewItem*>(selectedItem());
    if(!item)
        return;

    albumEdit(item);
}

void AlbumFolderView::albumEdit(AlbumFolderViewItem* item)
{
    PAlbum *album = item->getAlbum();

    if (!album)
        return;

    QString     oldTitle(album->title());
    QString     oldComments(album->caption());
    QString     oldCollection(album->collection());
    QDate       oldDate(album->date());
    QStringList oldAlbumCollections(AlbumSettings::instance()->getAlbumCollectionNames());

    QString     title, comments, collection;
    QDate       date;
    QStringList albumCollections;

    if(AlbumPropsEdit::editProps(album, title, comments, date,
                                  collection, albumCollections))
    {
        if(comments != oldComments)
            album->setCaption(comments);

        if(date != oldDate && date.isValid())
            album->setDate(date);

        if(collection != oldCollection)
            album->setCollection(collection);

        AlbumSettings::instance()->setAlbumCollectionNames(albumCollections);
        resort();

        // Do this last : so that if anything else changed we can
        // successfully save to the db with the old name

        if(title != oldTitle)
        {
            QString errMsg;
            if (!d->albumMan->renamePAlbum(album, title, errMsg))
                KMessageBox::error(0, errMsg);
        }

        emit signalAlbumModified();
    }
}

QDragObject* AlbumFolderView::dragObject()
{
    AlbumFolderViewItem *item = dynamic_cast<AlbumFolderViewItem*>(dragItem());
    if(!item)
        return 0;

    PAlbum *album = item->getAlbum();
    if(album->isRoot())
        return 0;

    AlbumDrag *a = new AlbumDrag(album->kurl(), album->id(), this);
    if(!a)
        return 0;
    a->setPixmap(*item->pixmap(0));

    return a;
}

bool AlbumFolderView::acceptDrop(const QDropEvent *e) const
{
    QPoint vp = contentsToViewport(e->pos());
    AlbumFolderViewItem *itemDrop = dynamic_cast<AlbumFolderViewItem*>(itemAt(vp));
    AlbumFolderViewItem *itemDrag = dynamic_cast<AlbumFolderViewItem*>(dragItem());

    if(AlbumDrag::canDecode(e))
    {
        switch(AlbumSettings::instance()->getAlbumSortOrder())
        {
        case(AlbumSettings::ByFolder):
        {
            // Allow dragging at the root, to move the album at the root
            if(!itemDrop)
                return true;

            // Dragging an item on itself makes no sense
            if(itemDrag == itemDrop)
                return false;

            // Dragging a parent on its child makes no sense
            if(itemDrag && itemDrag->getAlbum()->isAncestorOf(itemDrop->getAlbum()))
                return false;

            return true;
        }
        case (AlbumSettings::ByCollection):
        {
            if (!itemDrop)
                return false;

            // Only allow dragging onto Collection
            if (itemDrop->isGroupItem())
                return true;

            return false;
        }
        default:
        {
            return false;
        }
        };
    }

    if(itemDrop  && !itemDrop->parent())
    {
        // Do not allow drop images on album root
        return false;
    }

    if (itemDrop && itemDrop->isGroupItem())
    {
        // do not allow drop on a group item
        return false;
    }

    if(ItemDrag::canDecode(e))
    {
        return true;
    }

    if(QUriDrag::canDecode(e))
    {
        return true;
    }

    return false;
}

void AlbumFolderView::contentsDropEvent(QDropEvent *e)
{
    FolderView::contentsDropEvent(e);

    if(!acceptDrop(e))
        return;

    QPoint vp = contentsToViewport(e->pos());
    AlbumFolderViewItem *itemDrop = dynamic_cast<AlbumFolderViewItem*>(itemAt(vp));

    if(AlbumDrag::canDecode(e))
    {
        AlbumFolderViewItem *itemDrag = dynamic_cast<AlbumFolderViewItem*>(dragItem());
        if(!itemDrag)
            return;

        if (AlbumSettings::instance()->getAlbumSortOrder()
            == AlbumSettings::ByFolder)
        {
            // TODO: Copy?
            QPopupMenu popMenu(this);
            popMenu.insertItem(SmallIcon("goto"), i18n("&Move Here"), 10);
            popMenu.insertSeparator(-1);
            popMenu.insertItem(SmallIcon("cancel"), i18n("C&ancel"), 20);
            popMenu.setMouseTracking(true);
            int id = popMenu.exec(QCursor::pos());

            if(id == 10)
            {
                PAlbum *album = itemDrag->getAlbum();
                PAlbum *destAlbum;
                if(!itemDrop)
                {
                    // move dragItem to the root
                    destAlbum = d->albumMan->findPAlbum(0);
                }
                else
                {
                    // move dragItem below dropItem
                    destAlbum = itemDrop->getAlbum();
                }
                KIO::Job* job = DIO::move(album->kurl(), destAlbum->kurl());
                connect(job, SIGNAL(result(KIO::Job*)),
                        SLOT(slotDIOResult(KIO::Job*)));
            }
        }
        else if (AlbumSettings::instance()->getAlbumSortOrder()
                 == AlbumSettings::ByCollection)
        {
            if (!itemDrop)
                return;

            if (itemDrop->isGroupItem())
            {
                PAlbum *album = itemDrag->getAlbum();
                if (!album)
                    return;

                album->setCollection(itemDrop->text(0));
                resort();
            }
        }

        return;
    }

    if (ItemDrag::canDecode(e))
    {
        if (!itemDrop)
            return;

        PAlbum *destAlbum = itemDrop->getAlbum();
        PAlbum *srcAlbum;

        KURL::List      urls;
        KURL::List      kioURLs;
        QValueList<int> albumIDs;
        QValueList<int> imageIDs;

        if (!ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs))
            return;

        if (urls.isEmpty() || kioURLs.isEmpty() || albumIDs.isEmpty() || imageIDs.isEmpty())
            return;

        // all the albumids will be the same
        int albumID = albumIDs.first();
        srcAlbum = d->albumMan->findPAlbum(albumID);
        if (!srcAlbum)
        {
            kdWarning() << "Could not find source album of drag"
                    << endl;
            return;
        }

        int id = 0;
        char keys_return[32];
        XQueryKeymap(x11Display(), keys_return);
        int key_1 = XKeysymToKeycode(x11Display(), 0xFFE3);
        int key_2 = XKeysymToKeycode(x11Display(), 0xFFE4);
        int key_3 = XKeysymToKeycode(x11Display(), 0xFFE1);
        int key_4 = XKeysymToKeycode(x11Display(), 0xFFE2);

        if(srcAlbum == destAlbum)
        {
            // Setting the dropped image as the album thumbnail
            // If the ctrl key is pressed, when dropping the image, the
            // thumbnail is set without a popup menu
            if (((keys_return[key_1 / 8]) && (1 << (key_1 % 8))) ||
                ((keys_return[key_2 / 8]) && (1 << (key_2 % 8))))
            {
                id = 12;
            }
            else
            {
                QPopupMenu popMenu(this);
                popMenu.insertItem(i18n("Set as Album Thumbnail"), 12);
                popMenu.insertSeparator(-1);
                popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

                popMenu.setMouseTracking(true);
                id = popMenu.exec(QCursor::pos());
            }

            if(id == 12)
            {
                QString errMsg;
                AlbumManager::instance()->updatePAlbumIcon(destAlbum, imageIDs.first(), errMsg);
            }
            return;
        }

        // If shift key is pressed while dragging, move the drag object without
        // displaying popup menu -> move
        if (((keys_return[key_3 / 8]) && (1 << (key_3 % 8))) ||
            ((keys_return[key_4 / 8]) && (1 << (key_4 % 8))))
        {
            id = 10;
        }
        // If ctrl key is pressed while dragging, copy the drag object without
        // displaying popup menu -> copy
        else if (((keys_return[key_1 / 8]) && (1 << (key_1 % 8))) ||
                 ((keys_return[key_2 / 8]) && (1 << (key_2 % 8))))
        {
            id = 11;
        }
        else
        {
            QPopupMenu popMenu(this);
            popMenu.insertItem( SmallIcon("goto"), i18n("&Move Here"), 10 );
            popMenu.insertItem( SmallIcon("editcopy"), i18n("&Copy Here"), 11 );
            popMenu.insertSeparator(-1);
            popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

            popMenu.setMouseTracking(true);
            id = popMenu.exec(QCursor::pos());
        }

        switch(id)
        {
            case 10:
            {
                KIO::Job* job = DIO::move(kioURLs, destAlbum->kurl());
                connect(job, SIGNAL(result(KIO::Job*)),
                        SLOT(slotDIOResult(KIO::Job*)));
                break;
            }
            case 11:
            {
                KIO::Job* job = DIO::copy(kioURLs, destAlbum->kurl());
                connect(job, SIGNAL(result(KIO::Job*)),
                        SLOT(slotDIOResult(KIO::Job*)));
                break;
            }
            default:
                break;
        }

        return;
    }

    if(QUriDrag::canDecode(e))
    {
        // DnD from an external source

        PAlbum* destAlbum = 0;

        if (itemDrop)
            destAlbum = itemDrop->getAlbum();
        else
            destAlbum = d->albumMan->findPAlbum(0);

        KURL destURL(destAlbum->kurl());

        KURL::List srcURLs;
        KURLDrag::decode(e, srcURLs);

        char keys_return[32];
        XQueryKeymap(x11Display(), keys_return);
        int id = 0;

        int key_1 = XKeysymToKeycode(x11Display(), 0xFFE3);
        int key_2 = XKeysymToKeycode(x11Display(), 0xFFE4);
        int key_3 = XKeysymToKeycode(x11Display(), 0xFFE1);
        int key_4 = XKeysymToKeycode(x11Display(), 0xFFE2);
        // If shift key is pressed while dropping, move the drag object without
        // displaying popup menu -> move
        if(((keys_return[key_3 / 8]) && (1 << (key_3 % 8))) ||
           ((keys_return[key_4 / 8]) && (1 << (key_4 % 8))))
        {
            id = 10;
        }
        // If ctrl key is pressed while dropping, copy the drag object without
        // displaying popup menu -> copy
        else if(((keys_return[key_1 / 8]) && (1 << (key_1 % 8))) ||
                ((keys_return[key_2 / 8]) && (1 << (key_2 % 8))))
        {
            id = 11;
        }
        else
        {
            QPopupMenu popMenu(this);
            popMenu.insertItem( SmallIcon("goto"), i18n("&Move Here"), 10 );
            popMenu.insertItem( SmallIcon("editcopy"), i18n("&Copy Here"), 11 );
            popMenu.insertSeparator(-1);
            popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

            popMenu.setMouseTracking(true);
            id = popMenu.exec(QCursor::pos());
        }

        switch(id)
        {
            case 10: {
                KIO::Job* job = DIO::move(srcURLs, destAlbum->kurl());
                connect(job, SIGNAL(result(KIO::Job*)),
                        SLOT(slotDIOResult(KIO::Job*)));
                break;
            }
            case 11: {
                KIO::Job* job = DIO::copy(srcURLs, destAlbum->kurl());
                connect(job, SIGNAL(result(KIO::Job*)),
                        SLOT(slotDIOResult(KIO::Job*)));
                break;
            }
            default:
                break;
        }

        return;
    }
}

void AlbumFolderView::albumImportFolder()
{
    AlbumSettings* settings = AlbumSettings::instance();
    QDir libraryDir(settings->getAlbumLibraryPath());
    if(!libraryDir.exists())
    {
        KMessageBox::error(0,
                           i18n("The Albums Library has not been set correctly.\n"
                                "Select \"Configure Digikam\" from the Settings "
                                "menu and choose a folder to use for the Albums "
                                "Library."));
        return;
    }

    PAlbum* parent = 0;
    if(selectedItem())
    {
        AlbumFolderViewItem *folderItem =
                dynamic_cast<AlbumFolderViewItem*>(selectedItem());
        Album *album = folderItem->getAlbum();
        if (album && album->type() == Album::PHYSICAL)
        {
            parent = dynamic_cast<PAlbum*>(album);
        }
    }
    if(!parent)
        parent = dynamic_cast<PAlbum*>(d->albumMan->findPAlbum(0));

    QString libraryPath = parent->folderPath();

    KFileDialog dlg(QString::null, "inode/directory", this, "importFolder", true);
    dlg.setMode(KFile::Directory |  KFile::Files);
    if(dlg.exec() != QDialog::Accepted)
        return;

    KURL::List urls = dlg.selectedURLs();
    if(urls.empty())
        return;

    KIO::Job* job = DIO::copy(urls, parent->kurl());
    connect(job, SIGNAL(result(KIO::Job *)),
            this, SLOT(slotDIOResult(KIO::Job *)));
}

void AlbumFolderView::selectItem(int id)
{
    PAlbum* album = d->albumMan->findPAlbum(id);
    if(!album)
        return;

    AlbumFolderViewItem *item =
            (AlbumFolderViewItem*)album->extraData(this);
    if(item)
    {
        setSelected(item, true);
        ensureItemVisible(item);
    }
}

AlbumFolderViewItem* AlbumFolderView::findParent(PAlbum* album, bool& failed)
{
    if (album->isRoot())
    {
        failed = false;
        return 0;
    }

    switch(AlbumSettings::instance()->getAlbumSortOrder())
    {
    case(AlbumSettings::ByFolder):
    {
        return findParentByFolder(album, failed);
    }
    case(AlbumSettings::ByCollection):
    {
        return findParentByCollection(album, failed);
    }
    case(AlbumSettings::ByDate):
    {
        return findParentByDate(album, failed);
    }
    };

    failed = true;
    return 0;
}

AlbumFolderViewItem* AlbumFolderView::findParentByFolder(PAlbum* album, bool& failed)
{
    AlbumFolderViewItem* parent =
        (AlbumFolderViewItem*) album->parent()->extraData(this);
    if (!parent)
    {
        failed = true;
        return 0;
    }

    failed = false;
    return parent;
}

AlbumFolderViewItem* AlbumFolderView::findParentByCollection(PAlbum* album, bool& failed)
{
    QStringList collectionList =
        AlbumSettings::instance()->getAlbumCollectionNames();
    QString collection = album->collection();

    if (collection.isEmpty() || !collectionList.contains(collection))
        collection = i18n("Uncategorized Albums");

    AlbumFolderViewItem* parent = 0;

    for (QValueList<AlbumFolderViewItem*>::iterator it=d->groupItems.begin();
         it != d->groupItems.end(); ++it)
    {
        AlbumFolderViewItem* groupItem = *it;
        if (groupItem->text(0) == collection)
        {
            parent = groupItem;
            break;
        }
    }

    // Need to create a new parent item
    if (!parent)
    {
        parent = new AlbumFolderViewItem(firstChild(), collection, 0, 0);
        d->groupItems.append(parent);
    }

    failed = false;
    return parent;
}

AlbumFolderViewItem* AlbumFolderView::findParentByDate(PAlbum* album, bool& failed)
{
    QDate date = album->date();

    QString timeString = QString::number(date.year()) + ", " +
                         KGlobal::locale()->calendar()->monthName(date, false);

    AlbumFolderViewItem* parent = 0;

    for (QValueList<AlbumFolderViewItem*>::iterator it=d->groupItems.begin();
         it != d->groupItems.end(); ++it)
    {
        AlbumFolderViewItem* groupItem = *it;
        if (groupItem->text(0) == timeString)
        {
            parent = groupItem;
            break;
        }
    }

    // Need to create a new parent item
    if (!parent)
    {
        parent = new AlbumFolderViewItem(firstChild(), timeString,
                                         date.year(), date.month());
        d->groupItems.append(parent);
    }

    failed = false;
    return parent;
}

void AlbumFolderView::resort()
{
    AlbumFolderViewItem* prevSelectedItem =
        dynamic_cast<AlbumFolderViewItem*>(selectedItem());
    if (prevSelectedItem && prevSelectedItem->isGroupItem())
        prevSelectedItem = 0;

    AlbumList pList(AlbumManager::instance()->allPAlbums());
    for (AlbumList::iterator it = pList.begin(); it != pList.end(); ++it)
    {
        PAlbum *album = (PAlbum*)(*it);
        if (!album->isRoot() && album->extraData(this))
        {
            reparentItem(static_cast<AlbumFolderViewItem*>(album->extraData(this)));
        }
    }

    // Clear any groupitems which have been left empty
    clearEmptyGroupItems();

    if (prevSelectedItem)
    {
        ensureItemVisible(prevSelectedItem);
        setSelected(prevSelectedItem, true);
    }
}

void AlbumFolderView::reparentItem(AlbumFolderViewItem* folderItem)
{
    if (!folderItem)
        return;

    PAlbum* album = folderItem->getAlbum();
    if (!album || album->isRoot())
        return;

    AlbumFolderViewItem* oldParent =
        dynamic_cast<AlbumFolderViewItem*>(folderItem->parent());

    bool failed;
    AlbumFolderViewItem* newParent = findParent(album, failed);
    if (failed)
        return;

    if (oldParent == newParent)
        return;

    if (oldParent)
        oldParent->removeItem(folderItem);
    else
        removeItem(folderItem);

    // insert into new parent
    if (newParent)
        newParent->insertItem(folderItem);
    else
        insertItem(folderItem);
}

void AlbumFolderView::clearEmptyGroupItems()
{
    QValueList<AlbumFolderViewItem*> deleteItems;

    for (QValueList<AlbumFolderViewItem*>::iterator it=d->groupItems.begin();
         it != d->groupItems.end(); ++it)
    {
        AlbumFolderViewItem* groupItem = *it;

        if (!groupItem->firstChild())
        {
            deleteItems.append(groupItem);
        }
    }

    for (QValueList<AlbumFolderViewItem*>::iterator it=deleteItems.begin();
         it != deleteItems.end(); ++it)
    {
        d->groupItems.remove(*it);
        delete *it;
    }
}

#include "albumfolderview.moc"
