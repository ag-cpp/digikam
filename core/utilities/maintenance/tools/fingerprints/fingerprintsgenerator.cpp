/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-16
 * Description : fingerprints generator
 *
 * SPDX-FileCopyrightText:      2018 by Mario Frank    <mario dot frank at uni minus potsdam dot de>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "fingerprintsgenerator.h"

// Qt includes

#include <QApplication>
#include <QString>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "dimg.h"
#include "coredb.h"
#include "albummanager.h"
#include "coredbaccess.h"
#include "maintenancethread.h"

namespace Digikam
{

class Q_DECL_HIDDEN FingerPrintsGenerator::Private
{
public:

    Private() = default;

    bool               rebuildAll   = true;

    AlbumList          albumList;

    MaintenanceThread* thread       = nullptr;
};

FingerPrintsGenerator::FingerPrintsGenerator(const bool rebuildAll, const AlbumList& list, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("FingerPrintsGenerator"), parent),
      d              (new Private)
{
    d->albumList  = list;
    d->rebuildAll = rebuildAll;
    d->thread     = new MaintenanceThread(this);

    connect(d->thread, SIGNAL(signalCompleted()),
            this, SLOT(slotDone()));

    connect(d->thread, SIGNAL(signalAdvance(ItemInfo,QImage)),
            this, SLOT(slotAdvance(ItemInfo,QImage)));
}

FingerPrintsGenerator::~FingerPrintsGenerator()
{
    delete d;
}

void FingerPrintsGenerator::setUseMultiCoreCPU(bool b)
{
    d->thread->setUseMultiCore(b);
}

void FingerPrintsGenerator::slotCancel()
{
    d->thread->cancel();
    MaintenanceTool::slotCancel();
}

void FingerPrintsGenerator::slotStart()
{
    MaintenanceTool::slotStart();

    setLabel(i18n("Finger-prints"));

    ProgressManager::addProgressItem(this);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (d->albumList.isEmpty())
    {
        d->albumList = AlbumManager::instance()->allPAlbums();
    }

    // Get all item IDs from albums.

    QList<qlonglong> itemIds;

    for (AlbumList::ConstIterator it = d->albumList.constBegin() ;
         !canceled() && (it != d->albumList.constEnd()) ; ++it)
    {
        if      ((*it)->type() == Album::PHYSICAL)
        {
            const auto ids = CoreDbAccess().db()->getItemIDsInAlbum((*it)->id());

            for (const qlonglong& id : ids)
            {
                if (!itemIds.contains(id))
                {
                    itemIds << id;
                }
            }
        }
        else if ((*it)->type() == Album::TAG)
        {
            const auto ids = CoreDbAccess().db()->getItemIDsInTag((*it)->id());

            for (const qlonglong& id : ids)
            {
                if (!itemIds.contains(id))
                {
                    itemIds << id;
                }
            }
        }
    }

    QApplication::restoreOverrideCursor();

    if (itemIds.isEmpty())
    {
        slotDone();

        return;
    }

    setTotalItems(itemIds.count());

    d->thread->generateFingerprints(itemIds, d->rebuildAll);
    d->thread->start();
}

void FingerPrintsGenerator::slotAdvance(const ItemInfo& inf, const QImage& img)
{
    QString lbl = i18n("Rebuild Fingerprint for: %1\n", inf.name());
    lbl.append(i18n("Path: %1", inf.relativePath()));
    setLabel(lbl);
    setThumbnail(QIcon(QPixmap::fromImage(img)));
    advance(1);
}

void FingerPrintsGenerator::slotDone()
{
    // Switch on scanned for finger-prints flag on digiKam config file.

    KSharedConfig::openConfig()->group(QLatin1String("General Settings")).writeEntry(QLatin1String("Finger Prints Generator First Run"), true);

    MaintenanceTool::slotDone();
}

} // namespace Digikam

#include "moc_fingerprintsgenerator.cpp"
