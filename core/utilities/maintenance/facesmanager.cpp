/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-18
 * Description : batch face detection
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "facesmanager.h"

// Qt includes

#include <QClipboard>
#include <QVBoxLayout>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QApplication>
#include <QTextEdit>
#include <QHash>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "coredb.h"
#include "album.h"
#include "albummanager.h"
#include "albumpointer.h"
#include "detectionpipeline.h"
#include "facescansettings.h"
#include "iteminfo.h"
#include "iteminfojob.h"
#include "facetags.h"

namespace Digikam
{
class Q_DECL_HIDDEN FacesManager::Private
{
public:

    explicit Private()
        : source (FacesManager::Albums)
    {
    }

    ~Private()
    {
        delete pipeline;
    }

    FacesManager::InputSource source;

    AlbumPointerList<>         albumTodoList;
    ItemInfoList               infoTodoList;
    //QList<qlonglong>           idsTodoList;

    ItemInfoJob                albumListing;
    DetectionPipeline*         pipeline;
    //FacePipeline               pipeline;
};

FacesManager::FacesManager(const FaceScanSettings& settings, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("FacesManager"), parent),
      d              (new Private)
{
    setLabel(i18n("Updating faces database."));
    ProgressManager::addProgressItem(this);

    if ((settings.task == FaceScanSettings::DetectAndRecognize) ||
        (settings.task == FaceScanSettings::Detect))
    {
        bool scanAll = false;
        bool overWrite = false;

        if (settings.alreadyScannedHandling == FaceScanSettings::Rescan)
        {
            scanAll = true;
            overWrite = true;
        }
        else if (settings.alreadyScannedHandling == FaceScanSettings::Merge)
        {
            scanAll = true;
        }

        d->pipeline = new DetectionPipeline(scanAll, overWrite);
    }


    /*
    else if      (settings.task == FaceScanSettings::RetrainAll)
    {
        // clear all training data in the database
        FacialRecognitionWrapper().clearAllTraining(QLatin1String("digikam"));
        d->pipeline.plugRetrainingDatabaseFilter();
        d->pipeline.plugTrainer();
        d->pipeline.construct();
    }
    else // FaceScanSettings::RecognizeMarkedFaces
    {
        d->pipeline.plugRerecognizingDatabaseFilter();
        d->pipeline.plugFaceRecognizer();
        d->pipeline.plugDatabaseWriter(FacePipeline::NormalWrite);
        d->pipeline.setAccuracyAndModel(settings.accuracy,
                                        settings.useYoloV3);
        d->pipeline.construct();
    }
    */

    connect(&d->albumListing, SIGNAL(signalItemsInfo(ItemInfoList)),
            this, SLOT(slotItemsInfo(ItemInfoList)));

    connect(&d->albumListing, SIGNAL(signalCompleted()),
            this, SLOT(slotContinueAlbumListing()));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotCancel()));

    connect(d->pipeline, SIGNAL(processed(int)),
            this, SLOT(slotAdvance(int)));

    /*
    TODO facesengine: Ids is for recognition
    if      (settings.task == FaceScanSettings::RecognizeMarkedFaces)
    {
        d->idsTodoList = CoreDbAccess().db()->
            getImagesWithImageTagProperty(FaceTags::unknownPersonTagId(),
                                          ImageTagPropertyName::autodetectedFace());

        d->source = FacesManager::Ids;
    }
    else if (settings.task == FaceScanSettings::RetrainAll)
    {
        d->idsTodoList = CoreDbAccess().db()->
            getImagesWithProperty(ImageTagPropertyName::tagRegion());

        d->source = FacesManager::Ids;
    }
    
    else*/ if (settings.albums.isEmpty() && settings.infos.isEmpty())
    {
        d->albumTodoList = AlbumManager::instance()->allPAlbums();
        d->source = FacesManager::Albums;
    }
    else if (!settings.albums.isEmpty())
    {
        d->albumTodoList = settings.albums;
        d->source = FacesManager::Albums;
    }
    else
    {
        d->infoTodoList = settings.infos;
        d->source = FacesManager::Infos;
    }
}

