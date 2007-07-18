/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-05-05
 * Description : tags filter view
 *
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#include <Q3ValueList>
#include <Q3Header>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QCursor>
#include <QDropEvent>
#include <QMouseEvent>

// KDE includes.

#include <k3popupmenu.h>
//#include <kabc/stdaddressbook.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kglobalsettings.h>
#include <kcursor.h>
#include <kmessagebox.h>

// Local includes.

#include "ddebug.h"
#include "albummanager.h"
#include "albumdb.h"
#include "album.h"
#include "albumlister.h"
#include "albumthumbnailloader.h"
#include "databasetransaction.h"
#include "syncjob.h"
#include "dragobjects.h"
#include "folderitem.h"
#include "imageattributeswatch.h"
#include "imageinfo.h"
#include "metadatahub.h"
#include "tagcreatedlg.h"
#include "statusprogressbar.h"
#include "tagfilterview.h"
#include "tagfilterview.moc"

// X11 includes.

extern "C"
{
#include <X11/Xlib.h>
#include <kglobal.h>
}

namespace Digikam
{

class TagFilterViewItem : public FolderCheckListItem
{

public:

    TagFilterViewItem(Q3ListView* parent, TAlbum* tag, bool untagged=false)
        : FolderCheckListItem(parent, tag ? tag->title() : i18n("Not Tagged"),
                              Q3CheckListItem::CheckBox/*Controller*/)
    {
        m_tag      = tag;
        m_untagged = untagged;
        setDragEnabled(!untagged);

        if (tag)
            tag->setExtraData(listView(), this);
    }

    TagFilterViewItem(Q3ListViewItem* parent, TAlbum* tag)
        : FolderCheckListItem(parent, tag->title(), 
                              Q3CheckListItem::CheckBox/*Controller*/)
    {
        m_tag      = tag;
        m_untagged = false;
        setDragEnabled(true);

        if (tag)
            tag->setExtraData(listView(), this);
    }

    virtual void stateChange(bool val)
    {
        Q3CheckListItem::stateChange(val);

/* NOTE G.Caulier 2007/01/08: this code is now disable because TagFilterViewItem 
                              have been changed from QCheckListItem::CheckBoxController 
                              to QCheckListItem::CheckBox.
 
        // All TagFilterViewItems are CheckBoxControllers. If they have no children,
        // they should be of type CheckBox, but that is not possible with our way of adding items.
        // When clicked, children-less items first change to the NoChange state, and a second
        // click is necessary to set them to On and make the filter take effect.
        // So set them to On if the condition is met.
        if (!firstChild() && state() == NoChange)
        {
            setState(On);
        }
*/

        ((TagFilterView*)listView())->stateChanged(this);
    }

    int compare(Q3ListViewItem* i, int column, bool ascending) const
    {
        if (m_untagged)
            return 1;

        TagFilterViewItem* dItem = dynamic_cast<TagFilterViewItem*>(i);
        if (!dItem)
            return 0;

        if (dItem && dItem->m_untagged)
            return -1;

        return Q3ListViewItem::compare(i, column, ascending);
    }

    void paintCell(QPainter* p, const QColorGroup & cg, int column, int width, int align)
    {
        if (!m_untagged)
        {
            FolderCheckListItem::paintCell(p, cg, column, width, align);
            return;
        }

        QFont f(listView()->font());
        f.setBold(true);
        f.setItalic(true);
        p->setFont(f);

        QColorGroup mcg(cg);
        mcg.setColor(QColorGroup::Text, Qt::darkRed);

        FolderCheckListItem::paintCell(p, mcg, column, width, align);
    }

    TAlbum *m_tag;
    bool    m_untagged;
};

// ---------------------------------------------------------------------

class TagFilterViewPrivate
{

public:

    TagFilterViewPrivate()
    {
        dragItem       = 0;
        ABCMenu        = 0;
        timer          = 0;
        toggleAutoTags = TagFilterView::NoToggleAuto;
        matchingCond   = AlbumLister::OrCondition;
    }

    QTimer                         *timer;

    QPoint                          dragStartPos;    

    Q3PopupMenu                     *ABCMenu;

    TagFilterView::ToggleAutoTags   toggleAutoTags;

    AlbumLister::MatchingCondition  matchingCond;
    
