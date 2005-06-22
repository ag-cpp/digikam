//////////////////////////////////////////////////////////////////////////////
//
//    ALBUMFOLDERVIEW.CPP
//
//    Copyright (C) 2003-2004 Renchi Raju <renchi at pooh.tam.uiuc.edu>
//                            Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////



// Qt includes.

#include <qstring.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qptrlist.h>
#include <qevent.h>
#include <qpoint.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qtimer.h>
#include <qpainter.h>

// KDE includes.

#include <kdebug.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kglobal.h>
#include <kurldrag.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kpropsdlg.h>
#include <kfiledialog.h>
#include <kaction.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kglobalsettings.h>
#include <kcursor.h>

#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kcalendarsystem.h>
#endif

// Local includes.

#include "albummanager.h"
#include "album.h"
#include "albumdb.h"
#include "thumbnailjob.h"
#include "dio.h"
#include "digikamapp.h"
#include "syncjob.h"

#include "thumbnailsize.h"
#include "albumfolderitem_deprecated.h"
#include "albumfolderview_deprecated.h"
#include "albumpropsedit.h"
#include "tagcreatedlg.h"
#include "albumsettings.h"
#include "themeengine.h"

#include "cameratype.h"
#include "cameradragobject.h"
#include "dragobjects.h"

extern "C"
{
#include <X11/Xlib.h>
}

AlbumFolderView_Deprecated::AlbumFolderView_Deprecated(QWidget *parent)
               : ListView(parent)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    viewport()->setMouseTracking(true);
    
    dropTarget_     = 0;
    albumSortOrder_ = (int) AlbumSettings::ByFolder;
    groupItems_.setAutoDelete(false);
    groupItems_.clear();
    phyRootItem_ = 0;
    tagRootItem_ = 0;
    openAlbumTimer_ = new QTimer(this);
    stateInitialLoading_ = true;

    connect(this, SIGNAL(signalSelectionChanged(ListItem*)),
            SLOT(slotSelectionChanged(ListItem*)));
    connect(this, SIGNAL(signalDoubleClicked(ListItem*)),
            SLOT(slotDoubleClicked(ListItem*)));
    connect(this, SIGNAL(signalRightButtonPressed(ListItem*)),
            SLOT(slotRightButtonClicked(ListItem*)));

    albumMan_ = AlbumManager::instance();

    connect(albumMan_, SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotAlbumAdded(Album*)));
    connect(albumMan_, SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));
    connect(albumMan_, SIGNAL(signalAlbumsCleared()),
            this, SLOT(slotAlbumsCleared()));
//    connect(albumMan_, SIGNAL(signalAllAlbumsLoaded()),
//            this, SLOT(slotAllAlbumsLoaded()));
    connect(albumMan_, SIGNAL(signalAlbumRenamed(Album*)),
            this, SLOT(slotAlbumRenamed(Album*)));
    connect(albumMan_, SIGNAL(signalAlbumIconChanged(Album*)),
            this, SLOT(slotAlbumIconChanged(Album*)));

    connect(openAlbumTimer_, SIGNAL(timeout()),
            this, SLOT(slotOpenAlbumFolderItem_Deprecated()));

    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            SLOT(slotThemeChanged()));

    loadAlbumState();
}

AlbumFolderView_Deprecated::~AlbumFolderView_Deprecated()
{
    saveAlbumState();

    if (!iconThumbJob_.isNull())
        iconThumbJob_->kill();
}

void AlbumFolderView_Deprecated::applySettings()
{
    AlbumSettings* settings = AlbumSettings::instance();
    if (!settings)
        return;

    if (albumSortOrder_ != settings->getAlbumSortOrder())
    {
        albumSortOrder_ = settings->getAlbumSortOrder();
        resort();
    }
}

void AlbumFolderView_Deprecated::resort()
{
    AlbumFolderItem_Deprecated* prevSelectedItem =
        dynamic_cast<AlbumFolderItem_Deprecated*>(getSelected());
    if (prevSelectedItem && prevSelectedItem->isGroupItem())
        prevSelectedItem = 0;


    AlbumList pList(AlbumManager::instance()->allPAlbums());
    for (AlbumList::iterator it = pList.begin(); it != pList.end(); ++it)
    {
        PAlbum *album = (PAlbum*)(*it);
        if (!album->isRoot() && album->extraData(this))
        {
            reparentItem(static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this)));
        }
    }

    AlbumList tList(AlbumManager::instance()->allTAlbums());
    for (AlbumList::iterator it = tList.begin(); it != tList.end(); ++it)
    {
        TAlbum *album = (TAlbum*)(*it);
        if (!album->isRoot() && album->extraData(this))
        {
            reparentItem(static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this)));
        }
    }

    // Clear any groupitems which have been left empty
    clearEmptyGroupItems();

    if (prevSelectedItem)
    {
        ensureItemVisible(prevSelectedItem);
        setSelected(prevSelectedItem);
    }
}

void AlbumFolderView_Deprecated::reparentItem(AlbumFolderItem_Deprecated* folderItem)
{
    if (!folderItem)
        return;

    Album* album = folderItem->album();
    if (!album || album->isRoot())
        return;

    ListItem *oldParent = folderItem->parent();
    ListItem* newParent = findParent(album);

    if (!oldParent) {
        // FolderItem was a top-level item -  Don't touch
        return;
    }

    // Has a parent
    oldParent->removeChild(folderItem);

    // insert into new parent
    newParent->insertChild(folderItem);
}

AlbumFolderItem_Deprecated* AlbumFolderView_Deprecated::findParent(Album *album)
{
    if (!album || album->isRoot())
        return 0;

    if (album->type() == Album::PHYSICAL)
    {
        switch (albumSortOrder_)
        {
        case(AlbumSettings::ByFolder):
        {
            return findParentByFolder(album);
        }
        case(AlbumSettings::ByCollection):
        {
            return findParentByCollection(dynamic_cast<PAlbum*>(album));
        }
        case(AlbumSettings::ByDate):
        {
            return findParentByDate(dynamic_cast<PAlbum*>(album));
        }
        }

        return 0;
    }
    else if (album->type() == Album::TAG)
    {
        return findParentByFolder(album);
    }

    return 0;
}

AlbumFolderItem_Deprecated* AlbumFolderView_Deprecated::findParentByFolder(Album *album)
{
    if (!album)
        return 0;

    Album* parent = album->parent();

    if (parent)
    {
        AlbumFolderItem_Deprecated *pItem = (AlbumFolderItem_Deprecated*)parent->extraData(this);
        return pItem;
    }

    return 0;
}