FacesManager::~FacesManager()
{
    qDebug() << "FacesManager destroyed";
    delete d;
}

void FacesManager::slotStart()
{
    MaintenanceTool::slotStart();
    d->pipeline->start();

    setThumbnail(QIcon::fromTheme(QLatin1String("edit-image-face-show")).pixmap(22));
    
    if      (d->source == FacesManager::Infos)
    {
        int total = d->infoTodoList.count();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

        setTotalItems(total);

        if (d->infoTodoList.isEmpty())
        {
            slotDone();
            return;
        }

        slotItemsInfo(d->infoTodoList);
        return;
    }

    /*
    TODO facesengine: Only for recognition
    else if (d->source == FacesManager::Ids)
    {
        ItemInfoList itemInfos(d->idsTodoList);

        int total = itemInfos.count();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

        setTotalItems(total);

        if (itemInfos.isEmpty())
        {
            slotDone();
            return;
        }

        slotItemsInfo(itemInfos);
        return;
    }
    */

    setUsesBusyIndicator(true);

    // get total count, cached by AlbumManager

    QMap<int, int> palbumCounts;
    QMap<int, int> talbumCounts;
    bool hasPAlbums = false;
    bool hasTAlbums = false;

    foreach (Album* const album, d->albumTodoList)
    {
        if (album->type() == Album::PHYSICAL)
        {
            hasPAlbums = true;
        }
        else
        {
            hasTAlbums = true;
        }
    }

    palbumCounts = AlbumManager::instance()->getPAlbumsCount();
    talbumCounts = AlbumManager::instance()->getTAlbumsCount();

    if (palbumCounts.isEmpty() && hasPAlbums)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        palbumCounts = CoreDbAccess().db()->getNumberOfImagesInAlbums();
        QApplication::restoreOverrideCursor();
    }

    if (talbumCounts.isEmpty() && hasTAlbums)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        talbumCounts = CoreDbAccess().db()->getNumberOfImagesInTags();
        QApplication::restoreOverrideCursor();
    }

    // first, we use the progressValueMap map to store absolute counts

    QHash<Album*, int> progressValueMap;

    foreach (Album* const album, d->albumTodoList)
    {
        if (album->type() == Album::PHYSICAL)
        {
            progressValueMap[album] = palbumCounts.value(album->id());
        }
        else
        {
            // this is possibly broken of course because we do not know if images have multiple tags,
            // but there's no better solution without expensive operation

            progressValueMap[album] = talbumCounts.value(album->id());
        }
    }

    // second, calculate (approximate) overall sum

    int total = 0;

    foreach (int count, progressValueMap)
    {
        total += count;
    }

    total = qMax(1, total);
    qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

    setUsesBusyIndicator(false);
    setTotalItems(total);

    slotContinueAlbumListing();
}

void FacesManager::slotContinueAlbumListing()
{
    qDebug() << "slotContinueAlbumListing";

    if (d->source != FacesManager::Albums)
    {
        slotDone();
        return;
    }

    qDebug() << d->albumListing.isRunning();

    // list can have null pointer if album was deleted recently

    Album* album = nullptr;

    do
    {
        if (d->albumTodoList.isEmpty())
        {
            slotDone();
            return;
        }

        album = d->albumTodoList.takeFirst();
    }
    while (!album);

    qDebug() << "Scanning" << album->id();

    d->albumListing.allItemsFromAlbum(album);
}

void FacesManager::slotItemsInfo(const ItemInfoList& items)
{
    qDebug() << "Process" << items.size();
    d->pipeline->process(items);
}

// TODO facesengine: modify done point
void FacesManager::slotDone()
{
    // Switch on scanned for faces flag on digiKam config file.

    KSharedConfig::openConfig()->group("General Settings").writeEntry("Face Scanner First Run", true);

    //MaintenanceTool::slotDone();
}


void FacesManager::slotCancel()
{
    qDebug() << "canceled";
    d->pipeline->cancel();
    MaintenanceTool::slotCancel();
}

void FacesManager::slotAdvance(int a)
{
    advance(a);
}

} // namespace Digikam

#include "facesmanager.moc"