    TagFilterViewItem              *dragItem;
};


TagFilterView::TagFilterView(QWidget* parent)
             : FolderView(parent)
{
    d = new TagFilterViewPrivate;
    d->timer = new QTimer(this);

    addColumn(i18n("Tag Filters"));
    setResizeMode(Q3ListView::LastColumn);
    setRootIsDecorated(true);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    TagFilterViewItem* notTaggedItem = new TagFilterViewItem(this, 0, true);
    notTaggedItem->setPixmap(0, AlbumThumbnailLoader::componentData()->getStandardTagIcon());

    // -- setup slots ---------------------------------------------------------

    connect(AlbumManager::componentData(), SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotTagAdded(Album*)));

    connect(AlbumManager::componentData(), SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotTagDeleted(Album*)));

    connect(AlbumManager::componentData(), SIGNAL(signalAlbumRenamed(Album*)),
            this, SLOT(slotTagRenamed(Album*)));

    connect(AlbumManager::componentData(), SIGNAL(signalAlbumsCleared()),
            this, SLOT(slotClear()));

    connect(AlbumManager::componentData(), SIGNAL(signalAlbumIconChanged(Album*)),
            this, SLOT(slotAlbumIconChanged(Album*)));

    connect(AlbumManager::componentData(), SIGNAL(signalTAlbumMoved(TAlbum*, TAlbum*)),
            this, SLOT(slotTagMoved(TAlbum*, TAlbum*)));

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::componentData();

    connect(loader, SIGNAL(signalThumbnail(Album *, const QPixmap&)),
            this, SLOT(slotGotThumbnailFromIcon(Album *, const QPixmap&)));

    connect(loader, SIGNAL(signalFailed(Album *)),
            this, SLOT(slotThumbnailLost(Album *)));

    connect(loader, SIGNAL(signalReloadThumbnails()),
            this, SLOT(slotReloadThumbnails()));

    connect(this, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)),
            this, SLOT(slotContextMenu(Q3ListViewItem*, const QPoint&, int)));

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotTimeOut()));

    // -- read config ---------------------------------------------------------

    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group("Tag Filters View");
    d->matchingCond = (AlbumLister::MatchingCondition)(group.readEntry("Matching Condition", 
                                                       (int)AlbumLister::OrCondition));

    d->toggleAutoTags = (ToggleAutoTags)(group.readEntry("Toggle Auto Tags", (int)NoToggleAuto));
}

TagFilterView::~TagFilterView()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group = config->group("Tag Filters View");
    group.writeEntry("Matching Condition", (int)(d->matchingCond));
    group.writeEntry("Toggle Auto Tags", (int)(d->toggleAutoTags));
    config->sync();

    delete d->timer;
    delete d;
}

void TagFilterView::stateChanged(TagFilterViewItem* item)
{
    ToggleAutoTags oldAutoTags = d->toggleAutoTags;            

    switch(d->toggleAutoTags)
    {
        case Children:
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleChildTags(item, item->isOn());
            d->toggleAutoTags = oldAutoTags;
            break;
        case Parents:
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleParentTags(item, item->isOn());
            d->toggleAutoTags = oldAutoTags;
            break;
        case ChildrenAndParents:
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleChildTags(item, item->isOn());
            toggleParentTags(item, item->isOn());
            d->toggleAutoTags = oldAutoTags;
            break;
        default:
            break;
    }

    triggerChange();
}

void TagFilterView::triggerChange()
{
    d->timer->setSingleShot(true);
    d->timer->start(50);
}

void TagFilterView::contentsMouseMoveEvent(QMouseEvent *e)
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
        TagFilterViewItem *item = dynamic_cast<TagFilterViewItem*>(itemAt(vp));
        if(!item)
        {
            d->dragItem = 0;
            return;
        }
    }    
}

void TagFilterView::contentsMousePressEvent(QMouseEvent *e)
{
    QPoint vp = contentsToViewport(e->pos());
    TagFilterViewItem *item = dynamic_cast<TagFilterViewItem*>(itemAt(vp));

    if(item && e->button() == Qt::RightButton) 
    {
        bool isOn = item->isOn();
        Q3ListView::contentsMousePressEvent(e);
        // Restore the status of checkbox. 
        item->setOn(isOn);
        return;
    }

    Q3ListView::contentsMousePressEvent(e);

    if(item && e->button() == Qt::LeftButton) 
    {
        d->dragStartPos = e->pos();
        d->dragItem     = item;
    }
}

