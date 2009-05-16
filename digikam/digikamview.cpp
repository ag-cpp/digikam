/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : implementation of album view interface.
 *
 * Copyright (C) 2002-2005 by Renchi Raju  <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2002-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikamview.h"
#include "digikamview.moc"

// Qt includes

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QEvent>
#include <QFrame>
#include <QApplication>
#include <QSplitter>
#include <QTimer>
#include <QLabel>
#include <QListView>
#include <QTreeView>

// KDE includes

#include <kdebug.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <krun.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kvbox.h>
#include <kconfiggroup.h>

// LibKDcraw includes

#include <libkdcraw/rawfiles.h>

// Local includes

#include "config-digikam.h"
#include "album.h"
#include "albumfolderview.h"
#include "albumhistory.h"
#include "albummanager.h"
#include "albummodel.h"
#include "albumsettings.h"
#include "albumwidgetstack.h"
#include "batchsyncmetadata.h"
#include "collectionmanager.h"
#include "datefolderview.h"
#include "digikamapp.h"
#include "digikamimageview.h"
#include "dio.h"
#include "dmetadata.h"
#include "fuzzysearchfolderview.h"
#include "fuzzysearchview.h"
#include "gpssearchfolderview.h"
#include "gpssearchview.h"
#include "imageinfoalbumsjob.h"
#include "imagepreviewview.h"
#include "imagepropertiessidebardb.h"
#include "imageviewutilities.h"
#include "queuemgrwindow.h"
#include "searchfolderview.h"
#include "searchtabheader.h"
#include "sidebar.h"
#include "slideshow.h"
#include "statusprogressbar.h"
#include "tagfilterview.h"
#include "tagfolderview.h"
#include "themeengine.h"
#include "thumbnailsize.h"
#include "timelinefolderview.h"
#include "timelineview.h"

namespace Digikam
{

class DigikamViewPriv
{
public:

    DigikamViewPriv()
    {
        folderBox             = 0;
        tagBox                = 0;
        searchBox             = 0;
        tagFilterBox          = 0;
        folderSearchBar       = 0;
        tagSearchBar          = 0;
        searchSearchBar       = 0;
        tagFilterSearchBar    = 0;
        splitter              = 0;
        parent                = 0;
        iconView              = 0;
        folderView            = 0;
        albumManager          = 0;
        albumHistory          = 0;
        leftSideBar           = 0;
        rightSideBar          = 0;
        dateFolderView        = 0;
        timeLineView          = 0;
        tagFolderView         = 0;
        searchFolderView      = 0;
        tagFilterView         = 0;
        albumWidgetStack      = 0;
        selectionTimer        = 0;
        thumbSizeTimer        = 0;
        fuzzySearchView       = 0;
        gpsSearchView         = 0;
        needDispatchSelection = false;
        cancelSlideShow       = false;
        thumbSize             = ThumbnailSize::Medium;
    }

    QString userPresentableAlbumTitle(const QString& album);

    bool                      needDispatchSelection;
    bool                      cancelSlideShow;

    int                       initialAlbumID;
    int                       thumbSize;

    SidebarSplitter          *splitter;

    QTimer                   *selectionTimer;
    QTimer                   *thumbSizeTimer;

    KVBox                    *folderBox;
    KVBox                    *tagBox;
    KVBox                    *searchBox;
    KVBox                    *tagFilterBox;

    SearchTextBar            *folderSearchBar;
    SearchTextBar            *tagSearchBar;
    SearchTextBar            *searchSearchBar;
    SearchTextBar            *tagFilterSearchBar;

    DigikamApp               *parent;

    DigikamImageView         *iconView;
    AlbumFolderView          *folderView;
    AlbumManager             *albumManager;
    AlbumHistory             *albumHistory;
    AlbumWidgetStack         *albumWidgetStack;

    Sidebar                  *leftSideBar;
    ImagePropertiesSideBarDB *rightSideBar;

