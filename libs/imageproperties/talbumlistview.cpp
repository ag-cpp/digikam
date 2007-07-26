/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-18-12
 * Description : A list view to display digiKam Tags.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <Q3Header>
#include <Q3ValueList>
#include <QDropEvent>
#include <QMouseEvent>

// KDE includes.

#include <k3popupmenu.h>
#include <kmenu.h>
#include <klocale.h>
#include <kurl.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kdialog.h>

// Local includes.

#include "ddebug.h"
#include "albumiconitem.h"
#include "albummanager.h"
#include "albumdb.h"
#include "album.h"
#include "albumsettings.h"
#include "databasetransaction.h"
#include "imageinfo.h"
#include "tagcreatedlg.h"
#include "navigatebarwidget.h"
#include "dragobjects.h"
#include "imageattributeswatch.h"
#include "albumthumbnailloader.h"
#include "statusprogressbar.h"
#include "talbumlistview.h"
#include "talbumlistview.moc"

namespace Digikam
{

TAlbumCheckListItem::TAlbumCheckListItem(Q3ListView* parent, TAlbum* album)
                   : Q3CheckListItem(parent, album->title()), m_album(album)
{
    setDragEnabled(true);
}

TAlbumCheckListItem::TAlbumCheckListItem(Q3CheckListItem* parent, TAlbum* album)
                   : Q3CheckListItem(parent, album->title(), Q3CheckListItem::CheckBox),
                     m_album(album)
{
    setDragEnabled(true);
}

void TAlbumCheckListItem::setStatus(MetadataHub::TagStatus status)
{
    if (status == MetadataHub::MetadataDisjoint)
    {
        setState(Q3CheckListItem::NoChange);
    }
    else
    {
        setOn(status.hasTag);
    }
}

void TAlbumCheckListItem::stateChange(bool val)
{
    Q3CheckListItem::stateChange(val);
    TAlbumListView* view = dynamic_cast<TAlbumListView*>(listView());
    view->emitSignalItemStateChanged(this);
}

// ------------------------------------------------------------------------

class TAlbumListViewPriv
{

public:

    TAlbumListViewPriv()
    {
        dragItem = 0;
    }

    QPoint               dragStartPos;    

    TAlbumCheckListItem *dragItem;
};

TAlbumListView::TAlbumListView(QWidget* parent)
              : Q3ListView(parent)
{
    d = new TAlbumListViewPriv;
    addColumn(i18n("Tags"));
    header()->hide();
    setResizeMode(Q3ListView::LastColumn);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
}

TAlbumListView::~TAlbumListView()
{
    delete d;
}

void TAlbumListView::emitSignalItemStateChanged(TAlbumCheckListItem *item)
{
    emit signalItemStateChanged(item);
}

void TAlbumListView::contentsMouseMoveEvent(QMouseEvent *e)
{
    Q3ListView::contentsMouseMoveEvent(e);

    if(e->buttons() == Qt::NoButton)
    {
        if(KGlobalSettings::changeCursorOverIcon())
        {
            QPoint vp = contentsToViewport(e->pos());
            Q3ListViewItem *item = itemAt(vp);
            if (mouseInItemRect(item, vp.x()))
                setCursor(Qt::PointingHandCursor);
            else
                unsetCursor();
        }
        return;
    }
    
    if(d->dragItem && 
       (d->dragStartPos - e->pos()).manhattanLength() > QApplication::startDragDistance())
    {
        QPoint vp = contentsToViewport(e->pos());
        TAlbumCheckListItem *item = dynamic_cast<TAlbumCheckListItem*>(itemAt(vp));
        if(!item)
        {
            d->dragItem = 0;
            return;
        }
    }    
}

void TAlbumListView::contentsMousePressEvent(QMouseEvent *e)
{
    QPoint vp = contentsToViewport(e->pos());
    TAlbumCheckListItem *item = dynamic_cast<TAlbumCheckListItem*>(itemAt(vp));

    if(item && e->button() == Qt::RightButton) 
    {
        bool isOn = item->isOn();
        Q3ListView::contentsMousePressEvent(e);
        // Restore the status of checkbox. 
        item->setOn(isOn);
        return;
    }

    if(item && e->button() == Qt::LeftButton) 
    {
        d->dragStartPos = e->pos();
        d->dragItem     = item;
    }

    Q3ListView::contentsMousePressEvent(e);
}

void TAlbumListView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    Q3ListView::contentsMouseReleaseEvent(e);
    d->dragItem = 0;
}