AlbumFolderItem_Deprecated* AlbumFolderView_Deprecated::findParentByDate(PAlbum* album)
{
    if (!album)
        return 0;

    QDate date = album->date();

    QString timeString = QString::number(date.year()) + ", " +
#if KDE_IS_VERSION(3,2,0)
                         KGlobal::locale()->calendar()->monthName(date, false);
#else
                         KGlobal::locale()->monthName(date.month(), false);
#endif

    AlbumFolderItem_Deprecated* parentItem = 0;

    for (AlbumFolderItem_Deprecated* groupItem = groupItems_.first();
         groupItem; groupItem = groupItems_.next())
    {
        if (groupItem->text() == timeString)
        {
            parentItem = groupItem;
            break;
        }
    }

    // Need to create a new parent item
    if (!parentItem)
    {
        parentItem = new AlbumFolderItem_Deprecated(phyRootItem_, timeString,
                                         date.year(), date.month());
        parentItem->setOpen(false);
        groupItems_.append(parentItem);
    }

    return parentItem;
}


AlbumFolderItem_Deprecated* AlbumFolderView_Deprecated::findParentByCollection(PAlbum* album)
{
    if (!album)
        return 0;

    AlbumSettings* settings = AlbumSettings::instance();
    if (!settings) return 0;

    QStringList collectionList = settings->getAlbumCollectionNames();
    QString collection = album->collection();

    if (collection.isEmpty() || !collectionList.contains(collection))
        collection = i18n("Uncategorized Albums");

    AlbumFolderItem_Deprecated* parentItem = 0;

    for (AlbumFolderItem_Deprecated* groupItem = groupItems_.first();
         groupItem; groupItem = groupItems_.next()) {
        if (groupItem->text() == collection) {
            parentItem = groupItem;
            break;
        }
    }

    // Need to create a new parent item
    if (!parentItem) {
        parentItem = new AlbumFolderItem_Deprecated(phyRootItem_, collection,
                                         0, 0);
        parentItem->setOpen(false);
        groupItems_.append(parentItem);
    }

    return parentItem;
}

void AlbumFolderView_Deprecated::clearEmptyGroupItems()
{
    AlbumFolderItem_Deprecated* groupItem = groupItems_.first();
    while (groupItem) {
        AlbumFolderItem_Deprecated* nextGroupItem = groupItems_.next();
        if (groupItem->childCount() == 0) {
            groupItems_.remove(groupItem);
            delete groupItem;
        }
        groupItem = nextGroupItem;
    }
}


QPixmap AlbumFolderView_Deprecated::getBlendedIcon(TAlbum* album) const
{
    KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();

    QPixmap baseIcon(iconLoader->loadIcon("tag",
                                          KIcon::NoGroup,
                                          32,
                                          KIcon::DefaultState,
                                          0, true));

    if(!album)
        return baseIcon;

    QString icon(album->icon());

    QPixmap pix = SyncJob::getTagThumbnail(album->icon(), 20);

    if (!pix.isNull())
    {
        QPainter p(&baseIcon);
        p.drawPixmap(6, 9, pix, 0, 0, -1, -1);
        p.end();
    }

    return baseIcon;
}

void AlbumFolderView_Deprecated::slotAlbumAdded(Album *album)
{
    if (!album || (album->type() != Album::PHYSICAL && album->type() != Album::TAG))
        return;

    if (album->isRoot()) {

        KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();

        switch (album->type()) {
        case(Album::PHYSICAL): {
            phyRootItem_ = new AlbumFolderItem_Deprecated(this, album);
            phyRootItem_->setPixmap(iconLoader->loadIcon("folder_image",
                                                         KIcon::NoGroup,
                                                         32,
                                                         KIcon::DefaultState,
                                                         0, true));
            phyRootItem_->setOpen(true);
            album->setExtraData(this, phyRootItem_);
            break;
        }
        case(Album::TAG): {
            tagRootItem_= new AlbumFolderItem_Deprecated(this, album);
            tagRootItem_->setPixmap(iconLoader->loadIcon("tag-folder",
                                                         KIcon::NoGroup,
                                                         32,
                                                         KIcon::DefaultState,
                                                         0, true));

            tagRootItem_->setOpen(true);
            album->setExtraData(this, tagRootItem_);
            break;
        }
        default: {
            kdWarning() << k_funcinfo << "Unknown album type: "
                        << album->type() << endl;
            return;
        }
        }

    }
    else {

        AlbumFolderItem_Deprecated *pItem = findParent(album);
        if (!pItem) {
            kdWarning() << k_funcinfo << "No parent viewitem for album: "
                        << album->title() << endl;
            return;
        }

        // restore album's parent state based on last run
        if (stateInitialLoading_)
        {
            int fakeID = (album->type() == Album::PHYSICAL) ? 100000 : 200000;
            fakeID += album->id();
            pItem->setOpen(stateAlbumOpen_.contains(fakeID) &&
                           stateAlbumOpen_[fakeID]);
        }

        KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();

        AlbumFolderItem_Deprecated* folderItem = new AlbumFolderItem_Deprecated(pItem, album);
        if (album->type() == Album::PHYSICAL)
        {
            folderItem->setPixmap(iconLoader->loadIcon("folder",
                                                       KIcon::NoGroup,
                                                       32,
                                                       KIcon::DefaultState,
                                                       0, true));
            PAlbum* p = dynamic_cast<PAlbum*>(album);
            p->setExtraData(this, folderItem);

            folderItem->highlighted_ = false;
            albumHighlight(p);
        }
        else if (album->type() == Album::TAG)
        {
            TAlbum* t = dynamic_cast<TAlbum*>(album);
            folderItem->setPixmap(getBlendedIcon(t));
            album->setExtraData(this, folderItem);
        }
    }
}

void AlbumFolderView_Deprecated::slotAlbumDeleted(Album *album)
{
    if (!album || !album->extraData(this))
        return;

    if (album->type() == Album::PHYSICAL)
    {
        PAlbum* p = dynamic_cast<PAlbum*>(album);
        if (!p->icon().isEmpty() && !iconThumbJob_.isNull())
            iconThumbJob_->removeItem(p->icon());
    }

    AlbumFolderItem_Deprecated* folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
    delete folderItem;

    clearEmptyGroupItems();
}

