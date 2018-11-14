/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-24
 * Description : Qt item view for images
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * This program is free software you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "digikamimageview.h"
#include "digikamimageview_p.h"

// Qt includes

#include <QApplication>
#include <QPointer>
#include <QMenu>
#include <QIcon>
#include <QAction>
#include <QUrl>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "coredb.h"
#include "coredboperationgroup.h"
#include "advancedrenamedialog.h"
#include "advancedrenameprocessdialog.h"
#include "applicationsettings.h"
#include "assignnameoverlay.h"
#include "contextmenuhelper.h"
#include "coredbaccess.h"
#include "ddragobjects.h"
#include "digikamapp.h"
#include "digikamimagedelegate.h"
#include "digikamimagefacedelegate.h"
#include "dio.h"
#include "groupindicatoroverlay.h"
#include "itemalbumfiltermodel.h"
#include "itemalbummodel.h"
#include "itemdragdrop.h"
#include "imageratingoverlay.h"
#include "imagefsoverlay.h"
#include "imagecoordinatesoverlay.h"
#include "tagslineeditoverlay.h"
#include "imageviewutilities.h"
#include "imagewindow.h"
#include "fileactionmngr.h"
#include "fileactionprogress.h"
#include "thumbnailloadthread.h"
#include "tagregion.h"
#include "addtagslineedit.h"
#include "facerejectionoverlay.h"
#include "facetagsiface.h"

namespace Digikam
{

DigikamImageView::DigikamImageView(QWidget* const parent)
    : ImageCategorizedView(parent),
      d(new Private(this))
{
    installDefaultModels();

    d->editPipeline.plugDatabaseEditor();
    d->editPipeline.plugTrainer();
    d->editPipeline.construct();

    connect(&d->editPipeline, SIGNAL(scheduled()),
            this, SLOT(slotInitProgressIndicator()));

    d->normalDelegate = new DigikamImageDelegate(this);
    d->faceDelegate   = new DigikamImageFaceDelegate(this);

    setItemDelegate(d->normalDelegate);
    setSpacing(10);

    ApplicationSettings* const settings = ApplicationSettings::instance();

    imageFilterModel()->setCategorizationMode(ItemSortSettings::CategoryByAlbum);

    imageAlbumModel()->setThumbnailLoadThread(ThumbnailLoadThread::defaultIconViewThread());
    setThumbnailSize(ThumbnailSize(settings->getDefaultIconSize()));
    imageAlbumModel()->setPreloadThumbnails(true);

    imageModel()->setDragDropHandler(new ItemDragDropHandler(imageModel()));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(false);

    setToolTipEnabled(settings->showToolTipsIsValid());
    imageFilterModel()->setStringTypeNatural(settings->isStringTypeNatural());
    imageFilterModel()->setSortRole((ItemSortSettings::SortRole)settings->getImageSortOrder());
    imageFilterModel()->setSortOrder((ItemSortSettings::SortOrder)settings->getImageSorting());
    imageFilterModel()->setCategorizationMode((ItemSortSettings::CategorizationMode)settings->getImageSeparationMode());
    imageFilterModel()->setCategorizationSortOrder((ItemSortSettings::SortOrder) settings->getImageSeparationSortOrder());

    // selection overlay
    addSelectionOverlay(d->normalDelegate);
    addSelectionOverlay(d->faceDelegate);

    // rotation overlays
    d->rotateLeftOverlay  = ImageRotateOverlay::left(this);
    d->rotateRightOverlay = ImageRotateOverlay::right(this);
    d->fullscreenOverlay  = ImageFsOverlay::instance(this);
    d->updateOverlays();

    // rating overlay
    ImageRatingOverlay* const ratingOverlay = new ImageRatingOverlay(this);
    addOverlay(ratingOverlay);

    // face overlays
    // NOTE: order to plug this overlay is important, else rejection cant be suitable (see bug #324759).
    addAssignNameOverlay(d->faceDelegate);
    addRejectionOverlay(d->faceDelegate);

    GroupIndicatorOverlay* const groupOverlay = new GroupIndicatorOverlay(this);
    addOverlay(groupOverlay);

    addOverlay(new ImageCoordinatesOverlay(this));

    connect(ratingOverlay, SIGNAL(ratingEdited(QList<QModelIndex>,int)),
            this, SLOT(assignRating(QList<QModelIndex>,int)));

    connect(groupOverlay, SIGNAL(toggleGroupOpen(QModelIndex)),
            this, SLOT(groupIndicatorClicked(QModelIndex)));

    connect(groupOverlay, SIGNAL(showButtonContextMenu(QModelIndex,QContextMenuEvent*)),
            this, SLOT(showGroupContextMenu(QModelIndex,QContextMenuEvent*)));

    d->utilities = new ImageViewUtilities(this);

    connect(imageModel()->dragDropHandler(), SIGNAL(assignTags(QList<ItemInfo>,QList<int>)),
            FileActionMngr::instance(), SLOT(assignTags(QList<ItemInfo>,QList<int>)));

    connect(imageModel()->dragDropHandler(), SIGNAL(addToGroup(ItemInfo,QList<ItemInfo>)),
            FileActionMngr::instance(), SLOT(addToGroup(ItemInfo,QList<ItemInfo>)));

    connect(imageModel()->dragDropHandler(), SIGNAL(dragDropSort(ItemInfo,QList<ItemInfo>)),
            this, SLOT(dragDropSort(ItemInfo,QList<ItemInfo>)));

    connect(d->utilities, SIGNAL(editorCurrentUrlChanged(QUrl)),
            this, SLOT(setCurrentUrlWhenAvailable(QUrl)));

    connect(settings, SIGNAL(setupChanged()),
            this, SLOT(slotSetupChanged()));

    slotSetupChanged();
}

DigikamImageView::~DigikamImageView()
{
    delete d;
}

ImageViewUtilities* DigikamImageView::utilities() const
{
    return d->utilities;
}

void DigikamImageView::setThumbnailSize(const ThumbnailSize& size)
{
    imageThumbnailModel()->setPreloadThumbnailSize(size);
    ImageCategorizedView::setThumbnailSize(size);
}

ItemInfoList DigikamImageView::allItemInfos(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ImageCategorizedView::allItemInfos());
    }

    return ImageCategorizedView::allItemInfos();
}