void TagFilterView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    Q3ListView::contentsMouseReleaseEvent(e);

    d->dragItem = 0;
}

Q3DragObject* TagFilterView::dragObject()
{
    TagFilterViewItem *item = dynamic_cast<TagFilterViewItem*>(dragItem());
    if(!item)
        return 0;

    TagDrag *t = new TagDrag(item->m_tag->id(), this);
    t->setPixmap(*item->pixmap(0));

    return t;
}

TagFilterViewItem* TagFilterView::dragItem() const
{
    return d->dragItem;
}

bool TagFilterView::acceptDrop(const QDropEvent *e) const
{
    QPoint vp = contentsToViewport(e->pos());
    TagFilterViewItem *itemDrop = dynamic_cast<TagFilterViewItem*>(itemAt(vp));
    TagFilterViewItem *itemDrag = dynamic_cast<TagFilterViewItem*>(dragItem());

    if(TagDrag::canDecode(e) || TagListDrag::canDecode(e))
    {
        // Allow dragging at the root, to move the tag to the root
        if(!itemDrop)
            return true;

        // Do not allow dragging at the "Not Tagged" item.
        if (itemDrop->m_untagged)
            return false;

        // Dragging an item on itself makes no sense
        if(itemDrag == itemDrop)
            return false;

        // Dragging a parent on its child makes no sense
        if(itemDrag && itemDrag->m_tag->isAncestorOf(itemDrop->m_tag))
            return false;

        return true;
    }

    if (ItemDrag::canDecode(e) && itemDrop && !itemDrop->m_untagged)
    {
        TAlbum *tag = itemDrop->m_tag;
        
        if (tag)
        {
            if (tag->parent())
            {         
                // Only other possibility is image items being dropped
                // And allow this only if there is a Tag to be dropped
                // on and also the Tag is not root or "Not Tagged" item.
                return true;
            }
        }
    }

    return false;
}

void TagFilterView::contentsDropEvent(QDropEvent *e)
{
    FolderView::contentsDropEvent(e);

    if (!acceptDrop(e))
        return;

    QPoint vp = contentsToViewport(e->pos());
    TagFilterViewItem *itemDrop = dynamic_cast<TagFilterViewItem*>(itemAt(vp));

    if (!itemDrop || itemDrop->m_untagged)
        return;

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
        
        if (talbum == itemDrop->m_tag)
            return;

        K3PopupMenu popMenu(this);
        popMenu.insertTitle(SmallIcon("digikam"), i18n("Tag Filters"));
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
                newParentTag = itemDrop->m_tag;
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
        TAlbum *destAlbum = itemDrop->m_tag;

        KUrl::List      urls;
        KUrl::List      kioURLs;
        Q3ValueList<int> albumIDs;
        Q3ValueList<int> imageIDs;

        if (!ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs))
            return;

        if (urls.isEmpty() || kioURLs.isEmpty() || albumIDs.isEmpty() || imageIDs.isEmpty())
            return;

        int id = 0;
        char keys_return[32];
        XQueryKeymap(x11Info().display(), keys_return);
        int key_1 = XKeysymToKeycode(x11Info().display(), 0xFFE3);
        int key_2 = XKeysymToKeycode(x11Info().display(), 0xFFE4);

        // If a ctrl key is pressed while dropping the drag object,
        // the tag is assigned to the images without showing a
        // popup menu.
        if (((keys_return[key_1 / 8]) && (1 << (key_1 % 8))) ||
            ((keys_return[key_2 / 8]) && (1 << (key_2 % 8))))
        {
            id = 10;
        }
        else
        {
            K3PopupMenu popMenu(this);
            popMenu.insertTitle(SmallIcon("digikam"), i18n("Tag Filters"));
            popMenu.insertItem(SmallIcon("tag"), i18n("Assign Tag '%1' to Items")
                               .arg(destAlbum->prettyUrl()), 10) ;
            popMenu.insertItem(i18n("Set as Tag Thumbnail"),  11);
            popMenu.insertSeparator(-1);
            popMenu.insertItem(SmallIcon("cancel"), i18n("C&ancel"));

            popMenu.setMouseTracking(true);
            id = popMenu.exec(QCursor::pos());
        }

        if (id == 10)
        {
            emit signalProgressBarMode(StatusProgressBar::ProgressBarMode, 
                                       i18n("Assigning image tags. Please wait..."));

            {
                DatabaseTransaction transaction;
                int i=0;
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

            emit signalProgressBarMode(StatusProgressBar::TextMode, QString());
        }
        else if(id == 11)
        {
            QString errMsg;
            AlbumManager::componentData()->updateTAlbumIcon(destAlbum, QString(),
                                                       imageIDs.first(), errMsg);
        }
    }
}