void AlbumFolderView_Deprecated::slotAlbumsCleared()
{
    groupItems_.clear();
    clear();

    phyRootItem_ = 0;
    tagRootItem_ = 0;
}

void AlbumFolderView_Deprecated::slotAllAlbumsLoaded()
{
    stateInitialLoading_ = false;
    /*
    AlbumFolderItem_Deprecated* folderItem = 0;

    if (stateAlbumSel_ >= 100000 &&
        stateAlbumSel_ <  200000)
    {
        // last selected album was a PAlbum
        int id = stateAlbumSel_ - 100000;
        PAlbum* album = albumMan_->findPAlbum(id);
        if (album && album->extraData(this))
        {
            folderItem = static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
        }
    }
    else if (stateAlbumSel_ >= 200000)
    {
        // last selected album was a TAlbum
        int id = stateAlbumSel_ - 200000;
        TAlbum* album = albumMan_->findTAlbum(id);
        if (album && album->extraData(this))
        {
            folderItem = static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
        }
    }

    if (!folderItem)
        return;

    setSelected(folderItem);
    ensureItemVisible(folderItem);

    disconnect(albumMan_, SIGNAL(signalAllAlbumsLoaded()),
    this, SLOT(slotAllAlbumsLoaded()));*/
}

void AlbumFolderView_Deprecated::albumNew()
{
    PAlbum* album = 0;

    if (getSelected())
    {
        AlbumFolderItem_Deprecated *folderItem =
            dynamic_cast<AlbumFolderItem_Deprecated*>(getSelected());
        Album *a = folderItem->album();
        if (a && a->type() == Album::PHYSICAL)
        {
            album = dynamic_cast<PAlbum*>(a);
        }
    }

    if (!album)
        album = dynamic_cast<PAlbum*>(phyRootItem_->album());

    albumNew(album);
}

void AlbumFolderView_Deprecated::albumNew(PAlbum* parent)
{
    AlbumSettings* settings = AlbumSettings::instance();

    if (!settings)
    {
        kdWarning() << "AlbumFolderView_Deprecated: Couldn't get Album Settings" << endl;
        return;
    }

    QDir libraryDir(settings->getAlbumLibraryPath());

    if (!libraryDir.exists())
    {
        KMessageBox::error(0,
                           i18n("The Albums Library has not been set correctly.\n"
                                "Select \"Configure Digikam\" from the Settings "
                                "menu and choose a folder to use for the Albums "
                                "Library."));
        return;
    }

    QString     title;
    QString     comments;
    QString     collection;
    QDate       date;
    QStringList albumCollections;
        
    if (!AlbumPropsEdit::createNew(parent, title, comments, date, collection,
                                   albumCollections))
        return;

    QStringList oldAlbumCollections(AlbumSettings::instance()->getAlbumCollectionNames());
    if (albumCollections != oldAlbumCollections)
    {
        AlbumSettings::instance()->setAlbumCollectionNames(albumCollections);
        resort();
    }
    
    QString errMsg;
    if (albumMan_->createPAlbum(parent, title, comments, date,
                                collection, errMsg))
    {
        connect(albumMan_, SIGNAL(signalAlbumAdded(Album*)),
                this, SLOT(slotNewAlbumCreated(Album*)));
    }
    else
    {
        KMessageBox::error(0, errMsg);
    }
}


void AlbumFolderView_Deprecated::slotNewAlbumCreated(Album* album)
{
    disconnect(albumMan_, SIGNAL(signalAlbumAdded(Album*)),
               this, SLOT(slotNewAlbumCreated(Album*)));

    if (!album || !album->extraData(this) ||
        album->type() != Album::PHYSICAL) {
        return;
    }
    
    PAlbum* pa = dynamic_cast<PAlbum*>(album);
    AlbumFolderItem_Deprecated* folderItem =
         static_cast<AlbumFolderItem_Deprecated*>(pa->extraData(this));
    
    ensureItemVisible(folderItem);
    setSelected(folderItem);
}

void AlbumFolderView_Deprecated::albumDelete()
{
    Album *album = albumMan_->currentAlbum();
    if (!album)
        return;

    if (album->isRoot())
        return;

    if (album->type() == Album::PHYSICAL)
        albumDelete(dynamic_cast<PAlbum*>(album));
}

void AlbumFolderView_Deprecated::albumDelete(PAlbum* album)
{
    if (!album || album->isRoot())
        return;

    // find number of subalbums

    int children = 0;
    AlbumIterator it(album);
    while ( it.current() )
    {
        children++;
        ++it;
    }

    int result = KMessageBox::No;
    
    AlbumSettings* settings = AlbumSettings::instance();
    if (children)
    {
        result =
            KMessageBox::warningYesNo(this, settings->getUseTrash() ?
                                      i18n("Album '%1' has %2 subalbums. "
                                           "Moving this to trash will also "
                                           "move the subalbums to trash. "
                                           "Are you sure you want to continue?")
                                      .arg(album->title())
                                      .arg(children) :
                                      i18n("Album '%1' has %2 subalbums. "
                                           "Deleting this will also delete "
                                           "the subalbums. "
                                           "Are you sure you want to continue?")
                                      .arg(album->title())
                                      .arg(children));
    }
    else
    {
        result = KMessageBox::questionYesNo(this, settings->getUseTrash() ?
                                            i18n("Move album '%1' to trash?")
                                            .arg(album->title()) :
                                            i18n("Delete album '%1' from disk?")
                                            .arg(album->title()));
    }

    if (result == KMessageBox::Yes)
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

void AlbumFolderView_Deprecated::albumEdit(PAlbum* album)
{
    if (!album || !album->extraData(this))
        return;

    QString     oldTitle(album->title());
    QString     oldComments(album->caption());
    QString     oldCollection(album->collection());
    QDate       oldDate(album->date());
    QStringList oldAlbumCollections(AlbumSettings::instance()->getAlbumCollectionNames());

    QString     title, comments, collection;
    QDate       date;
    QStringList albumCollections;

    if (AlbumPropsEdit::editProps(album, title, comments,
                                  date, collection,
                                  albumCollections))
    {
        if (comments != oldComments)
            album->setCaption(comments);

        if (date != oldDate && date.isValid())
            album->setDate(date);

        if (collection != oldCollection)
            album->setCollection(collection);

        AlbumSettings::instance()->setAlbumCollectionNames(albumCollections);
        resort();

        // Do this last : so that if anything else changed we can
        // successfully save to the db with the old name

        if (title != oldTitle)
        {
            QString errMsg;
            if (!albumMan_->renamePAlbum(album, title, errMsg))
                KMessageBox::error(0, errMsg);
        }

	emit signalAlbumModified();
    }
}

void AlbumFolderView_Deprecated::slotAlbumIconChanged(Album* album)
{
    if (!album || !album->extraData(this))
    {
        return;
    }

    AlbumFolderItem_Deprecated *folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
    folderItem->highlighted_ = false;

    if (album->type() == Album::PHYSICAL)
    {
        albumHighlight((PAlbum*)album);
    }
    else if (album->type() == Album::TAG)
    {
        folderItem->setPixmap(getBlendedIcon((TAlbum*)album));
    }
}

void AlbumFolderView_Deprecated::slotAlbumRenamed(Album* album)
{
    if (!album || !album->extraData(this))
    {
        return;
    }
    
    AlbumFolderItem_Deprecated *folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));

    folderItem->setText(album->title());
}