ItemInfoList DigikamImageView::selectedItemInfos(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ImageCategorizedView::selectedItemInfos());
    }

    return ImageCategorizedView::selectedItemInfos();
}

ItemInfoList DigikamImageView::selectedItemInfosCurrentFirst(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ImageCategorizedView::selectedItemInfosCurrentFirst());
    }

    return ImageCategorizedView::selectedItemInfosCurrentFirst();
}

void DigikamImageView::dragDropSort(const ItemInfo& pick, const QList<ItemInfo>& infos)
{
    if (pick.isNull() || infos.isEmpty())
    {
        return;
    }

    ItemInfoList infoList = allItemInfos(false);
    qlonglong counter      = pick.manualOrder();
    bool order             = (ApplicationSettings::instance()->
                                getImageSorting() == Qt::AscendingOrder);
    bool found             = false;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    CoreDbOperationGroup group;
    group.setMaximumTime(200);

    foreach (ItemInfo info, infoList)
    {
        if (!found && info.id() == pick.id())
        {
            foreach (ItemInfo dropInfo, infos)
            {
                dropInfo.setManualOrder(counter);
                counter += (order ? 1 : -1);
            }

            info.setManualOrder(counter);
            found = true;
        }
        else if (found && !infos.contains(info))
        {
            if (( order && info.manualOrder() > counter) ||
                (!order && info.manualOrder() < counter))
            {
                break;
            }

            counter += (order ? 100 : -100);
            info.setManualOrder(counter);
        }

        group.allowLift();
    }

    QApplication::restoreOverrideCursor();

    imageFilterModel()->invalidate();
 }

bool DigikamImageView::allNeedGroupResolving(const ApplicationSettings::OperationType type) const
{
    return needGroupResolving(type, allItemInfos());
}

bool DigikamImageView::selectedNeedGroupResolving(const ApplicationSettings::OperationType type) const
{
    return needGroupResolving(type, selectedItemInfos());
}

int DigikamImageView::fitToWidthIcons()
{
    return delegate()->calculatethumbSizeToFit(viewport()->size().width());
}

void DigikamImageView::slotSetupChanged()
{
    imageFilterModel()->setStringTypeNatural(ApplicationSettings::instance()->isStringTypeNatural());
    setToolTipEnabled(ApplicationSettings::instance()->showToolTipsIsValid());
    setFont(ApplicationSettings::instance()->getIconViewFont());

    d->updateOverlays();

    ImageCategorizedView::slotSetupChanged();
}

bool DigikamImageView::hasHiddenGroupedImages(const ItemInfo& info) const
{
    return info.hasGroupedImages() && !imageFilterModel()->isGroupOpen(info.id());
}