    DateFolderView           *dateFolderView;
    TimeLineView             *timeLineView;
    TagFolderView            *tagFolderView;
    SearchFolderView         *searchFolderView;
    SearchTabHeader          *searchTabHeader;
    TagFilterView            *tagFilterView;
    FuzzySearchView          *fuzzySearchView;
    GPSSearchView            *gpsSearchView;
};

DigikamView::DigikamView(QWidget *parent)
           : KHBox(parent), d(new DigikamViewPriv)
{
    d->parent       = static_cast<DigikamApp*>(parent);
    d->albumManager = AlbumManager::instance();

    d->splitter = new SidebarSplitter;
    d->splitter->setFrameStyle( QFrame::NoFrame );
    d->splitter->setFrameShadow( QFrame::Plain );
    d->splitter->setFrameShape( QFrame::NoFrame );
    d->splitter->setOpaqueResize(false);

    d->leftSideBar = new Sidebar(this, d->splitter, KMultiTabBar::Left);
    d->leftSideBar->setObjectName("Digikam Left Sidebar");
    d->splitter->setParent(this);

    d->albumWidgetStack = new AlbumWidgetStack(d->splitter);
    d->splitter->setStretchFactor(1, 10);      // set AlbumWidgetStack default size to max.
    d->iconView         = d->albumWidgetStack->imageIconView();
    d->rightSideBar     = new ImagePropertiesSideBarDB(this, d->splitter, KMultiTabBar::Right, true);
    d->rightSideBar->setObjectName("Digikam Right Sidebar");

    // To the left.
    // Folders sidebar tab contents.
    d->folderBox       = new KVBox(this);
    d->folderView      = new AlbumFolderView(d->folderBox);
    d->folderSearchBar = new SearchTextBar(d->folderBox, "DigikamViewFolderSearchBar");
    d->folderBox->setSpacing(KDialog::spacingHint());
    d->folderBox->setMargin(0);

    // Tags sidebar tab contents.
    d->tagBox        = new KVBox(this);
    d->tagFolderView = new TagFolderView(d->tagBox);
    d->tagSearchBar  = new SearchTextBar(d->tagBox, "DigikamViewTagSearchBar");
    d->tagBox->setSpacing(KDialog::spacingHint());
    d->tagBox->setMargin(0);

    // Search sidebar tab contents.
    d->searchBox        = new KVBox(this);
    d->searchTabHeader  = new SearchTabHeader(d->searchBox);
    d->searchFolderView = new SearchFolderView(d->searchBox);
    d->searchSearchBar  = new SearchTextBar(d->searchBox, "DigikamViewSearchSearchBar");
    d->searchBox->setStretchFactor(d->searchFolderView, 1);
    d->searchBox->setSpacing(KDialog::spacingHint());
    d->searchBox->setMargin(0);

    d->dateFolderView   = new DateFolderView(this);
    d->timeLineView     = new TimeLineView(this);
    d->fuzzySearchView  = new FuzzySearchView(this);
#ifdef HAVE_MARBLEWIDGET
    d->gpsSearchView    = new GPSSearchView(this);
#endif

    d->leftSideBar->appendTab(d->folderBox, SmallIcon("folder-image"), i18n("Albums"));
    d->leftSideBar->appendTab(d->dateFolderView, SmallIcon("view-calendar-list"), i18n("Calendar"));
    d->leftSideBar->appendTab(d->tagBox, SmallIcon("tag"), i18n("Tags"));
    d->leftSideBar->appendTab(d->timeLineView, SmallIcon("player-time"), i18n("Timeline"));
    d->leftSideBar->appendTab(d->searchBox, SmallIcon("edit-find"), i18n("Searches"));
    d->leftSideBar->appendTab(d->fuzzySearchView, SmallIcon("tools-wizard"), i18n("Fuzzy Searches"));
#ifdef HAVE_MARBLEWIDGET
    d->leftSideBar->appendTab(d->gpsSearchView, SmallIcon("applications-internet"), i18n("Map Searches"));
#endif

    // To the right.

    // Tags Filter sidebar tab contents.
    d->tagFilterBox       = new KVBox(this);
    d->tagFilterView      = new TagFilterView(d->tagFilterBox);
    d->tagFilterSearchBar = new SearchTextBar(d->tagFilterBox, "DigikamViewTagFilterSearchBar");
    d->tagFilterBox->setSpacing(KDialog::spacingHint());
    d->tagFilterBox->setMargin(0);

    d->rightSideBar->appendTab(d->tagFilterBox, SmallIcon("tag-assigned"), i18n("Tag Filters"));

    d->selectionTimer = new QTimer(this);
    d->thumbSizeTimer = new QTimer(this);
    d->thumbSizeTimer->setSingleShot(true);
    d->thumbSizeTimer->setInterval(300);

    setupConnections();

    d->albumHistory = new AlbumHistory();

    slotSidebarTabTitleStyleChanged();
}

DigikamView::~DigikamView()
{
    saveViewState();

    delete d->albumHistory;
    delete d;
}

void DigikamView::applySettings()
{
    AlbumSettings *settings = AlbumSettings::instance();
    d->albumWidgetStack->applySettings();
    
    d->folderView->setEnableToolTips(settings->getShowAlbumToolTips());
    refreshView();
}

void DigikamView::refreshView()
{
    d->folderView->refresh();
    d->dateFolderView->refresh();
    d->tagFolderView->refresh();
    d->tagFilterView->refresh();
    d->rightSideBar->refreshTagsView();
}

void DigikamView::setupConnections()
{
    // -- DigikamApp connections ----------------------------------

    connect(d->parent, SIGNAL(signalEscapePressed()),
            this, SLOT(slotEscapePreview()));

    connect(d->parent, SIGNAL(signalEscapePressed()),
            d->albumWidgetStack, SLOT(slotEscapePreview()));

    connect(d->parent, SIGNAL(signalNextItem()),
            this, SLOT(slotNextItem()));

    connect(d->parent, SIGNAL(signalPrevItem()),
            this, SLOT(slotPrevItem()));

    connect(d->parent, SIGNAL(signalFirstItem()),
            this, SLOT(slotFirstItem()));

    connect(d->parent, SIGNAL(signalLastItem()),
            this, SLOT(slotLastItem()));

    connect(d->parent, SIGNAL(signalCopyAlbumItemsSelection()),
            d->iconView, SLOT(copy()));

    connect(d->parent, SIGNAL(signalPasteAlbumItemsSelection()),
            d->iconView, SLOT(paste()));

    connect(this, SIGNAL(signalProgressBarMode(int, const QString&)),
            d->parent, SLOT(slotProgressBarMode(int, const QString&)));

    connect(this, SIGNAL(signalProgressValue(int)),
            d->parent, SLOT(slotProgressValue(int)));

    connect(d->parent, SIGNAL(signalCancelButtonPressed()),
            this, SLOT(slotCancelSlideShow()));

    // -- AlbumManager connections --------------------------------

    connect(d->albumManager, SIGNAL(signalAlbumCurrentChanged(Album*)),
            this, SLOT(slotAlbumSelected(Album*)));

    connect(d->albumManager, SIGNAL(signalAllAlbumsLoaded()),
            this, SLOT(slotAllAlbumsLoaded()));

    connect(d->albumManager, SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotAlbumAdded(Album*)));