bool TAlbumListView::mouseInItemRect(Q3ListViewItem* item, int x) const
{
    if (!item)
        return false;
    
    x += contentsX();

    int offset = treeStepSize()*(item->depth() + (rootIsDecorated() ? 1 : 0));
    offset    += itemMargin();
    int width  = item->width(fontMetrics(), this, 0);
    
    return (x > offset && x < (offset + width));
}

Q3DragObject* TAlbumListView::dragObject()
{
    TAlbumCheckListItem *item = dynamic_cast<TAlbumCheckListItem*>(dragItem());
    if(!item)
        return 0;

    if(!item->parent())
        return 0;

    TagDrag *t = new TagDrag(item->m_album->id(), this);
    t->setPixmap(*item->pixmap(0));

    return t;
}

void TAlbumListView::startDrag()
{
    Q3DragObject *o = dragObject();
    if(o)
        o->drag();        
}

TAlbumCheckListItem* TAlbumListView::dragItem() const
{
    return d->dragItem;
}

bool TAlbumListView::acceptDrop(const QDropEvent *e) const
{
    QPoint vp = contentsToViewport(e->pos());
    TAlbumCheckListItem *itemDrop = dynamic_cast<TAlbumCheckListItem*>(itemAt(vp));
    TAlbumCheckListItem *itemDrag = dynamic_cast<TAlbumCheckListItem*>(dragItem());

    if(TagDrag::canDecode(e) || TagListDrag::canDecode(e))
    {
        // Allow dragging at the root, to move the tag to the root
        if(!itemDrop)
            return true;

        // Dragging an item on itself makes no sense
        if(itemDrag == itemDrop)
            return false;

        // Dragging a parent on its child makes no sense
        if(itemDrag && itemDrag->m_album->isAncestorOf(itemDrop->m_album))
            return false;

        return true;
    }

    if (ItemDrag::canDecode(e) && itemDrop && itemDrop->m_album->parent())
    {
        // Only other possibility is image items being dropped
        // And allow this only if there is a Tag to be dropped
        // on and also the Tag is not root.
        return true;
    }

    return false;
}