ItemInfoList DigikamImageView::imageInfos(const QList<QModelIndex>& indexes,
                                           ApplicationSettings::OperationType type) const
{
    ItemInfoList infos = ImageCategorizedView::imageInfos(indexes);

    if (needGroupResolving(type, infos))
    {
        return resolveGrouping(infos);
    }

    return infos;
}

void DigikamImageView::setFaceMode(bool on)
{
    d->faceMode = on;

    if (on)
    {
        // See ItemLister, which creates a search the implements listing tag in the ioslave
        imageAlbumModel()->setSpecialTagListing(QLatin1String("faces"));
        setItemDelegate(d->faceDelegate);
        // grouping is not very much compatible with faces
        imageFilterModel()->setAllGroupsOpen(true);
    }
    else
    {
        imageAlbumModel()->setSpecialTagListing(QString());
        setItemDelegate(d->normalDelegate);
        imageFilterModel()->setAllGroupsOpen(false);
    }
}

void DigikamImageView::addRejectionOverlay(ImageDelegate* delegate)
{
    FaceRejectionOverlay* const rejectionOverlay = new FaceRejectionOverlay(this);

    connect(rejectionOverlay, SIGNAL(rejectFaces(QList<QModelIndex>)),
            this, SLOT(removeFaces(QList<QModelIndex>)));

    addOverlay(rejectionOverlay, delegate);
}

/*
void DigikamImageView::addTagEditOverlay(ImageDelegate* delegate)
{
    TagsLineEditOverlay* tagOverlay = new TagsLineEditOverlay(this);

    connect(tagOverlay, SIGNAL(tagEdited(QModelIndex,QString)),
            this, SLOT(assignTag(QModelIndex,QString)));

    addOverlay(tagOverlay, delegate);
}
*/

void DigikamImageView::addAssignNameOverlay(ImageDelegate* delegate)
{
    AssignNameOverlay* const nameOverlay = new AssignNameOverlay(this);
    addOverlay(nameOverlay, delegate);

    connect(nameOverlay, SIGNAL(confirmFaces(QList<QModelIndex>,int)),
            this, SLOT(confirmFaces(QList<QModelIndex>,int)));

    connect(nameOverlay, SIGNAL(removeFaces(QList<QModelIndex>)),
            this, SLOT(removeFaces(QList<QModelIndex>)));
}