    connect(d->albumManager, SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));

    connect(d->albumManager, SIGNAL(signalAlbumRenamed(Album*)),
            this, SLOT(slotAlbumRenamed(Album*)));

    connect(d->albumManager, SIGNAL(signalAlbumsCleared()),
            this, SLOT(slotAlbumsCleared()));

    // -- IconView Connections -------------------------------------

    connect(d->iconView->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(slotImageSelected()));

    connect(d->iconView->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(slotImageSelected()));

    connect(d->iconView->model(), SIGNAL(layoutChanged()),
            this, SLOT(slotImageSelected()));

    connect(d->iconView, SIGNAL(previewRequested(const ImageInfo &)),
            this, SLOT(slotTogglePreviewMode(const ImageInfo &)));

    connect(d->iconView, SIGNAL(gotoAlbumAndImageRequested(const ImageInfo&)),
            this, SLOT(slotGotoAlbumAndItem(const ImageInfo&)));

    connect(d->iconView, SIGNAL(gotoDateAndImageRequested(const ImageInfo&)),
            this, SLOT(slotGotoDateAndItem(const ImageInfo&)));

    connect(d->iconView, SIGNAL(gotoTagAndImageRequested(int)),
            this, SLOT(slotGotoTagAndItem(int)));

    connect(d->iconView, SIGNAL(zoomOutStep()),
            this, SLOT(slotZoomOut()));

    connect(d->iconView, SIGNAL(zoomInStep()),
            this, SLOT(slotZoomIn()));

    // -- Sidebar Connections -------------------------------------

    connect(d->leftSideBar, SIGNAL(signalChangedTab(QWidget*)),
            this, SLOT(slotLeftSidebarChangedTab(QWidget*)));

    connect(d->rightSideBar, SIGNAL(signalFirstItem()),
            this, SLOT(slotFirstItem()));

    connect(d->rightSideBar, SIGNAL(signalNextItem()),
            this, SLOT(slotNextItem()));

    connect(d->rightSideBar, SIGNAL(signalPrevItem()),
            this, SLOT(slotPrevItem()));

    connect(d->rightSideBar, SIGNAL(signalLastItem()),
            this, SLOT(slotLastItem()));

    connect(this, SIGNAL(signalNoCurrentItem()),
            d->rightSideBar, SLOT(slotNoCurrentItem()));

    connect(d->rightSideBar, SIGNAL(signalProgressBarMode(int, const QString&)),
            d->parent, SLOT(slotProgressBarMode(int, const QString&)));

    connect(d->rightSideBar, SIGNAL(signalProgressValue(int)),
            d->parent, SLOT(slotProgressValue(int)));

    connect(d->tagFilterView, SIGNAL(signalProgressBarMode(int, const QString&)),
            d->parent, SLOT(slotProgressBarMode(int, const QString&)));

    connect(d->tagFilterView, SIGNAL(signalProgressValue(int)),
            d->parent, SLOT(slotProgressValue(int)));

    connect(d->tagFolderView, SIGNAL(signalProgressBarMode(int, const QString&)),
            d->parent, SLOT(slotProgressBarMode(int, const QString&)));

    connect(d->tagFolderView, SIGNAL(signalProgressValue(int)),
            d->parent, SLOT(slotProgressValue(int)));

    connect(d->parent, SIGNAL(signalResetTagFilters()),
            d->tagFilterView, SLOT(slotResetTagFilters()));

    connect(d->fuzzySearchView, SIGNAL(signalUpdateFingerPrints()),
            d->parent, SLOT(slotRebuildAllFingerPrints()));

    // -- Filter Bars Connections ---------------------------------

    connect(d->folderSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->folderView, SLOT(slotTextFolderFilterChanged(const SearchTextSettings&)));

    connect(d->tagSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->tagFolderView, SLOT(slotTextTagFilterChanged(const SearchTextSettings&)));

    connect(d->searchSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->searchFolderView, SLOT(slotTextSearchFilterChanged(const SearchTextSettings&)));

    connect(d->tagFilterSearchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            d->tagFilterView, SLOT(slotTextTagFilterChanged(const SearchTextSettings&)));

    connect(d->folderView, SIGNAL(signalTextFolderFilterMatch(bool)),
            d->folderSearchBar, SLOT(slotSearchResult(bool)));

    connect(d->tagFolderView, SIGNAL(signalTextTagFilterMatch(bool)),
            d->tagSearchBar, SLOT(slotSearchResult(bool)));

    connect(d->searchFolderView, SIGNAL(newSearch()),
            d->searchTabHeader, SLOT(newAdvancedSearch()));

    connect(d->searchFolderView, SIGNAL(signalTextSearchFilterMatch(bool)),
            d->searchSearchBar, SLOT(slotSearchResult(bool)));

    connect(d->searchFolderView, SIGNAL(editSearch(SAlbum *)),
            d->searchTabHeader, SLOT(editSearch(SAlbum *)));

    connect(d->searchFolderView, SIGNAL(selectedSearchChanged(SAlbum *)),
            d->searchTabHeader, SLOT(selectedSearchChanged(SAlbum *)));

    connect(d->searchTabHeader, SIGNAL(searchShallBeSelected(SAlbum *)),
            d->searchFolderView, SLOT(slotSelectSearch(SAlbum *)));

    connect(d->tagFilterView, SIGNAL(signalTextTagFilterMatch(bool)),
            d->tagFilterSearchBar, SLOT(slotSearchResult(bool)));

    // -- Preview image widget Connections ------------------------

    connect(d->albumWidgetStack, SIGNAL(signalNextItem()),
            this, SLOT(slotNextItem()));

    connect(d->albumWidgetStack, SIGNAL(signalPrevItem()),
            this, SLOT(slotPrevItem()));

    connect(d->albumWidgetStack, SIGNAL(signalEditItem()),
            this, SLOT(slotImageEdit()));

    connect(d->albumWidgetStack, SIGNAL(signalDeleteItem()),
            this, SLOT(slotImageDelete()));

    connect(d->albumWidgetStack, SIGNAL(signalToggledToPreviewMode(bool)),
            this, SLOT(slotToggledToPreviewMode(bool)));

    connect(d->albumWidgetStack, SIGNAL(signalBack2Album()),
            this, SLOT(slotEscapePreview()));

    connect(d->albumWidgetStack, SIGNAL(signalSlideShow()),
            this, SLOT(slotSlideShowAll()));

    connect(d->albumWidgetStack, SIGNAL(signalZoomFactorChanged(double)),
            this, SLOT(slotZoomFactorChanged(double)));

    connect(d->albumWidgetStack, SIGNAL(signalInsert2LightTable()),
            this, SLOT(slotImageAddToLightTable()));

    connect(d->albumWidgetStack, SIGNAL(signalInsert2QueueMgr()),
            this, SLOT(slotImageAddToCurrentQueue()));

    connect(d->albumWidgetStack, SIGNAL(signalFindSimilar()),
            this, SLOT(slotImageFindSimilar()));

    connect(d->albumWidgetStack, SIGNAL(signalUrlSelected(const KUrl&)),
            this, SLOT(slotSelectItemByUrl(const KUrl&)));

    connect(d->albumWidgetStack, SIGNAL(signalAddToExistingQueue(int)),
            this, SLOT(slotImageAddToExistingQueue(int)));

    connect(d->albumWidgetStack, SIGNAL(signalGotoAlbumAndItem(ImageInfo&)),
            this, SLOT(slotGotoAlbumAndItem(const ImageInfo&)));

    connect(d->albumWidgetStack, SIGNAL(signalGotoDateAndItem(ImageInfo&)),
            this, SLOT(slotGotoDateAndItem(const ImageInfo&)));

    connect(d->albumWidgetStack, SIGNAL(signalGotoTagAndItem(int)),
            this, SLOT(slotGotoTagAndItem(int)));

    // -- timers ---------------

    connect(d->selectionTimer, SIGNAL(timeout()),
            this, SLOT(slotDispatchImageSelected()));

    connect(d->thumbSizeTimer, SIGNAL(timeout()),
            this, SLOT(slotThumbSizeEffect()) );

    // -- Album Settings ----------------

    connect(AlbumSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSidebarTabTitleStyleChanged()));
}

void DigikamView::loadViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("MainWindow");

    d->splitter->restoreState(group);
    d->initialAlbumID = group.readEntry("InitialAlbumID", 0);
}

void DigikamView::saveViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("MainWindow");
    d->splitter->saveState(group);

    Album *album = AlbumManager::instance()->currentAlbum();
    if(album)
    {
        group.writeEntry("InitialAlbumID", album->globalID());
    }
    else
    {
        group.writeEntry("InitialAlbumID", 0);
    }
}

KUrl::List DigikamView::allUrls() const
{
    return d->iconView->urls();
}