void AlbumFolderView_Deprecated::albumHighlight(PAlbum* album)
{
    if (!album || !album->extraData(this)) {
        return;
    }

    AlbumFolderItem_Deprecated *folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));

    if (folderItem->isGroupItem() || folderItem->isHighlighted())
    {
        return;
    }

    if(!album->icon().isEmpty())
    {
        if (iconThumbJob_.isNull())
        {
            iconThumbJob_ = new ThumbnailJob(album->iconKURL(),
                                             (int)ThumbnailSize::Tiny,
                                             true);
            connect(iconThumbJob_,
                    SIGNAL(signalThumbnail(const KURL&,
                                           const QPixmap&)),
                    this,
                    SLOT(slotGotThumbnailFromIcon(const KURL&,
                                                  const QPixmap&)));
            connect(iconThumbJob_,
                    SIGNAL(signalFailed(const KURL&)),
                    SLOT(slotThumbnailLost(const KURL&)));
        }
        else
        {
             iconThumbJob_->addItem(album->iconKURL());
        }
    }
    else
    {
        KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();
        folderItem->setPixmap(iconLoader->loadIcon("folder",
                                                KIcon::NoGroup,
                                                32,
                                                KIcon::DefaultState,
                                                0, true));
    }
}

void AlbumFolderView_Deprecated::slotGotThumbnailFromIcon(const KURL& url,
                                                          const QPixmap& thumbnail)
{
    PAlbum* album = albumMan_->findPAlbum(url.directory());

    if (!album || !album->extraData(this))
        return;

    AlbumFolderItem_Deprecated *folderItem =
        (AlbumFolderItem_Deprecated*)(album->extraData(this));

    folderItem->setPixmap(thumbnail);
}

void AlbumFolderView_Deprecated::slotThumbnailLost(const KURL &url)
{
    PAlbum *album = AlbumManager::instance()->findPAlbum( url.directory() );
    if (!album)
        return;
    
    AlbumFolderItem_Deprecated *folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
    KIconLoader *iconLoader = KApplication::kApplication()->iconLoader();
    folderItem->setPixmap(iconLoader->loadIcon("folder",
                                               KIcon::NoGroup,
                                               32,
                                               KIcon::DefaultState,
                                               0, true));

    //QString errMsg;
    // No point fixing this. this class is going away anyway
    //AlbumManager::instance()->updatePAlbumIcon(album, "", false, errMsg);
}


void AlbumFolderView_Deprecated::albumImportFolder()
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
    if(getSelected())
    {
        AlbumFolderItem_Deprecated *folderItem =
            dynamic_cast<AlbumFolderItem_Deprecated*>(getSelected());
        Album *a = folderItem->album();
        if (a && a->type() == Album::PHYSICAL)
        {
            parent = dynamic_cast<PAlbum*>(a);
        }
    }
    if(!parent)
        parent = dynamic_cast<PAlbum*>(phyRootItem_->album());

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

void AlbumFolderView_Deprecated::slotDIOResult(KIO::Job* job)
{
    if (job->error())
        job->showErrorDialog(this);
}

void AlbumFolderView_Deprecated::tagNew()
{
    TAlbum *album = 0;

    if (getSelected())
    {
        AlbumFolderItem_Deprecated *folderItem = static_cast<AlbumFolderItem_Deprecated*>(getSelected());
        Album *a = folderItem->album();
        if (a && a->type() == Album::TAG)
        {
            album = dynamic_cast<TAlbum*>(a);
        }
    }

    if (!album)
        album = dynamic_cast<TAlbum*>(tagRootItem_->album());

    tagNew(album);
}

void AlbumFolderView_Deprecated::tagNew(TAlbum* parent)
{
    QString title, icon;

    if (!TagCreateDlg::tagCreate(parent, title, icon))
        return;

    QString errMsg;
    if (!albumMan_->createTAlbum(parent, title, icon, errMsg))
        KMessageBox::error(0, errMsg);
}

void AlbumFolderView_Deprecated::tagDelete()
{
    Album *album = albumMan_->currentAlbum();
    if (!album || album->isRoot() || album->type() != Album::TAG)
        return;

    tagDelete(dynamic_cast<TAlbum*>(album));
}

void AlbumFolderView_Deprecated::tagDelete(TAlbum *album)
{
    if (!album || album->isRoot())
        return;

    // find number of subtags

    int children = 0;
    AlbumIterator it(album);
    while ( it.current() )
    {
        children++;
        ++it;
    }

    if (children)
    {
        int result =
            KMessageBox::warningYesNo(this, i18n("Tag '%1' has %2 subtag(s). "
                                                 "Deleting this will also delete "
                                                 "the subtag(s). "
                                                 "Are you sure you want to continue?")
                                      .arg(album->title())
                                      .arg(children));

        if (result == KMessageBox::Yes)
        {
            QString errMsg;
            if (!albumMan_->deleteTAlbum(album, errMsg))
            {
                KMessageBox::error(0, errMsg);
            }
        }

    }
    else
    {
        int result =
            KMessageBox::questionYesNo(0, i18n("Delete '%1' tag?")
                                       .arg(album->title()));

        if (result == KMessageBox::Yes)
        {
            QString errMsg;
            if (!albumMan_->deleteTAlbum(album, errMsg))
                KMessageBox::error(0, errMsg);
        }
    }
}

void AlbumFolderView_Deprecated::tagEdit()
{
    Album *album = albumMan_->currentAlbum();
    if (!album || album->isRoot() || album->type() != Album::TAG)
        return;

    tagEdit(dynamic_cast<TAlbum*>(album));
}