void TagFilterView::slotTagAdded(Album* album)
{
    if (!album || album->isRoot())
        return;

    TAlbum* tag = dynamic_cast<TAlbum*>(album);
    if (!tag)
        return;

    if (tag->parent()->isRoot())
    {
        new TagFilterViewItem(this, tag);
    }
    else
    {
        TagFilterViewItem* parent = (TagFilterViewItem*)(tag->parent()->extraData(this));
        if (!parent)
        {
            DWarning() << k_funcinfo << " Failed to find parent for Tag "
                       << tag->tagPath() << endl;
            return;
        }

        new TagFilterViewItem(parent, tag);
    }

    setTagThumbnail(tag);
}

void TagFilterView::slotTagRenamed(Album* album)
{
    if (!album)
        return;

    TAlbum* tag = dynamic_cast<TAlbum*>(album);
    if (!tag)
        return;

    TagFilterViewItem* item = (TagFilterViewItem*)(tag->extraData(this));
    if (item)
    {
        item->setText(0, tag->title());
    }
}

void TagFilterView::slotTagMoved(TAlbum* tag, TAlbum* newParent)
{
    if (!tag || !newParent)
        return;

    TagFilterViewItem* item = (TagFilterViewItem*)(tag->extraData(this));
    if (!item)
        return;

    if (item->parent())
    {
        Q3ListViewItem* oldPItem = item->parent();
        oldPItem->takeItem(item);

        TagFilterViewItem* newPItem = (TagFilterViewItem*)(newParent->extraData(this));
        if (newPItem)
            newPItem->insertItem(item);
        else
            insertItem(item);
    }
    else
    {
        takeItem(item);

        TagFilterViewItem* newPItem = (TagFilterViewItem*)(newParent->extraData(this));

        if (newPItem)
            newPItem->insertItem(item);
        else
            insertItem(item);
    }
}

void TagFilterView::slotTagDeleted(Album* album)
{
    if (!album || album->isRoot())
        return;

    TAlbum* tag = dynamic_cast<TAlbum*>(album);
    if (!tag)
        return;

    TagFilterViewItem* item = (TagFilterViewItem*)(album->extraData(this));
    if (!item)
        return;

    album->removeExtraData(this);
    delete item;
}

void TagFilterView::setTagThumbnail(TAlbum *album)
{
    if(!album)
        return;

    TagFilterViewItem* item = (TagFilterViewItem*) album->extraData(this);

    if(!item)
        return;

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::componentData();
    QPixmap icon;
    if (!loader->getTagThumbnail(album, icon))
    {
        if (icon.isNull())
        {
            item->setPixmap(0, loader->getStandardTagIcon(album));
        }
        else
        {
            QPixmap blendedIcon = loader->blendIcons(loader->getStandardTagIcon(), icon);
            item->setPixmap(0, blendedIcon);
        }
    }
    else
    {
        // for the time being, set standard icon
        item->setPixmap(0, loader->getStandardTagIcon(album));
    }
}

void TagFilterView::slotGotThumbnailFromIcon(Album *album, const QPixmap& thumbnail)
{
    if(!album || album->type() != Album::TAG)
        return;

    TagFilterViewItem* item = (TagFilterViewItem*)album->extraData(this);

    if(!item)
        return;

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::componentData();
    QPixmap blendedIcon = loader->blendIcons(loader->getStandardTagIcon(), thumbnail);
    item->setPixmap(0, blendedIcon);
}

void TagFilterView::slotThumbnailLost(Album *)
{
    // we already set the standard icon before loading
}

void TagFilterView::slotReloadThumbnails()
{
    AlbumList tList = AlbumManager::componentData()->allTAlbums();
    for (AlbumList::iterator it = tList.begin(); it != tList.end(); ++it)
    {
        TAlbum* tag  = (TAlbum*)(*it);
        setTagThumbnail(tag);
    }
}

void TagFilterView::slotAlbumIconChanged(Album* album)
{
    if(!album || album->type() != Album::TAG)
        return;

    setTagThumbnail((TAlbum *)album);
}