KUrl::List DigikamView::selectedUrls() const
{
    return d->iconView->selectedUrls();
}

void DigikamView::showSideBars()
{
    d->leftSideBar->restore();
    d->rightSideBar->restore();
}

void DigikamView::hideSideBars()
{
    d->leftSideBar->backup();
    d->rightSideBar->backup();
}

void DigikamView::slotFirstItem()
{
    d->iconView->toFirstIndex();
}

void DigikamView::slotPrevItem()
{
    d->iconView->toPreviousIndex();
}

void DigikamView::slotNextItem()
{
    d->iconView->toNextIndex();
}

void DigikamView::slotLastItem()
{
    d->iconView->toLastIndex();
}

void DigikamView::slotSelectItemByUrl(const KUrl& url)
{
    d->iconView->toIndex(url);
}

void DigikamView::slotAllAlbumsLoaded()
{
    disconnect(d->albumManager, SIGNAL(signalAllAlbumsLoaded()),
               this, SLOT(slotAllAlbumsLoaded()));

    loadViewState();
    Album *album = d->albumManager->findAlbum(d->initialAlbumID);
    d->albumManager->setCurrentAlbum(album);

    d->leftSideBar->loadViewState();
    d->rightSideBar->loadViewState();
    d->rightSideBar->populateTags();

    slotAlbumSelected(album);
}

void DigikamView::slotSortAlbums(int order)
{
    AlbumSettings* settings = AlbumSettings::instance();
    if (!settings) return;
    settings->setAlbumSortOrder((AlbumSettings::AlbumSortOrder) order);
    d->folderView->resort();
}

void DigikamView::slotNewAlbum()
{
    d->folderView->albumNew();
}

void DigikamView::slotNewAlbumFromSelection()
{
    d->iconView->createNewAlbumForSelected();
}

void DigikamView::slotDeleteAlbum()
{
    d->folderView->albumDelete();
}

void DigikamView::slotNewTag()
{
    d->tagFolderView->tagNew();
}

void DigikamView::slotDeleteTag()
{
    d->tagFolderView->tagDelete();
}

void DigikamView::slotEditTag()
{
    d->tagFolderView->tagEdit();
}

void DigikamView::slotNewKeywordSearch()
{
    if (d->leftSideBar->getActiveTab() != d->searchBox)
        d->leftSideBar->setActiveTab(d->searchBox);
    d->searchTabHeader->newKeywordSearch();
}

void DigikamView::slotNewAdvancedSearch()
{
    if (d->leftSideBar->getActiveTab() != d->searchBox)
        d->leftSideBar->setActiveTab(d->searchBox);
    d->searchTabHeader->newAdvancedSearch();
}

void DigikamView::slotNewDuplicatesSearch()
{
    if (d->leftSideBar->getActiveTab() != d->fuzzySearchView)
        d->leftSideBar->setActiveTab(d->fuzzySearchView);
    d->fuzzySearchView->newDuplicatesSearch();
}

void DigikamView::slotAlbumAdded(Album *album)
{
    if (!album->isRoot())
    {
        switch (album->type())
        {
            case Album::PHYSICAL:
            {
                d->folderSearchBar->completionObject()->addItem(album->title());
                break;
            }
            case Album::TAG:
            {
                d->tagSearchBar->completionObject()->addItem(album->title());
                d->tagFilterSearchBar->completionObject()->addItem(album->title());
                break;
            }
            case Album::SEARCH:
            {
                SAlbum* salbum = (SAlbum*)(album);
                if (salbum->isNormalSearch() || salbum->isKeywordSearch() || salbum->isAdvancedSearch())
                    d->searchSearchBar->completionObject()->addItem(salbum->title());
                else if (salbum->isTimelineSearch())
                    d->timeLineView->searchBar()->completionObject()->addItem(salbum->title());
                else if (salbum->isHaarSearch())
                    d->fuzzySearchView->searchBar()->completionObject()->addItem(salbum->title());
#ifdef HAVE_MARBLEWIDGET
                else if (salbum->isMapSearch())
                    d->gpsSearchView->searchBar()->completionObject()->addItem(salbum->title());
#endif

                break;
            }
            default:
            {
                // Nothing to do with Album::DATE
                break;
            }
        }
    }
}

void DigikamView::slotAlbumDeleted(Album *album)
{
    d->albumHistory->deleteAlbum(album);

    /*
    // For what is this needed?
    Album *a;
    QWidget *widget;
    d->albumHistory->getCurrentAlbum(&a, &widget);

    changeAlbumFromHistory(a, widget);
    */

    if (!album->isRoot())
    {
        switch (album->type())
        {
            case Album::PHYSICAL:
            {
                d->folderSearchBar->completionObject()->removeItem(album->title());
                break;
            }
            case Album::TAG:
            {
                d->tagSearchBar->completionObject()->removeItem(album->title());
                d->tagFilterSearchBar->completionObject()->removeItem(album->title());
                break;
            }
            case Album::SEARCH:
            {
                SAlbum* salbum = (SAlbum*)(album);
                if (salbum->isNormalSearch() || salbum->isKeywordSearch() || salbum->isAdvancedSearch())
                    d->searchSearchBar->completionObject()->removeItem(salbum->title());
                else if (salbum->isTimelineSearch())
                    d->timeLineView->searchBar()->completionObject()->removeItem(salbum->title());
                else if (salbum->isHaarSearch())
                    d->fuzzySearchView->searchBar()->completionObject()->removeItem(salbum->title());
#ifdef HAVE_MARBLEWIDGET
                else if (salbum->isMapSearch())
                    d->gpsSearchView->searchBar()->completionObject()->removeItem(salbum->title());
#endif

                break;
            }
            default:
            {
                // Nothing to do with Album::DATE
                break;
            }
        }
    }
}