void AlbumFolderView_Deprecated::tagEdit(TAlbum* album)
{
    if (!album || album->isRoot())
        return;

    QString title, icon;
    if (!TagEditDlg::tagEdit(album, title, icon))
    {
        return;
    }

    AlbumFolderItem_Deprecated *folderItem =
        static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));

    if (album->title() != title)
    {
        QString errMsg;
        if (!albumMan_->renameTAlbum(album, title, errMsg))
            KMessageBox::error(0, errMsg);
        else
            folderItem->setText(title);
    }
    
    // No point fixing this. this is going away anyway
    /*
    if (album->icon() != icon)
    {
        QString errMsg;
        if (!albumMan_->updateTAlbumIcon(album, icon, false, errMsg))
            KMessageBox::error(0, errMsg);
        else
            folderItem->setPixmap(getBlendedIcon(album));
    }
    */
    
    emit signalTagsAssigned();
}

void AlbumFolderView_Deprecated::slotSelectionChanged(ListItem* item)
{
    stateAlbumSel_ = 0;

    if (!item)
    {
        albumMan_->setCurrentAlbum(0);
        return;
    }

    AlbumFolderItem_Deprecated *folderItem
        = static_cast<AlbumFolderItem_Deprecated *>(item);

    if (folderItem->isGroupItem())
    {
        albumMan_->setCurrentAlbum(0);
        return;
    }

    if (folderItem->album_)
    {
        albumMan_->setCurrentAlbum(folderItem->album_);
        stateAlbumSel_ = folderItem->album_->id();
        stateAlbumSel_ += (folderItem->album_->type() ==
                           Album::PHYSICAL) ? 100000 : 200000;
    }
}

void AlbumFolderView_Deprecated::slotDoubleClicked(ListItem* item)
{
     if (!item) return;

     item->setOpen(!item->isOpen());
}

void AlbumFolderView_Deprecated::slotRightButtonClicked(ListItem* item)
{
    if (!item)
        return;

     AlbumFolderItem_Deprecated *folderItem = dynamic_cast<AlbumFolderItem_Deprecated*>(item);

     if (!folderItem || folderItem->isGroupItem() || !folderItem->album())
         return;

     Album* a = folderItem->album();

     switch (a->type())
     {
     case (Album::PHYSICAL):
     {
         contextMenuPAlbum(dynamic_cast<PAlbum*>(a));
         break;
     }
     case (Album::TAG):
     {
         contextMenuTAlbum(dynamic_cast<TAlbum*>(a));
         break;
     }
     default:
         break;
     }
}