void TagFilterView::slotClear()
{
    clear();

    TagFilterViewItem* notTaggedItem = new TagFilterViewItem(this, 0, true);
    notTaggedItem->setPixmap(0, AlbumThumbnailLoader::componentData()->getStandardTagIcon());
}

void TagFilterView::slotTimeOut()
{
    Q3ValueList<int> filterTags;

    bool showUnTagged = false;

    Q3ListViewItemIterator it(this, Q3ListViewItemIterator::Checked);
    while (it.current())
    {
        TagFilterViewItem* item = (TagFilterViewItem*)it.current();
        if (item->m_tag)
            filterTags.append(item->m_tag->id());
        else if (item->m_untagged)
            showUnTagged = true;
        ++it;
    }

    AlbumLister::componentData()->setTagFilter(filterTags, d->matchingCond, showUnTagged);
}

void TagFilterView::slotContextMenu(Q3ListViewItem* it, const QPoint&, int)
{
    TagFilterViewItem *item = dynamic_cast<TagFilterViewItem*>(it);
    if (item && item->m_untagged)
        return;

    d->ABCMenu = new Q3PopupMenu;
    
    connect(d->ABCMenu, SIGNAL( aboutToShow() ),
            this, SLOT( slotABCContextMenu() ));

    K3PopupMenu popmenu(this);
    popmenu.insertTitle(SmallIcon("digikam"), i18n("Tag Filters"));
    popmenu.insertItem(SmallIcon("tag-new"), i18n("New Tag..."), 10);
    popmenu.insertItem(SmallIcon("tag-addressbook"), i18n("Create Tag From AddressBook"), d->ABCMenu);

    if (item)
    {
        popmenu.insertItem(SmallIcon("tag-properties"), i18n("Edit Tag Properties..."), 11);
        popmenu.insertItem(SmallIcon("tag-reset"),      i18n("Reset Tag Icon"),         13);
        popmenu.insertSeparator(-1);
        popmenu.insertItem(SmallIcon("tag-delete"),     i18n("Delete Tag"),             12);
    }
 
    popmenu.insertSeparator(-1);

    Q3PopupMenu selectTagsMenu;
    selectTagsMenu.insertItem(i18n("All Tags"),   14);
    if (item)
    {
        selectTagsMenu.insertSeparator(-1);
        selectTagsMenu.insertItem(i18n("Children"),     17);
        selectTagsMenu.insertItem(i18n("Parents"),    19);
    }
    popmenu.insertItem(i18n("Select"), &selectTagsMenu);

    Q3PopupMenu deselectTagsMenu;
    deselectTagsMenu.insertItem(i18n("All Tags"), 15);
    if (item)
    {
        deselectTagsMenu.insertSeparator(-1);
        deselectTagsMenu.insertItem(i18n("Children"),   18);
        deselectTagsMenu.insertItem(i18n("Parents"),  20);
    }
    popmenu.insertItem(i18n("Deselect"), &deselectTagsMenu);

    popmenu.insertItem(i18n("Invert Selection"),  16);
    popmenu.insertSeparator(-1);

    Q3PopupMenu toggleAutoMenu;
    toggleAutoMenu.setCheckable(true);
    toggleAutoMenu.insertItem(i18n("None"),    21);
    toggleAutoMenu.insertSeparator(-1);
    toggleAutoMenu.insertItem(i18n("Children"),  22);
    toggleAutoMenu.insertItem(i18n("Parents"), 23);
    toggleAutoMenu.insertItem(i18n("Both"),    24);
    toggleAutoMenu.setItemChecked(21 + d->toggleAutoTags, true);
    popmenu.insertItem(i18n("Toogle Auto"), &toggleAutoMenu);

    Q3PopupMenu matchingCongMenu;
    matchingCongMenu.setCheckable(true);
    matchingCongMenu.insertItem(i18n("Or Between Tags"),  25);
    matchingCongMenu.insertItem(i18n("And Between Tags"), 26);
    matchingCongMenu.setItemChecked((d->matchingCond == AlbumLister::OrCondition) ? 25 : 26, true);
    popmenu.insertItem(i18n("Matching Condition"), &matchingCongMenu);

    ToggleAutoTags oldAutoTags = d->toggleAutoTags;            

    int choice = popmenu.exec((QCursor::pos()));
    switch( choice )
    {
        case 10:    // New Tag.
        {
            tagNew(item);
            break;
        }
        case 11:    // Edit Tag Properties.
        {
            tagEdit(item);
            break;
        }
        case 12:    // Delete Tag.
        {
            tagDelete(item);
            break;
        }
        case 13:    // Reset Tag Icon.
        {
            QString errMsg;
            AlbumManager::componentData()->updateTAlbumIcon(item->m_tag, QString("tag"), 0, errMsg);
            break;
        }        
        case 14:    // Select All Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            Q3ListViewItemIterator it(this, Q3ListViewItemIterator::NotChecked);
            while (it.current())
            {
                TagFilterViewItem* item = (TagFilterViewItem*)it.current();

                // Ignore "Not Tagged" tag filter.
                if (!item->m_untagged)
                    item->setOn(true);

                ++it;
            }
            triggerChange();
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 15:    // Deselect All Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            Q3ListViewItemIterator it(this, Q3ListViewItemIterator::Checked);
            while (it.current())
            {
                TagFilterViewItem* item = (TagFilterViewItem*)it.current();

                // Ignore "Not Tagged" tag filter.
                if (!item->m_untagged)
                    item->setOn(false);

                ++it;
            }
            triggerChange();
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 16:       // Invert All Tags Selection.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            Q3ListViewItemIterator it(this);
            while (it.current())
            {
                TagFilterViewItem* item = (TagFilterViewItem*)it.current();

                // Ignore "Not Tagged" tag filter.
                if (!item->m_untagged)
                    item->setOn(!item->isOn());

                ++it;
            }
            triggerChange();
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 17:   // Select Child Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleChildTags(item, true);
            TagFilterViewItem *tItem = (TagFilterViewItem*)item->m_tag->extraData(this);
            tItem->setOn(true);            
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 18:   // Deselect Child Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleChildTags(item, false);
            TagFilterViewItem *tItem = (TagFilterViewItem*)item->m_tag->extraData(this);
            tItem->setOn(false);            
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 19:   // Select Parent Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleParentTags(item, true);
            TagFilterViewItem *tItem = (TagFilterViewItem*)item->m_tag->extraData(this);
            tItem->setOn(true);            
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 20:   // Deselect Parent Tags.
        {
            d->toggleAutoTags = TagFilterView::NoToggleAuto;
            toggleParentTags(item, false);
            TagFilterViewItem *tItem = (TagFilterViewItem*)item->m_tag->extraData(this);
            tItem->setOn(false);            
            d->toggleAutoTags = oldAutoTags;
            break;
        }
        case 21:   // No toggle auto tags.
        {
            d->toggleAutoTags = NoToggleAuto;
            break;
        }
        case 22:   // Toggle auto Children tags.
        {
            d->toggleAutoTags = Children;
            break;
        }
        case 23:   // Toggle auto Parents tags.
        {
            d->toggleAutoTags = Parents;
            break;
        }
        case 24:   // Toggle auto Children and Parents tags.
        {
            d->toggleAutoTags = ChildrenAndParents;
            break;
        }
        case 25:    // Or Between Tags.
        {
            d->matchingCond = AlbumLister::OrCondition;
            triggerChange();
            break;
        }
        case 26:    // And Between Tags.
        {
            d->matchingCond = AlbumLister::AndCondition;
            triggerChange();
            break;
        }
        default:
            break;
    }

    if ( choice > 100 )
    {
        tagNew(item, d->ABCMenu->text( choice ), "tag-people" );
    }

    delete d->ABCMenu;
    d->ABCMenu = 0;
}