void DigikamImageView::confirmFaces(const QList<QModelIndex>& indexes, int tagId)
{
    QList<ItemInfo>    infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex>  sourceIndexes;

    // fast-remove in the "unknown person" view

    bool needFastRemove = false;

    if (imageAlbumModel()->currentAlbums().size() == 1)
    {
        needFastRemove = d->faceMode && (tagId != imageAlbumModel()->currentAlbums().first()->id());
    }

    foreach (const QModelIndex& index, indexes)
    {
        infos << ItemModel::retrieveItemInfo(index);
        faces << d->faceDelegate->face(index);

        if (needFastRemove)
        {
            sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
        }
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    for (int i = 0 ; i < infos.size() ; i++)
    {
        d->editPipeline.confirm(infos[i], faces[i], tagId);
    }
}

void DigikamImageView::removeFaces(const QList<QModelIndex>& indexes)
{
    QList<ItemInfo> infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex> sourceIndexes;

    foreach (const QModelIndex& index, indexes)
    {
        infos << ItemModel::retrieveItemInfo(index);
        faces << d->faceDelegate->face(index);
        sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    for (int i = 0 ; i < infos.size() ; i++)
    {
        d->editPipeline.remove(infos[i], faces[i]);
    }
}

void DigikamImageView::activated(const ItemInfo& info, Qt::KeyboardModifiers modifiers)
{
    if (info.isNull())
    {
        return;
    }

    if (modifiers != Qt::MetaModifier)
    {
        if (ApplicationSettings::instance()->getItemLeftClickAction() == ApplicationSettings::ShowPreview)
        {
            emit previewRequested(info);
        }
        else
        {
            openFile(info);
        }
    }
    else
    {
        d->utilities->openInfosWithDefaultApplication(QList<ItemInfo>() << info);
    }
}

void DigikamImageView::showContextMenuOnInfo(QContextMenuEvent* event, const ItemInfo& info)
{
    emit signalShowContextMenuOnInfo(event, info, QList<QAction*>(), imageFilterModel());
}

void DigikamImageView::showGroupContextMenu(const QModelIndex& index, QContextMenuEvent* event)
{
    Q_UNUSED(index);
    emit signalShowGroupContextMenu(event, selectedItemInfosCurrentFirst(), imageFilterModel());
}

void DigikamImageView::showContextMenu(QContextMenuEvent* event)
{
    emit signalShowContextMenu(event);
}

void DigikamImageView::openFile(const ItemInfo& info)
{
    d->utilities->openInfos(info, allItemInfos(), currentAlbum());
}

void DigikamImageView::deleteSelected(const ImageViewUtilities::DeleteMode deleteMode)
{
    ItemInfoList imageInfoList = selectedItemInfos(true);

    if (d->utilities->deleteImages(imageInfoList, deleteMode))
    {
        awayFromSelection();
    }
}

void DigikamImageView::deleteSelectedDirectly(const ImageViewUtilities::DeleteMode deleteMode)
{
    ItemInfoList imageInfoList = selectedItemInfos(true);

    d->utilities->deleteImagesDirectly(imageInfoList, deleteMode);
    awayFromSelection();
}

void DigikamImageView::assignRating(const QList<QModelIndex>& indexes, int rating)
{
    ItemInfoList infos = imageInfos(indexes, ApplicationSettings::Metadata);
    FileActionMngr::instance()->assignRating(infos, rating);
}

void DigikamImageView::groupIndicatorClicked(const QModelIndex& index)
{
    ItemInfo info = imageFilterModel()->imageInfo(index);

    if (info.isNull())
    {
        return;
    }

    setCurrentIndex(index);
    imageFilterModel()->toggleGroupOpen(info.id());
    imageAlbumModel()->ensureHasGroupedImages(info);
}

void DigikamImageView::rename()
{
    ItemInfoList infos = selectedItemInfos();

    if (needGroupResolving(ApplicationSettings::Rename, infos))
    {
        infos = resolveGrouping(infos);
    }

    QList<QUrl> urls = infos.toImageUrlList();
    bool loop        = false;
    NewNamesList newNamesList;

    do
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Selected URLs to rename: " << urls;

        QPointer<AdvancedRenameDialog> dlg = new AdvancedRenameDialog(this);
        dlg->slotAddImages(urls);

        if (dlg->exec() != QDialog::Accepted)
        {
            delete dlg;
            break;
        }

        if (!loop)
        {
            QUrl nextUrl = nextInOrder(infos.last(), 1).fileUrl();
            setCurrentUrl(nextUrl);
            loop = true;
        }

        newNamesList = dlg->newNames();
        delete dlg;

        setFocus();
        qApp->processEvents();

        if (!newNamesList.isEmpty())
        {
            QPointer<AdvancedRenameProcessDialog> dlg = new AdvancedRenameProcessDialog(newNamesList, this);
            dlg->exec();

            imageFilterModel()->invalidate();
            urls = dlg->failedUrls();
            delete dlg;
        }
    }
    while (!urls.isEmpty() && !newNamesList.isEmpty());
}

void DigikamImageView::slotRotateLeft(const QList<QModelIndex>& indexes)
{
    ItemInfoList infos = imageInfos(indexes, ApplicationSettings::Metadata);
    FileActionMngr::instance()->transform(infos, MetaEngineRotation::Rotate270);
}

void DigikamImageView::slotRotateRight(const QList<QModelIndex>& indexes)
{
    ItemInfoList infos = imageInfos(indexes, ApplicationSettings::Metadata);
    FileActionMngr::instance()->transform(infos, MetaEngineRotation::Rotate90);
}

void DigikamImageView::slotFullscreen(const QList<QModelIndex>& indexes)
{
   QList<ItemInfo> infos = imageInfos(indexes, ApplicationSettings::Slideshow);

   if (infos.isEmpty())
   {
        return;
   }

   // Just fullscreen the first.
   const ItemInfo& info = infos.at(0);

   emit fullscreenRequested(info);
}

void DigikamImageView::slotInitProgressIndicator()
{
    if (!ProgressManager::instance()->findItembyId(QLatin1String("FaceActionProgress")))
    {
        FileActionProgress* const item = new FileActionProgress(QLatin1String("FaceActionProgress"));

        connect(&d->editPipeline, SIGNAL(started(QString)),
                item, SLOT(slotProgressStatus(QString)));

        connect(&d->editPipeline, SIGNAL(progressValueChanged(float)),
                item, SLOT(slotProgressValue(float)));

        connect(&d->editPipeline, SIGNAL(finished()),
                item, SLOT(slotCompleted()));
    }
}

} // namespace Digikam