void AlbumFolderView_Deprecated::contextMenuPAlbum(PAlbum* album)
{
    QPopupMenu popmenu(this);
    KActionMenu menuImport(i18n("Import"));
    KActionMenu menuKIPIBatch(i18n("Batch Processes"));

    popmenu.insertItem(SmallIcon("albumfoldernew"),
                       i18n("New Album..."), 10);

    if (!album->isRoot())
    {
        popmenu.insertItem(SmallIcon("pencil"),
                           i18n("Edit Album Properties..."), 11);
        popmenu.insertSeparator();

        // Add KIPI Albums plugins Actions

        KAction *action;
        const QPtrList<KAction>& albumActions =
            DigikamApp::getinstance()->menuAlbumActions();
        QPtrListIterator<KAction> it(albumActions);

        while ((action = it.current()) != 0)
        {
            action->plug(&popmenu);
            ++it;
        }

        // Add All Import Actions

        const QPtrList<KAction> importActions =
                DigikamApp::getinstance()->menuImportActions();
        if ( !importActions.isEmpty() )
        {
            QPtrListIterator<KAction> it3(importActions);
            while ( (action = it3.current()) != 0 )
            {
                menuImport.insert(action);
                ++it3;
            }
            menuImport.plug(&popmenu);
        }

        // Add KIPI Batch processes plugins Actions

        const QPtrList<KAction>& batchActions =
            DigikamApp::getinstance()->menuBatchActions();
        if ( !batchActions.isEmpty() )
        {
            QPtrListIterator<KAction> it2(batchActions);
            while ( (action = it2.current()) != 0 )
            {
                menuKIPIBatch.insert(action);
                ++it2;
            }
            menuKIPIBatch.plug(&popmenu);
        }

        if ( !albumActions.isEmpty() || !batchActions.isEmpty() ||
             !importActions.isEmpty() )
        {
            popmenu.insertSeparator();
        }

        if (AlbumSettings::instance()->getUseTrash())
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

    switch (popmenu.exec(QCursor::pos()))
    {
    case 10:
    {
        albumNew(album);
        break;
    }
    case 11: {
        if (!album->isRoot())
            albumEdit(album);
        break;
    }
    case 12: {
        if (!album->isRoot())
            albumDelete(album);
        break;
    }
    default:
        break;
    }
}

void AlbumFolderView_Deprecated::contextMenuTAlbum(TAlbum* album)
{
    QPopupMenu popmenu(this);

    popmenu.insertItem(SmallIcon("tag"),
                       i18n("New Tag..."), 10);

    if (!album->isRoot())
    {
        popmenu.insertItem(SmallIcon("pencil"),
                           i18n("Edit Tag Properties..."), 11);
        popmenu.insertItem(SmallIcon("edittrash"),
                           i18n("Delete Tag"), 12);
    }

    switch (popmenu.exec(QCursor::pos()))
    {
    case 10:
    {
        tagNew(album);
        break;
    }
    case 11:
    {
        if (!album->isRoot())
            tagEdit(album);
        break;
    }
    case 12:
    {
        if (!album->isRoot())
            tagDelete(album);
        break;
    }
    default:
        break;
    }
}

void AlbumFolderView_Deprecated::resizeEvent(QResizeEvent* e)
{
    ListView::resizeEvent(e);

    int w = frameRect().width();
    int h = itemHeight();
    if (itemRegPix_.width()  != w || itemRegPix_.height() != h)
    {
        slotThemeChanged();
    }
}

// Drag and Drop -----------------------------------------

void AlbumFolderView_Deprecated::contentsMousePressEvent(QMouseEvent *e)
{
    if( !e ) return;

    dragStartPos_ = e->pos();
    dragItem_ = itemAt( e->pos() );

    ListView::contentsMousePressEvent(e);
}

void AlbumFolderView_Deprecated::contentsMouseMoveEvent(QMouseEvent *e)
{
    if ( !e ) return;

    ListItem* item = itemAt(e->pos());

    if ( e->state() == NoButton )
    {
        if (KGlobalSettings::changeCursorOverIcon())
        {
            if (item)
                setCursor(KCursor::handCursor());
            else
                unsetCursor();
        }
        dragItem_ = 0;
        return;
    }

    // Dragging ?
    if( dragItem_ )
    {
        if (( dragStartPos_ - e->pos() ).manhattanLength()
             > QApplication::startDragDistance() )
        {
            startDrag();
        }
        return;
    }
}

void AlbumFolderView_Deprecated::contentsMouseReleaseEvent(QMouseEvent *)
{
    //dragItem_ = 0;
}

void AlbumFolderView_Deprecated::leaveEvent(QEvent*)
{
    //dragItem_ = 0;    
}

void AlbumFolderView_Deprecated::startDrag()
{
    AlbumFolderItem_Deprecated *folderItem
        = static_cast<AlbumFolderItem_Deprecated *>(dragItem_);

    if (folderItem->isGroupItem() || folderItem->album()->isRoot())
        return;

    Album* album = folderItem->album();

    if (album->type() == Album::PHYSICAL)
    {
        PAlbum *palbum = dynamic_cast<PAlbum*>(album);
        // Start dragging an album
        AlbumDrag *albumDrag = new AlbumDrag(palbum->kurl(),
                                             palbum->id(), this);
        if(folderItem->pixmap())
            albumDrag->setPixmap(*folderItem->pixmap());
        albumDrag->drag();
    }
    else if (album->type() == Album::TAG)
    {
        // Start dragging a tag
        TagDrag *tagDrag = new TagDrag(album->id(), this);
        if(tagDrag) {
            if(folderItem->pixmap())
                tagDrag->setPixmap(*folderItem->pixmap());
            tagDrag->drag();
        }
    }
}

void AlbumFolderView_Deprecated::contentsDragEnterEvent(QDragEnterEvent*)
{
    // override the default drag enter event avoid selection problems
    // in case of a dropevent
    return;
}

void AlbumFolderView_Deprecated::contentsDragMoveEvent(QDragMoveEvent* event)
{
    if( !QUriDrag::canDecode(event) &&
        !CameraDragObject::canDecode(event) &&
        !TagDrag::canDecode(event) )
    {
        clearDropTarget();
        event->ignore();
        return;
    }

    // Get a pointer to the new drop item
    QPoint point(0, event->pos().y());
    AlbumFolderItem_Deprecated* newDropTarget = dynamic_cast<AlbumFolderItem_Deprecated*>(itemAt(point));
    if (!newDropTarget)
    {
        clearDropTarget();
        event->ignore();
        return;
    }

    if(!newDropTarget->album() && !newDropTarget->isGroupItem())
    {
        clearDropTarget();
        event->ignore();
        return;
    }

    bool validDrag = false;

    if( AlbumDrag::canDecode(event) )
    {
        // An album can be dragged on
        //   - collections to attach the album
        //   - another album, when the dragged tag isn't an ancestor of the
        //     album, to move/copy it
        //   - album root item to attach it directly under the root
        // An album cannot be dragged on
        //   - on tag root item
        //   - group items other than collections

        AlbumFolderItem_Deprecated *folderDragItem =
            dynamic_cast<AlbumFolderItem_Deprecated*>(dragItem_);
        AlbumSettings* settings = AlbumSettings::instance();

        if( folderDragItem &&
            !newDropTarget->isGroupItem() &&
            newDropTarget->album()->type() == Album::PHYSICAL &&
            !folderDragItem->album()->isAncestorOf(newDropTarget->album()) &&
            settings->getAlbumSortOrder() == AlbumSettings::ByFolder )
        {
            validDrag = true;
        }
        else if( folderDragItem && settings &&
                 newDropTarget->isGroupItem() &&
                 settings->getAlbumSortOrder() == AlbumSettings::ByCollection )
        {
            validDrag = true;
        }
    }
    else if( TagItemsDrag::canDecode(event) )
    {
        // An tag item can be dropped on
        //   - tags, to asign the tag
        // Tag items cannot be dropped on
        //   - physical albums
        //   - any root item
        //   - any group item like collections

        if( newDropTarget->album()->type() == Album::TAG &&
            !newDropTarget->isGroupItem() &&
            !newDropTarget->album()->isRoot() )
        {
            validDrag = true;
        }
    }
    else if( AlbumItemsDrag::canDecode(event) )
    {
        // An album item can be dropped on
        //   - albums, to move/copy the item
        //   - tags, to asign the tag
        // Album items cannot be dropped on
        //   - any root item
        //   - any group item like collections

        if( !newDropTarget->isGroupItem() &&
            !newDropTarget->album()->isRoot() )
        {
            validDrag = true;
        }
    }
    else if( TagDrag::canDecode(event) )
    {
        // Tags can be dropped on
        //   - other tags, when the dragged tag isn't an ancestor of the tag
        //   - the tag root to attach the tag directly under the root
        // Tags cannot be dropped on
        //   - PAlbums
        //   - GroupItems like collections

        AlbumFolderItem_Deprecated *folderDragItem =
            dynamic_cast<AlbumFolderItem_Deprecated*>(dragItem_);

        if( folderDragItem &&
            !newDropTarget->isGroupItem() &&
            newDropTarget->album()->type() == Album::TAG &&
            !folderDragItem->album()->isAncestorOf(newDropTarget->album()))
        {
            validDrag = true;
        }
    }
    else if( QUriDrag::canDecode(event) )
    {
        // Allow external objects to be dropped on
        //   - palbums
        //   - PAlbum root
        // don't allow drop on
        //   - tags
        //   - group items like collections
        //   - tag root item
        if( !newDropTarget->isGroupItem() &&
            newDropTarget->album()->type() == Album::PHYSICAL )
        {
            validDrag = true;
        }
    }

    if(validDrag)
    {
        openAlbumTimer_->start(500, true);

        event->accept();
        if (dropTarget_ == newDropTarget) {
            return;
        }

        if (dropTarget_)
            dropTarget_->removeDropHighlight();

        dropTarget_ = newDropTarget;
        dropTarget_->addDropHighlight();
    }
    else
    {
        clearDropTarget();
        event->ignore();
    }
}

void AlbumFolderView_Deprecated::contentsDragLeaveEvent(QDragLeaveEvent *)
{
    clearDropTarget();
}

void AlbumFolderView_Deprecated::contentsDropEvent(QDropEvent* event)
{
    openAlbumTimer_->stop();

    if (!dropTarget_)
        return;

    dropTarget_->removeDropHighlight();

    if(dropTarget_->isGroupItem())
    {
        AlbumFolderItem_Deprecated *item =
            static_cast<AlbumFolderItem_Deprecated*>(dragItem_);
        PAlbum *itemAlbum = static_cast<PAlbum*>(item->album());
        itemAlbum->setCollection(dropTarget_->text());
        resort();
    }
    else
    {
        Album* a = dropTarget_->album();
        if (!a)
            return;

        if (a->type() == Album::PHYSICAL) {
            PAlbum *album = static_cast<PAlbum*>(a);
            phyAlbumDropEvent(event, album);
        }
        else if (a->type() == Album::TAG) {
            TAlbum *album = static_cast<TAlbum*>(a);
            tagAlbumDropEvent(event, album);
        }
    }

    dropTarget_ = 0;
}

void AlbumFolderView_Deprecated::phyAlbumDropEvent(QDropEvent* event, PAlbum *album)
{
    if (TagItemsDrag::canDecode(event))
        return;

    if (AlbumItemsDrag::canDecode(event))
    {
        // Internal drag from one album to another
        if(album->isRoot())
            return;

        PAlbum* srcAlbum  = 0;
        PAlbum* destAlbum = album;

        KURL::List      urls;
        QValueList<int> dirIDs;

        if (!AlbumItemsDrag::decode(event, urls, dirIDs))
            return;

        if (urls.isEmpty() || dirIDs.isEmpty())
            return;

        // all the dirids will be the same
        int dirID = dirIDs.first();
        srcAlbum = AlbumManager::instance()->findPAlbum(dirID);
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
            if ( ( (keys_return[key_1 / 8]) && (1 << (key_1 % 8)) ) ||
                 ( (keys_return[key_2 / 8]) && (1 << (key_2 % 8)) ))
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
                // No point fixing this. this class is going away anyway
                //QString errMsg;
                //AlbumManager::instance()->updatePAlbumIcon(destAlbum,
                //    urls.first().filename(), true, errMsg);
            }
            return;
        }

        // If shift key is pressed while dragging, move the drag object without
        // displaying popup menu -> move
        if ( ( (keys_return[key_3 / 8]) && (1 << (key_3 % 8)) ) ||
             ( (keys_return[key_4 / 8]) && (1 << (key_4 % 8)) ))
        {
            id = 10;
        }
        // If ctrl key is pressed while dragging, copy the drag object without
        // displaying popup menu -> copy
        else if ( ( (keys_return[key_1 / 8]) && (1 << (key_1 % 8)) ) ||
                  ( (keys_return[key_2 / 8]) && (1 << (key_2 % 8)) ))
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
            KIO::Job* job = DIO::move(urls, destAlbum->kurl());
            connect(job, SIGNAL(result(KIO::Job*)),
                    SLOT(slotDIOResult(KIO::Job*)));
            break;
        }
        case 11:
        {
            KIO::Job* job = DIO::copy(urls, destAlbum->kurl());
            connect(job, SIGNAL(result(KIO::Job*)),
                    SLOT(slotDIOResult(KIO::Job*)));
            break;
        }
        default:
            break;
        }
    }
    else if (QUriDrag::canDecode(event))
    {
        // DnD from an external source

        PAlbum *destAlbum = album;
        KURL destURL(destAlbum->kurl());

        KURL::List srcURLs;
        KURLDrag::decode(event, srcURLs);

        char keys_return[32];
        XQueryKeymap(x11Display(), keys_return);
        int id = 0;

        int key_1 = XKeysymToKeycode(x11Display(), 0xFFE3);
        int key_2 = XKeysymToKeycode(x11Display(), 0xFFE4);
        int key_3 = XKeysymToKeycode(x11Display(), 0xFFE1);
        int key_4 = XKeysymToKeycode(x11Display(), 0xFFE2);
        // If shift key is pressed while dropping, move the drag object without
        // displaying popup menu -> move
        if ( ( (keys_return[key_3 / 8]) && (1 << (key_3 % 8)) ) ||
             ( (keys_return[key_4 / 8]) && (1 << (key_4 % 8)) ))
        {
            id = 10;
        }
        // If ctrl key is pressed while dropping, copy the drag object without
        // displaying popup menu -> copy
        else if ( ( (keys_return[key_1 / 8]) && (1 << (key_1 % 8)) ) ||
                  ( (keys_return[key_2 / 8]) && (1 << (key_2 % 8)) ))
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

        switch(id) {
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
    }
}