void TagFilterView::slotABCContextMenu()
{
/*    d->ABCMenu->clear();

    int counter = 100;
    KABC::AddressBook* ab = KABC::StdAddressBook::self();
    QStringList names;
    for ( KABC::AddressBook::Iterator it = ab->begin(); it != ab->end(); ++it )
    {
        names.push_back(it->formattedName());
    }

    qSort(names);

    for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
    {
        QString name = *it;
        if ( !name.isNull() )
            d->ABCMenu->insertItem( name, ++counter );
    }

    if (counter == 100)
    {
        d->ABCMenu->insertItem( i18n("No AddressBook entries found"), ++counter );
        d->ABCMenu->setItemEnabled( counter, false );
    }*/
}

void TagFilterView::tagNew(TagFilterViewItem* item, const QString& _title, const QString& _icon)
{
    TAlbum  *parent;
    QString  title    = _title;
    QString  icon     = _icon;
    AlbumManager *man = AlbumManager::componentData();

    if (!item)
        parent = man->findTAlbum(0);
    else
        parent = item->m_tag;

    if (title.isNull())
    {
        if (!TagCreateDlg::tagCreate(kapp->activeWindow(), parent, title, icon))
            return;
    }

    QString errMsg;
    TAlbum* newAlbum = man->createTAlbum(parent, title, icon, errMsg);

    if( !newAlbum )
    {
        KMessageBox::error(0, errMsg);
    }
    else
    {
        TagFilterViewItem *item = (TagFilterViewItem*)newAlbum->extraData(this);
        if ( item )
        {
            clearSelection();
            setSelected(item, true);
            setCurrentItem(item);
            ensureItemVisible( item );
        }
    }
}