void DigikamView::slotAlbumRenamed(Album *album)
{
    // display changed names

    if (!album->isRoot())
    {
        switch (album->type())
        {
            case Album::PHYSICAL:
            {
                d->folderSearchBar->completionObject()->addItem(album->title());
                d->folderView->slotTextFolderFilterChanged(d->folderSearchBar->searchTextSettings());
                break;
            }
            case Album::TAG:
            {
                d->tagSearchBar->completionObject()->addItem(album->title());
                d->tagFolderView->slotTextTagFilterChanged(d->tagSearchBar->searchTextSettings());

                d->tagFilterSearchBar->completionObject()->addItem(album->title());
                d->tagFilterView->slotTextTagFilterChanged(d->tagFilterSearchBar->searchTextSettings());
                break;
            }
            case Album::SEARCH:
            {
                SAlbum* salbum = (SAlbum*)(album);
                if (salbum->isNormalSearch() || salbum->isKeywordSearch() || salbum->isAdvancedSearch())
                {
                    d->searchSearchBar->completionObject()->addItem(salbum->title());
                    d->searchFolderView->slotTextSearchFilterChanged(d->searchSearchBar->searchTextSettings());
                }
                else if (salbum->isTimelineSearch())
                {
                    d->timeLineView->searchBar()->completionObject()->addItem(salbum->title());
                    d->timeLineView->folderView()->slotTextSearchFilterChanged(d->timeLineView->searchBar()->searchTextSettings());
                }
                else if (salbum->isHaarSearch())
                {
                    d->fuzzySearchView->searchBar()->completionObject()->addItem(salbum->title());
                    d->fuzzySearchView->folderView()->slotTextSearchFilterChanged(d->fuzzySearchView->searchBar()->searchTextSettings());
                }
#ifdef HAVE_MARBLEWIDGET
                else if (salbum->isMapSearch())
                {
                    d->gpsSearchView->searchBar()->completionObject()->addItem(salbum->title());
                    d->gpsSearchView->folderView()->slotTextSearchFilterChanged(d->gpsSearchView->searchBar()->searchTextSettings());
                }
#endif

                break;
            }
            default:
            {
                // Nothing to do with Album::DATE
                break;
            }
        }
    }
}

void DigikamView::slotAlbumsCleared()
{
    emit signalAlbumSelected(false);

    d->folderSearchBar->completionObject()->clear();

    d->tagSearchBar->completionObject()->clear();
    d->tagFilterSearchBar->completionObject()->clear();

    d->searchSearchBar->completionObject()->clear();
    d->timeLineView->searchBar()->completionObject()->clear();
    d->fuzzySearchView->searchBar()->completionObject()->clear();
#ifdef HAVE_MARBLEWIDGET
    d->gpsSearchView->searchBar()->completionObject()->clear();
#endif
}

void DigikamView::slotAlbumHistoryBack(int steps)
{
    Album   *album=0;
    QWidget *widget=0;

    d->albumHistory->back(&album, &widget, steps);

    changeAlbumFromHistory(album, widget);
}

void DigikamView::slotAlbumHistoryForward(int steps)
{
    Album   *album=0;
    QWidget *widget=0;

    d->albumHistory->forward(&album, &widget, steps);

    changeAlbumFromHistory(album, widget);
}

void DigikamView::changeAlbumFromHistory(Album *album, QWidget *widget)
{
    if (album && widget)
    {
        Q3ListViewItem *item = 0;

        // Check if widget is a vbox used to host folderview, tagview or searchview.
        if (KVBox *v = dynamic_cast<KVBox*>(widget))
        {
            if (v == d->folderBox)
            {
                item = (Q3ListViewItem*)album->extraData(d->folderView);
                if(!item) return;

                d->folderView->setSelected(item, true);
                d->folderView->ensureItemVisible(item);
            }
            else if (v == d->tagBox)
            {
                item = (Q3ListViewItem*)album->extraData(d->tagFolderView);
                if(!item) return;

                d->tagFolderView->setSelected(item, true);
                d->tagFolderView->ensureItemVisible(item);
            }
            else if (v == d->searchBox)
            {
                item = (Q3ListViewItem*)album->extraData(d->searchFolderView);
                if(!item) return;

                d->searchFolderView->setSelected(item, true);
                d->searchFolderView->ensureItemVisible(item);
            }
        }
        else if (DateFolderView *v = dynamic_cast<DateFolderView*>(widget))
        {
            item = (Q3ListViewItem*)album->extraData(v);
            if(!item) return;
            v->setSelected(item);
        }
        else if (TimeLineView *v = dynamic_cast<TimeLineView*>(widget))
        {
            item = (Q3ListViewItem*)album->extraData(v->folderView());
            if(!item) return;

            v->folderView()->setSelected(item, true);
            v->folderView()->ensureItemVisible(item);
        }
        else if (FuzzySearchView *v = dynamic_cast<FuzzySearchView*>(widget))
        {
            item = (Q3ListViewItem*)album->extraData(v->folderView());
            if(!item) return;

            v->folderView()->setSelected(item, true);
            v->folderView()->ensureItemVisible(item);
        }

        d->leftSideBar->setActiveTab(widget);

        d->parent->enableAlbumBackwardHistory(!d->albumHistory->isBackwardEmpty());
        d->parent->enableAlbumForwardHistory(!d->albumHistory->isForwardEmpty());
    }
}

void DigikamView::clearHistory()
{
    d->albumHistory->clearHistory();
    d->parent->enableAlbumBackwardHistory(false);
    d->parent->enableAlbumForwardHistory(false);
}

void DigikamView::getBackwardHistory(QStringList& titles)
{
    d->albumHistory->getBackwardHistory(titles);
    for (int i=0; i<titles.size(); ++i)
        titles[i] = d->userPresentableAlbumTitle(titles[i]);
}

void DigikamView::getForwardHistory(QStringList& titles)
{
    d->albumHistory->getForwardHistory(titles);
    for (int i=0; i<titles.size(); ++i)
        titles[i] = d->userPresentableAlbumTitle(titles[i]);
}

QString DigikamViewPriv::userPresentableAlbumTitle(const QString& title)
{
    if (title == FuzzySearchFolderView::currentFuzzySketchSearchName())
        return i18n("Fuzzy Sketch Search");
    else if (title == FuzzySearchFolderView::currentFuzzyImageSearchName())
        return i18n("Fuzzy Image Search");
    else if (title == GPSSearchFolderView::currentGPSSearchName())
        return i18n("Map Search");
    else if (title == SearchFolderView::currentSearchViewSearchName())
        return i18n("Last Search");
    else if (title == TimeLineFolderView::currentTimeLineSearchName())
        return i18n("Timeline");
    return title;
}

void DigikamView::slotSelectAlbum(const KUrl &)
{
    /* TODO
    if (url.isEmpty())
        return;

    Album *album = d->albumManager->findPAlbum(url);
    if(album && album->getViewItem())
    {
        AlbumFolderItem_Deprecated *item;
        item = static_cast<AlbumFolderItem_Deprecated*>(album->getViewItem());
        mFolderView_Deprecated->setSelected(item);
        d->parent->enableAlbumBackwardHistory(!d->albumHistory->isBackwardEmpty());
        d->parent->enableAlbumForwardHistory(!d->albumHistory->isForwardEmpty());
    }
    */
}