void AlbumFolderView_Deprecated::tagAlbumDropEvent(QDropEvent* event, TAlbum *album)
{
    if( TagItemsDrag::canDecode(event) || AlbumItemsDrag::canDecode(event) )
    {
        KURL::List      urls;
        QValueList<int> dirIDs;

        if (!TagItemsDrag::decode(event, urls, dirIDs))
            return;

        char keys_return[32];
        XQueryKeymap(x11Display(), keys_return);
        int key_1 = XKeysymToKeycode(x11Display(), 0xFFE3);
        int key_2 = XKeysymToKeycode(x11Display(), 0xFFE4);

        if( AlbumManager::instance()->currentAlbum() == album )
        {
            int id = 0;
            // Setting the dropped image as the tag thumbnail
            // If the ctrl key is pressed, when dropping the image, the
            // thumbnail is set without a popup menu
            if ( ( (keys_return[key_1 / 8]) && (1 << (key_1 % 8)) ) ||
                 ( (keys_return[key_2 / 8]) && (1 << (key_2 % 8)) ))
            {
                id = 12;
            }
            else
            {
                QPopupMenu popMenu(this);
                popMenu.insertItem(i18n("Set as Tag Thumbnail"), 12);
                popMenu.insertSeparator(-1);
                popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

                popMenu.setMouseTracking(true);
                id = popMenu.exec(QCursor::pos());
            }

            if(id == 12)
            {
                // No point fixing this.
                /*
                QString errMsg;
                AlbumManager::instance()->updateTAlbumIcon(album,
                    urls.first().path(), true, errMsg);
                */
            }
            return;
        }

        // If a ctrl key is pressed while dropping the drag object,
        // the tag is assigned to the images without showing a
        // popup menu.
        if (!( (keys_return[key_1 / 8]) && (1 << (key_1 % 8)) ) ||
             ( (keys_return[key_2 / 8]) && (1 << (key_2 % 8)) ))
        {
            QPopupMenu popmenu(this);
            popmenu.insertItem(SmallIcon("tag"),
                            i18n("Assign Tag '%1' to Dropped Items")
                            .arg(album->prettyURL()), 10);
            popmenu.insertSeparator(-1);
            popmenu.insertItem( SmallIcon("cancel"), i18n("C&ancel") );

            if (popmenu.exec(QCursor::pos()) != 10)
                return;
        }

        AlbumDB* db = AlbumManager::instance()->albumDB();

        db->beginTransaction();
        KURL::List::const_iterator      itU = urls.begin();
        QValueList<int>::const_iterator itD = dirIDs.begin();
        for ( ; itU != urls.end() || itD != dirIDs.end(); ++itU, ++itD)
        {
            db->addItemTag(*itD, (*itU).fileName(), album->id());
        }
        db->commitTransaction();

        emit signalTagsAssigned();
    }
    else if(TagDrag::canDecode(event))
    {
        QPopupMenu popMenu(this);
        popMenu.insertItem( SmallIcon("goto"), i18n("&Move Here"), 10 );
        popMenu.insertSeparator(-1);
        popMenu.insertItem( SmallIcon("cancel"), i18n("C&ancel"), 20 );
        popMenu.setMouseTracking(true);
        int id = popMenu.exec(QCursor::pos());

        if(id == 10)
        {
            AlbumFolderItem_Deprecated *item =
                static_cast<AlbumFolderItem_Deprecated*>(dragItem_);
            TAlbum *itemAlbum = static_cast<TAlbum*>(item->album());
            TAlbum *parent = static_cast<TAlbum*>(album);

            QString errMsg;
            AlbumManager::instance()->moveTAlbum(itemAlbum, parent, errMsg);

            ListItem *parentItem = dragItem_->parent();
            parentItem->removeChild(dragItem_);
            AlbumFolderItem_Deprecated* dropItem =
                static_cast<AlbumFolderItem_Deprecated*>(album->extraData(this));
            dropItem->insertChild(dragItem_);
            if (!dropItem->isOpen())
                dropItem->setOpen(true);
        }
    }
}