void TagFilterView::tagEdit(TagFilterViewItem* item)
{
    if (!item)
        return;

    TAlbum *tag = item->m_tag;
    if (!tag)
        return;

    QString title, icon;
    if (!TagEditDlg::tagEdit(kapp->activeWindow(), tag, title, icon))
    {
        return;
    }

    AlbumManager* man = AlbumManager::componentData();

    if (tag->title() != title)
    {
        QString errMsg;
        if(!man->renameTAlbum(tag, title, errMsg))
            KMessageBox::error(0, errMsg);
        else
            item->setText(0, title);
    }

    if (tag->icon() != icon)
    {
        QString errMsg;
        if (!man->updateTAlbumIcon(tag, icon, 0, errMsg))
            KMessageBox::error(0, errMsg);
        else
            setTagThumbnail(tag);
    }
}

void TagFilterView::tagDelete(TagFilterViewItem* item)
{
    if (!item)
        return;

    TAlbum *tag = item->m_tag;
    if (!tag || tag->isRoot())
        return;

    // find number of subtags
    int children = 0;
    AlbumIterator iter(tag);
    while(iter.current())
    {
        children++;
        ++iter;
    }

    AlbumManager* man = AlbumManager::componentData();

    if (children)
    {
        int result = KMessageBox::warningContinueCancel(this,
                     i18np("Tag '%1' has one subtag. "
                           "Deleting this will also delete "
                           "the subtag. "
                           "Do you want to continue?",
                           "Tag '%1' has %n subtags. "
                           "Deleting this will also delete "
                           "the subtags. "
                           "Do you want to continue?",
                           children, tag->title()),
                     i18n("Delete Tag"),
                     KGuiItem(i18n("Delete"),
                     "editdelete"));

        if(result == KMessageBox::Continue)
        {
            QString errMsg;
            if (!man->deleteTAlbum(tag, errMsg))
                KMessageBox::error(0, errMsg);
        }
    }
    else
    {
        int result = KMessageBox::warningContinueCancel(0, i18n("Delete '%1' tag?")
                                                        .arg(tag->title()),i18n("Delete Tag"),
                                                        KGuiItem(i18n("Delete"), "editdelete"));

        if (result == KMessageBox::Continue)
        {
            QString errMsg;
            if (!man->deleteTAlbum(tag, errMsg))
                KMessageBox::error(0, errMsg);
        }
    }
}

void TagFilterView::toggleChildTags(TagFilterViewItem* tItem, bool b)
{
    if (!tItem)
        return;

    TAlbum *album = tItem->m_tag; 
    if (!album)
        return;

    AlbumIterator it(album);
    while ( it.current() )
    {
        TAlbum *ta              = (TAlbum*)it.current();
        TagFilterViewItem *item = (TagFilterViewItem*)ta->extraData(this);
        if (item)
            if (item->isVisible())
                item->setOn(b);
        ++it;
    }
}

void TagFilterView::toggleParentTags(TagFilterViewItem* tItem, bool b)
{
    if (!tItem)
        return;

    TAlbum *album = tItem->m_tag; 
    if (!album)
        return;

    Q3ListViewItemIterator it(this);
    while (it.current())
    {
        TagFilterViewItem* item = dynamic_cast<TagFilterViewItem*>(it.current());
        if (item->isVisible())
        {
            Album *a = dynamic_cast<Album*>(item->m_tag);
            if (a)
            {
                if (a == album->parent())
                {
                    item->setOn(b);
                    toggleParentTags(item, b);
                }
            }
        }
        ++it;
    }
}

}  // namespace Digikam