void TAlbumListView::contentsDropEvent(QDropEvent *e)
{
    Q3ListView::contentsDropEvent(e);

    if(!acceptDrop(e))
        return;

    QPoint vp = contentsToViewport(e->pos());
    TAlbumCheckListItem *itemDrop = dynamic_cast<TAlbumCheckListItem*>(itemAt(vp));

    if(TagDrag::canDecode(e))
    {
        QByteArray ba = e->encodedData("digikam/tag-id");
        QDataStream ds(&ba, QIODevice::ReadOnly);
        int tagID;
        ds >> tagID;

        AlbumManager* man = AlbumManager::componentData();
        TAlbum* talbum    = man->findTAlbum(tagID);

        if(!talbum)
            return;

        if (talbum == itemDrop->m_album)
            return;

        K3PopupMenu popMenu(this);
        popMenu.insertTitle(SmallIcon("digikam"), i18n("Tags"));
        popMenu.insertItem(SmallIcon("goto"), i18n("&Move Here"), 10);
        popMenu.insertSeparator(-1);
        popMenu.insertItem(SmallIcon("cancel"), i18n("C&ancel"), 20);
        popMenu.setMouseTracking(true);
        int id = popMenu.exec(QCursor::pos());

        if(id == 10)
        {
            TAlbum *newParentTag = 0;

            if (!itemDrop)
            {
                // move dragItem to the root
                newParentTag = AlbumManager::componentData()->findTAlbum(0);
            }
            else
            {
                // move dragItem as child of dropItem
                newParentTag = itemDrop->m_album;
            }

            QString errMsg;
            if (!AlbumManager::componentData()->moveTAlbum(talbum, newParentTag, errMsg))
            {
                KMessageBox::error(this, errMsg);
            }

            if(itemDrop && !itemDrop->isOpen())
                itemDrop->setOpen(true);
        }

        return;
    }

    if (ItemDrag::canDecode(e))
    {
        TAlbum *destAlbum = itemDrop->m_album;
        TAlbum *srcAlbum;

        KUrl::List       urls;
        KUrl::List       kioURLs;        
        Q3ValueList<int> albumIDs;
        Q3ValueList<int> imageIDs;

        if (!ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs))
            return;

        if (urls.isEmpty() || kioURLs.isEmpty() || albumIDs.isEmpty() || imageIDs.isEmpty())
            return;

        // all the albumids will be the same
        int albumID = albumIDs.first();
        srcAlbum    = AlbumManager::componentData()->findTAlbum(albumID);
        if (!srcAlbum)
        {
            DWarning() << "Could not find source album of drag"
                       << endl;
            return;
        }

        int id = 0;

        if(srcAlbum == destAlbum)
        {
            // Setting the dropped image as the album thumbnail
            // If the ctrl key is pressed, when dropping the image, the
            // thumbnail is set without a popup menu
            if (e->keyboardModifiers() == Qt::ControlModifier)
            {
                id = 12;
            }
            else
            {
                K3PopupMenu popMenu(this);
                popMenu.insertTitle(SmallIcon("digikam"), i18n("Tags"));
                popMenu.insertItem(i18n("Set as Tag Thumbnail"), 12);
                popMenu.insertSeparator(-1);
                popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

                popMenu.setMouseTracking(true);
                id = popMenu.exec(QCursor::pos());
            }

            if(id == 12)
            {
                QString errMsg;
                AlbumManager::componentData()->updateTAlbumIcon(destAlbum, QString(),
                                                                imageIDs.first(), errMsg);
            }
            return;
        }

        // If a ctrl key is pressed while dropping the drag object,
        // the tag is assigned to the images without showing a
        // popup menu.
        if (e->keyboardModifiers() == Qt::ControlModifier)
        {
            id = 10;
        }
        else
        {
            K3PopupMenu popMenu(this);
            popMenu.insertTitle(SmallIcon("digikam"), i18n("Tags"));
            popMenu.insertItem( SmallIcon("tag"), i18n("Assign Tag '%1' to Items", destAlbum->prettyUrl()), 10) ;
            popMenu.insertSeparator(-1);
            popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

            popMenu.setMouseTracking(true);
            id = popMenu.exec(QCursor::pos());
        }

        if (id == 10)
        {
            emit signalProgressBarMode(StatusProgressBar::ProgressBarMode, 
                                       i18n("Assign tag to pictures. Please wait..."));

            int i=0;
            {
                DatabaseTransaction transaction;
                for (Q3ValueList<int>::const_iterator it = imageIDs.begin();
                    it != imageIDs.end(); ++it)
                {
                    // create temporary ImageInfo object
                    ImageInfo info(*it);

                    MetadataHub hub;
                    hub.load(info);
                    hub.setTag(destAlbum, true);
                    hub.write(info, MetadataHub::PartialWrite);
                    hub.write(info.filePath(), MetadataHub::FullWriteIfChanged);

                    emit signalProgressValue((int)((i++/(float)imageIDs.count())*100.0));
                    kapp->processEvents();
                }
            }

            //ImageAttributesWatch::componentData().imagesChanged(destAlbum->id());

            emit signalProgressBarMode(StatusProgressBar::TextMode, QString());
        }
    }
}

}  // NameSpace Digikam