void AlbumFolderView_Deprecated::clearDropTarget()
{
    if (dropTarget_)
        dropTarget_->removeDropHighlight();
    dropTarget_ = 0;
    openAlbumTimer_->stop();
}

void AlbumFolderView_Deprecated::slotOpenAlbumFolderItem_Deprecated()
{
    openAlbumTimer_->stop();
    if(dropTarget_ && !dropTarget_->isOpen())
        dropTarget_->setOpen(true);
}

void AlbumFolderView_Deprecated::paintItemBase(QPainter* p, const QColorGroup&,
                                    const QRect& r, bool selected)
{
    p->drawPixmap(r.x(), r.y(), selected ? itemSelPix_ : itemRegPix_);
}

void AlbumFolderView_Deprecated::slotThemeChanged()
{
    int w = frameRect().width();
    int h = itemHeight();

    itemRegPix_ = ThemeEngine::instance()->listRegPixmap(w, h);
    itemSelPix_ = ThemeEngine::instance()->listSelPixmap(w, h);

    QPalette plt(palette());
    QColorGroup cg(plt.active());
    cg.setColor(QColorGroup::Base, ThemeEngine::instance()->baseColor());
    cg.setColor(QColorGroup::Text, ThemeEngine::instance()->textRegColor());
    cg.setColor(QColorGroup::HighlightedText, ThemeEngine::instance()->textSelColor());
    plt.setActive(cg);
    plt.setInactive(cg);
    setPalette(plt);

    viewport()->update();
}

void AlbumFolderView_Deprecated::loadAlbumState()
{
/*    QString filePath = locateLocal("appdata", "albumtreestate.bin");
    QFile file(filePath);
    if (!file.open(IO_ReadOnly))
    {
        kdWarning() << k_funcinfo << "Failed to open albumtreestate.bin"
                    << endl;
        return;
    }

    QDataStream ds(&file);
    ds >> stateAlbumSel_;
    ds >> stateAlbumOpen_;
    file.close();*/
}

void AlbumFolderView_Deprecated::saveAlbumState()
{
    stateAlbumSel_ = 100000;
    if (getSelected())
    {
        AlbumFolderItem_Deprecated *folderItem =
            dynamic_cast<AlbumFolderItem_Deprecated*>(getSelected());
        Album *a = folderItem->album();
        if (a)
        {
            stateAlbumSel_  = a->id();
            stateAlbumSel_ += (a->type() == Album::PHYSICAL) ? 100000 : 200000;
        }
    }

    stateAlbumOpen_.clear();

    stateAlbumOpen_.insert(100000, true);
    stateAlbumOpen_.insert(200000, true);

    AlbumList pList(AlbumManager::instance()->allPAlbums());
    for (AlbumList::iterator it = pList.begin(); it != pList.end(); ++it)
    {
        PAlbum *a = (PAlbum*)(*it);
        if (!a->isRoot() && a->extraData(this))
        {
            AlbumFolderItem_Deprecated* folderItem =
                static_cast<AlbumFolderItem_Deprecated*>(a->extraData(this));
            stateAlbumOpen_.insert(100000 + a->id(),
                                   folderItem->parent()->isOpen());
        }
    }

    AlbumList tList(AlbumManager::instance()->allTAlbums());
    for (AlbumList::iterator it = tList.begin(); it != tList.end(); ++it)
    {
        TAlbum *a = (TAlbum*)(*it);
        if (!a->isRoot() && a->extraData(this))
        {
            AlbumFolderItem_Deprecated* folderItem =
                static_cast<AlbumFolderItem_Deprecated*>(a->extraData(this));
            stateAlbumOpen_.insert(200000 + a->id(),
                                   folderItem->parent()->isOpen());
        }
    }


    QString filePath = locateLocal("appdata", "albumtreestate.bin");
    QFile file(filePath);
    if (!file.open(IO_WriteOnly))
    {
        kdWarning() << k_funcinfo << "Failed to open albumtreestate.bin"
                    << endl;
        return;
    }

    QDataStream ds(&file);
    ds << stateAlbumSel_;
    ds << stateAlbumOpen_;
    file.close();
}

#include "albumfolderview_deprecated.moc"