void DigikamView::slotGotoAlbumAndItem(const ImageInfo& imageInfo)
{
    emit signalNoCurrentItem();

    Album* album = dynamic_cast<Album*>(AlbumManager::instance()->findPAlbum(imageInfo.albumId()));

    // Change the current album in list view.
    d->folderView->setCurrentAlbum(album);

    // Change to (physical) Album view.
    // Note, that this also opens the side bar if it is closed; this is
    // considered as a feature, because it highlights that the view was changed.
    d->leftSideBar->setActiveTab(d->folderBox);

    // Set the activate item url to find in the Album View after
    // all items have be reloaded.
    d->iconView->scrollToWhenAvailable(imageInfo.id());

    // And finally toggle album manager to handle album history and
    // reload all items.
    d->albumManager->setCurrentAlbum(album);
}

void DigikamView::slotGotoDateAndItem(const ImageInfo& imageInfo)
{
    QDate date = imageInfo.dateTime().date();

    emit signalNoCurrentItem();

    // Change to Date Album view.
    // Note, that this also opens the side bar if it is closed; this is
    // considered as a feature, because it highlights that the view was changed.
    d->leftSideBar->setActiveTab(d->dateFolderView);

    // Set the activate item url to find in the Album View after
    // all items have be reloaded.
    d->iconView->scrollToWhenAvailable(imageInfo.id());

    // Change the year and month of the iconItem (day is unused).
    d->dateFolderView->gotoDate(date);
}

void DigikamView::slotGotoTagAndItem(int tagID)
{
    // FIXME: Arnd: don't know yet how to get the iconItem passed through ...
    // FIXME: then we would know how to use the following ...
    //  KURL url( iconItem->imageInfo()->kurl() );
    //  url.cleanPath();

    emit signalNoCurrentItem();

    // Change to Tag Folder view.
    // Note, that this also opens the side bar if it is closed; this is
    // considered as a feature, because it highlights that the view was changed.
    d->leftSideBar->setActiveTab(d->tagBox);

    // Set the current tag in the tag folder view.
    d->tagFolderView->selectItem(tagID);

    // Set the activate item url to find in the Tag View after
    // all items have be reloaded.
    // FIXME: see above
    // d->iconView->setAlbumItemToFind(url);
}

void DigikamView::slotAlbumSelected(Album* album)
{
    emit signalNoCurrentItem();

    if (!album)
    {
        d->iconView->openAlbum(0);
        emit signalAlbumSelected(false);
        emit signalTagSelected(false);
        return;
    }

    if (album->type() == Album::PHYSICAL)
    {
        emit signalAlbumSelected(true);
        emit signalTagSelected(false);
    }
    else if (album->type() == Album::TAG)
    {
        emit signalAlbumSelected(false);
        emit signalTagSelected(true);
    }

    d->albumHistory->addAlbum(album, d->leftSideBar->getActiveTab());
    d->parent->enableAlbumBackwardHistory(!d->albumHistory->isBackwardEmpty());
    d->parent->enableAlbumForwardHistory(!d->albumHistory->isForwardEmpty());

    d->iconView->openAlbum(album);
    if (album->isRoot())
        d->albumWidgetStack->setPreviewMode(AlbumWidgetStack::WelcomePageMode);
    else
        d->albumWidgetStack->setPreviewMode(AlbumWidgetStack::PreviewAlbumMode);
}

void DigikamView::slotAlbumOpenInKonqui()
{
    Album *album = d->albumManager->currentAlbum();
    if (!album || album->type() != Album::PHYSICAL)
        return;

    PAlbum* palbum = dynamic_cast<PAlbum*>(album);

    new KRun(KUrl(palbum->folderPath()), this); // KRun will delete itself.
}

void DigikamView::slotAlbumRefresh()
{
    //TODO: Decide what to do with refresh()
    //d->iconView->refreshItems(d->iconView->allItems());
}

void DigikamView::slotImageSelected()
{
    // delay to slotDispatchImageSelected
    d->needDispatchSelection = true;
    d->selectionTimer->setSingleShot(true);
    d->selectionTimer->start(75);
}

void DigikamView::slotDispatchImageSelected()
{
    if (d->needDispatchSelection)
    {
        // the list of ImageInfos of currently selected items, currentItem first
        ImageInfoList list = d->iconView->selectedImageInfosCurrentFirst();

        ImageInfoList allImages = d->iconView->imageInfos();

        if (list.isEmpty())
        {
            d->albumWidgetStack->setPreviewItem();
            emit signalImageSelected(list, false, false, allImages);
            emit signalNoCurrentItem();
            emit signalSelectionChanged(false);
        }
        else
        {
            d->rightSideBar->itemChanged(list);

            ImageInfo previousInfo = d->iconView->previousInfo(list.first());
            ImageInfo nextInfo = d->iconView->nextInfo(list.first());

            if (!d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
                d->albumWidgetStack->setPreviewItem(list.first(), previousInfo, nextInfo);

            emit signalImageSelected(list, !previousInfo.isNull(), !nextInfo.isNull(), allImages);
            emit signalSelectionChanged(true);
        }

        d->needDispatchSelection = false;
    }
}

void DigikamView::setThumbSize(int size)
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        double h    = (double)ThumbnailSize::Huge;
        double s    = (double)ThumbnailSize::Small;
        double zmin = d->albumWidgetStack->zoomMin();
        double zmax = d->albumWidgetStack->zoomMax();
        double b    = (zmin-(zmax*s/h))/(1-s/h);
        double a    = (zmax-b)/h;
        double z    = a*size+b;
        d->albumWidgetStack->setZoomFactorSnapped(z);
    }
    else if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        if (size > ThumbnailSize::Huge)
            d->thumbSize = ThumbnailSize::Huge;
        else if (size < ThumbnailSize::Small)
            d->thumbSize = ThumbnailSize::Small;
        else
            d->thumbSize = size;

        emit signalThumbSizeChanged(d->thumbSize);

        d->thumbSizeTimer->start();
    }
}

void DigikamView::slotThumbSizeEffect()
{
    emit signalNoCurrentItem();

    d->iconView->setThumbnailSize(d->thumbSize);
    toggleZoomActions();

    AlbumSettings::instance()->setDefaultIconSize(d->thumbSize);
}

void DigikamView::toggleZoomActions()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        d->parent->enableZoomMinusAction(true);
        d->parent->enableZoomPlusAction(true);

        if (d->albumWidgetStack->maxZoom())
            d->parent->enableZoomPlusAction(false);

        if (d->albumWidgetStack->minZoom())
            d->parent->enableZoomMinusAction(false);
    }
    else if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        d->parent->enableZoomMinusAction(true);
        d->parent->enableZoomPlusAction(true);

        if (d->thumbSize >= ThumbnailSize::Huge)
            d->parent->enableZoomPlusAction(false);

        if (d->thumbSize <= ThumbnailSize::Small)
            d->parent->enableZoomMinusAction(false);
    }
}

