/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-05-16
 * Description : fingerprints generator
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "fingerprintsgenerator.h"
#include "fingerprintsgenerator.moc"

// Qt includes.

#include <QString>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QPointer>
#include <QCloseEvent>

// KDE includes.

#include <kapplication.h>
#include <kcodecs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandardguiitem.h>

// Local includes.

#include "dimg.h"
#include "album.h"
#include "albumdb.h"
#include "albummanager.h"
#include "albumsettings.h"
#include "databaseaccess.h"
#include "haar.h"
#include "haariface.h"
#include "previewloadthread.h"

namespace Digikam
{

class FingerPrintsGeneratorPriv
{
public:

    FingerPrintsGeneratorPriv()
    {
        cancel            = false;
        rebuildAll        = true;
        previewLoadThread = 0;
        duration.start();
    }

    bool               cancel;
    bool               rebuildAll;

    QTime              duration;

    QStringList        allPicturesPath;

    PreviewLoadThread *previewLoadThread;

    HaarIface          haarIface;
};

FingerPrintsGenerator::FingerPrintsGenerator(QWidget* /*parent*/, bool rebuildAll)
                     : DProgressDlg(0), d(new FingerPrintsGeneratorPriv)
{
    d->rebuildAll        = rebuildAll;
    d->previewLoadThread = new PreviewLoadThread();

    connect(d->previewLoadThread, SIGNAL(signalImageLoaded(const LoadingDescription&, const DImg&)),
            this, SLOT(slotGotImagePreview(const LoadingDescription&, const DImg&)));

    setModal(false);
    setValue(0);
    setCaption(i18n("Rebuild All Fingerprints"));
    setLabel(i18n("<b>Updating fingerprint database. Please wait...</b>"));
    setButtonText(i18n("&Abort"));
    resize(600, 300);

    QTimer::singleShot(500, this, SLOT(slotRebuildFingerPrints()));
}

FingerPrintsGenerator::~FingerPrintsGenerator()
{
    delete d;
}

void FingerPrintsGenerator::slotRebuildFingerPrints()
{
    setTitle(i18n("Processing..."));
    const AlbumList palbumList = AlbumManager::instance()->allPAlbums();

    // Get all digiKam albums collection pictures path, depending of d->rebuildAll flag.

    if (d->rebuildAll)
    {
        for (AlbumList::ConstIterator it = palbumList.constBegin();
             !d->cancel && (it != palbumList.constEnd()); ++it)
        {
            d->allPicturesPath += DatabaseAccess().db()->getItemURLsInAlbum((*it)->id());
        }
    }
    else
    {
        d->allPicturesPath = DatabaseAccess().db()->getDirtyOrMissingFingerprintURLs();
    }

    setMaximum(d->allPicturesPath.count());

    if(d->allPicturesPath.isEmpty())
    {
       slotCancel();
       return;
    }

    processOne();
}

void FingerPrintsGenerator::processOne()
{
    if (d->cancel) return;
    QString path = d->allPicturesPath.first();
    d->previewLoadThread->load(LoadingDescription(path, HaarIface::preferredSize(),
                               AlbumSettings::instance()->getExifRotate()));
}

void FingerPrintsGenerator::complete()
{
    QTime t;
    t = t.addMSecs(d->duration.elapsed());
    setLabel(i18n("<b>Update of fingerprint database complete.</b>"));
    setTitle(i18n("Duration: %1", t.toString()));
    setButtonGuiItem(KStandardGuiItem::ok());
    setButtonText(i18n("&Close"));
    emit signalRebuildAllFingerPrintsDone();
}

void FingerPrintsGenerator::slotGotImagePreview(const LoadingDescription& desc, const DImg& img)
{
    if (d->allPicturesPath.first() != desc.filePath)
        return;

    if (!img.isNull())
    {
        // compute Haar fingerprint
        d->haarIface.indexImage(desc.filePath, img);
    }
    QPixmap pix = DImg(img).smoothScale(128, 128, Qt::KeepAspectRatio).convertToPixmap();
    addedAction(pix, desc.filePath);
    advance(1);
    if (!d->allPicturesPath.isEmpty())
        d->allPicturesPath.removeFirst();
    if (d->allPicturesPath.isEmpty())
        complete();
    else
        processOne();
}

void FingerPrintsGenerator::slotCancel()
{
    abort();
    done(Cancel);
}

void FingerPrintsGenerator::closeEvent(QCloseEvent *e)
{
    abort();
    e->accept();
}

void FingerPrintsGenerator::abort()
{
    d->cancel = true;
    emit signalRebuildAllFingerPrintsDone();
}

}  // namespace Digikam