void DigikamView::slotZoomIn()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        setThumbSize(d->thumbSize + ThumbnailSize::Step);
        toggleZoomActions();
        emit signalThumbSizeChanged(d->thumbSize);
    }
    else if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        d->albumWidgetStack->increaseZoom();
    }
}

void DigikamView::slotZoomOut()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        setThumbSize(d->thumbSize - ThumbnailSize::Step);
        toggleZoomActions();
        emit signalThumbSizeChanged(d->thumbSize);
    }
    else if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        d->albumWidgetStack->decreaseZoom();
    }
}

void DigikamView::slotZoomTo100Percents()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        d->albumWidgetStack->toggleFitToWindowOr100();
    }
}

void DigikamView::slotFitToWindow()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
    {
        d->albumWidgetStack->fitToWindow();
    }
}

void DigikamView::slotZoomFactorChanged(double zoom)
{
    toggleZoomActions();

    double h    = (double)ThumbnailSize::Huge;
    double s    = (double)ThumbnailSize::Small;
    double zmin = d->albumWidgetStack->zoomMin();
    double zmax = d->albumWidgetStack->zoomMax();
    double b    = (zmin-(zmax*s/h))/(1-s/h);
    double a    = (zmax-b)/h;
    int size    = (int)((zoom - b) /a);

    emit signalZoomChanged(zoom, size);
}

void DigikamView::slotAlbumPropsEdit()
{
    d->folderView->albumEdit();
}

void DigikamView::slotAlbumSyncPicturesMetadata()
{
    Album *album = d->albumManager->currentAlbum();
    if (!album)
        return;

    BatchSyncMetadata *syncMetadata = new BatchSyncMetadata(this, album);

    connect(syncMetadata, SIGNAL(signalProgressBarMode(int, const QString&)),
            d->parent, SLOT(slotProgressBarMode(int, const QString&)));

    connect(syncMetadata, SIGNAL(signalProgressValue(int)),
            d->parent, SLOT(slotProgressValue(int)));

    connect(syncMetadata, SIGNAL(signalComplete()),
            this, SLOT(slotAlbumSyncPicturesMetadataDone()));

    connect(d->parent, SIGNAL(signalCancelButtonPressed()),
            syncMetadata, SLOT(slotAbort()));

    syncMetadata->parseAlbum();
}

void DigikamView::slotAlbumSyncPicturesMetadataDone()
{
    applySettings();
}

// ----------------------------------------------------------------

void DigikamView::slotEscapePreview()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode ||
        d->albumWidgetStack->previewMode() == AlbumWidgetStack::WelcomePageMode)
        return;

    slotTogglePreviewMode(d->iconView->currentInfo());
}

void DigikamView::slotImagePreview()
{
    ImageInfo info = d->iconView->currentInfo();
    if (!info.isNull())
        slotTogglePreviewMode(info);
}

// This method toggle between AlbumView and ImagePreview Modes, depending of context.
void DigikamView::slotTogglePreviewMode(const ImageInfo &info)
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode && !info.isNull())
    {
        d->albumWidgetStack->setPreviewItem(info, d->iconView->previousInfo(info), d->iconView->nextInfo(info));
    }
    else
    {
        // We go back to AlbumView Mode.
        d->albumWidgetStack->setPreviewMode( AlbumWidgetStack::PreviewAlbumMode );
    }
}

void DigikamView::slotToggledToPreviewMode(bool b)
{
    toggleZoomActions();

    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
        emit signalThumbSizeChanged(d->iconView->thumbnailSize().size());
    else if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewImageMode)
        slotZoomFactorChanged(d->albumWidgetStack->zoomFactor());

    emit signalTogglePreview(b);
}

void DigikamView::slotImageEdit()
{
    d->iconView->openCurrentInEditor();
}

void DigikamView::slotImageFindSimilar()
{
    ImageInfo current = d->iconView->currentInfo();
    if (!current.isNull())
    {
        d->fuzzySearchView->setImageInfo(current);

        if (d->leftSideBar->getActiveTab() != d->fuzzySearchView)
            d->leftSideBar->setActiveTab(d->fuzzySearchView);
    }
}

void DigikamView::slotImageExifOrientation(int orientation)
{
    d->iconView->setExifOrientationOfSelected(orientation);
}

void DigikamView::slotLightTable()
{
    d->iconView->utilities()->insertToLightTable(ImageInfoList(), ImageInfo(), true);
}

void DigikamView::slotQueueMgr()
{
    d->iconView->utilities()->insertToQueueManager(ImageInfoList(), ImageInfo(), false);
}

void DigikamView::slotImageLightTable()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        // put images into an emptied light table
        d->iconView->insertSelectedToLightTable(false);
    }
    else
    {
        ImageInfoList list;
        ImageInfo info = d->albumWidgetStack->imagePreviewView()->getImageInfo();
        list.append(info);
        // put images into an emptied light table
        d->iconView->utilities()->insertToLightTable(list, info, false);
    }
}

void DigikamView::slotImageAddToLightTable()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        // add images to the existing images in the light table
        d->iconView->insertSelectedToLightTable(true);
    }
    else
    {
        ImageInfoList list;
        ImageInfo info = d->albumWidgetStack->imagePreviewView()->getImageInfo();
        list.append(info);
        // add images to the existing images in the light table
        d->iconView->utilities()->insertToLightTable(list, info, true);
    }
}

void DigikamView::slotImageAddToCurrentQueue()
{
    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        d->iconView->insertSelectedToCurrentQueue();
    }
    else
    {
        ImageInfoList list;
        ImageInfo info = d->albumWidgetStack->imagePreviewView()->getImageInfo();
        list.append(info);
        d->iconView->utilities()->insertToQueueManager(list, info, false);
    }
}

void DigikamView::slotImageAddToNewQueue()
{
    bool newQueue = QueueMgrWindow::queueManagerWindowCreated() &&
                   !QueueMgrWindow::queueManagerWindow()->queuesMap().isEmpty();

    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
    {
        if (newQueue)
            d->iconView->insertSelectedToNewQueue();
        else
            d->iconView->insertSelectedToCurrentQueue();
    }
    else
    {
        //FIXME.
        ImageInfoList list;
        ImageInfo info = d->albumWidgetStack->imagePreviewView()->getImageInfo();
        list.append(info);
        d->iconView->utilities()->insertToQueueManager(list, info, newQueue);
    }
}

void DigikamView::slotImageAddToExistingQueue(int queueid)
{
    ImageInfoList list;

    if (d->albumWidgetStack->previewMode() == AlbumWidgetStack::PreviewAlbumMode)
        list = d->albumWidgetStack->imageIconView()->selectedImageInfos();
    else
        list << d->albumWidgetStack->imagePreviewView()->getImageInfo();

    d->iconView->utilities()->insertSilentToQueueManager(list, list.first(), queueid);
}

void DigikamView::slotImageRename()
{
    d->iconView->renameCurrent();
}

void DigikamView::slotImageDelete()
{
    d->iconView->deleteSelected(false);
}

void DigikamView::slotImageDeletePermanently()
{
    d->iconView->deleteSelected(true);
}

void DigikamView::slotImageDeletePermanentlyDirectly()
{
    d->iconView->deleteSelectedDirectly(false);
}

void DigikamView::slotImageTrashDirectly()
{
    d->iconView->deleteSelectedDirectly(true);
}

void DigikamView::slotSelectAll()
{
    d->iconView->selectAll();
}

void DigikamView::slotSelectNone()
{
    d->iconView->clearSelection();
}

void DigikamView::slotSelectInvert()
{
    d->iconView->invertSelection();
}

void DigikamView::slotSortImages(int order)
{
    //TODO!!!
    AlbumSettings* settings = AlbumSettings::instance();
    if (!settings)
        return;
    settings->setImageSortOrder((AlbumSettings::ImageSortOrder) order);
}

void DigikamView::slotLeftSidebarChangedTab(QWidget* w)
{
    // setActive means that selection changes are propagated, nothing more.
    // Additionally, when it is set to true, the selectionChanged signal will be emitted.
    // So this is the place which causes the behavior that when the left sidebar
    // tab is changed, the current album is changed as well.
    d->dateFolderView->setActive(w == d->dateFolderView);
    d->folderView->setActive(w == d->folderBox);
    d->tagFolderView->setActive(w == d->tagBox);
    d->searchFolderView->setActive(w == d->searchBox);
    d->timeLineView->setActive(w == d->timeLineView);
    d->fuzzySearchView->setActive(w == d->fuzzySearchView);
#ifdef HAVE_MARBLEWIDGET
    d->gpsSearchView->setActive(w == d->gpsSearchView);
#endif
}

void DigikamView::slotAssignRating(int rating)
{
    d->iconView->assignRatingToSelected(rating);
}

void DigikamView::slotAssignRatingNoStar()
{
    d->iconView->assignRatingToSelected(0);
}

void DigikamView::slotAssignRatingOneStar()
{
    d->iconView->assignRatingToSelected(1);
}

void DigikamView::slotAssignRatingTwoStar()
{
    d->iconView->assignRatingToSelected(2);
}

void DigikamView::slotAssignRatingThreeStar()
{
    d->iconView->assignRatingToSelected(3);
}

void DigikamView::slotAssignRatingFourStar()
{
    d->iconView->assignRatingToSelected(4);
}

void DigikamView::slotAssignRatingFiveStar()
{
    d->iconView->assignRatingToSelected(5);
}

void DigikamView::slotSlideShowAll()
{
    slideShow(d->iconView->imageInfos());
}

void DigikamView::slotSlideShowSelection()
{
    slideShow(d->iconView->selectedImageInfos());
}

void DigikamView::slotSlideShowRecursive()
{
    Album *album = AlbumManager::instance()->currentAlbum();
    if(album)
    {
        AlbumList albumList;
        albumList.append(album);
        AlbumIterator it(album);
        while (it.current())
        {
            albumList.append(*it);
            ++it;
        }

        ImageInfoAlbumsJob *job = new ImageInfoAlbumsJob;
        connect(job, SIGNAL(signalCompleted(const ImageInfoList&)),
                this, SLOT(slotItemsInfoFromAlbums(const ImageInfoList&)));
        job->allItemsFromAlbums(albumList);
    }
}

void DigikamView::slotItemsInfoFromAlbums(const ImageInfoList& infoList)
{
    ImageInfoList list = infoList;
    slideShow(list);
}

void DigikamView::slideShow(const ImageInfoList& infoList)
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("ImageViewer Settings");
    bool startWithCurrent     = group.readEntry("SlideShowStartCurrent", false);

    int     i = 0;
    float cnt = (float)infoList.count();
    emit signalProgressBarMode(StatusProgressBar::CancelProgressBarMode,
                               i18np("Preparing slideshow of 1 image. Please wait...","Preparing slideshow of %1 images. Please wait...", infoList.count()));

    SlideShowSettings settings;
    settings.exifRotate           = AlbumSettings::instance()->getExifRotate();
    settings.ratingColor          = ThemeEngine::instance()->textSpecialRegColor();
    settings.delay                = group.readEntry("SlideShowDelay", 5) * 1000;
    settings.printName            = group.readEntry("SlideShowPrintName", true);
    settings.printDate            = group.readEntry("SlideShowPrintDate", false);
    settings.printApertureFocal   = group.readEntry("SlideShowPrintApertureFocal", false);
    settings.printExpoSensitivity = group.readEntry("SlideShowPrintExpoSensitivity", false);
    settings.printMakeModel       = group.readEntry("SlideShowPrintMakeModel", false);
    settings.printComment         = group.readEntry("SlideShowPrintComment", false);
    settings.printRating          = group.readEntry("SlideShowPrintRating", false);
    settings.loop                 = group.readEntry("SlideShowLoop", false);

    d->cancelSlideShow = false;
    for (ImageInfoList::const_iterator it = infoList.constBegin();
         !d->cancelSlideShow && (it != infoList.constEnd()) ; ++it)
    {
        ImageInfo info = *it;
        settings.fileList.append(info.fileUrl());
        SlidePictureInfo pictInfo;
        pictInfo.comment   = info.comment();
        pictInfo.rating    = info.rating();
        pictInfo.photoInfo = info.photoInfoContainer();
        settings.pictInfoMap.insert(info.fileUrl(), pictInfo);

        emit signalProgressValue((int)((i++/cnt)*100.0));
        kapp->processEvents();
    }

    emit signalProgressBarMode(StatusProgressBar::TextMode, QString());

    if (!d->cancelSlideShow)
    {
        SlideShow *slide = new SlideShow(settings);
        if (startWithCurrent)
        {
            slide->setCurrent(d->iconView->currentUrl());
        }

        slide->show();
    }
}

void DigikamView::slotCancelSlideShow()
{
    d->cancelSlideShow = true;
}

void DigikamView::toggleShowBar(bool b)
{
    d->albumWidgetStack->toggleShowBar(b);
}

void DigikamView::slotSidebarTabTitleStyleChanged()
{
    d->leftSideBar->setStyle(AlbumSettings::instance()->getSidebarTitleStyle());
    d->rightSideBar->setStyle(AlbumSettings::instance()->getSidebarTitleStyle());
}

}  // namespace Digikam
