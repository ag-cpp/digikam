/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-21
 * Description : metadatahub manager
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#include "metadatahubmngr.h"

// Qt includes

#include <QMutex>
#include <QDebug>
#include <QProgressDialog>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "iteminfo.h"
#include "metadatahub.h"
#include "iteminfolist.h"
#include "metadatasynchronizer.h"

namespace Digikam
{

QPointer<MetadataHubMngr> MetadataHubMngr::internalPtr = QPointer<MetadataHubMngr>();

class Q_DECL_HIDDEN MetadataHubMngr::Private
{
public:

    explicit Private()
        : mutex()
    {
    }

    QList<qlonglong> pendingItemIds;
    QMutex           mutex;
};

MetadataHubMngr::MetadataHubMngr()
    : d(new Private())
{
}

MetadataHubMngr::~MetadataHubMngr()
{
    delete d;
}

MetadataHubMngr* MetadataHubMngr::instance()
{
    if (internalPtr.isNull())
    {
        internalPtr = new MetadataHubMngr();
    }

    return internalPtr;
}

bool MetadataHubMngr::isCreated()
{
    return (!internalPtr.isNull());
}

void MetadataHubMngr::addPending(const ItemInfo& info)
{
    QMutexLocker locker(&d->mutex);

    if (!d->pendingItemIds.contains(info.id()))
    {
        d->pendingItemIds.append(info.id());
    }

    emit signalPendingMetadata(d->pendingItemIds.size());
}

void MetadataHubMngr::slotApplyPending()
{
    QMutexLocker lock(&d->mutex);

    if (d->pendingItemIds.isEmpty())
    {
        return;
    }

    ItemInfoList infos;

    foreach (const qlonglong& id, d->pendingItemIds)
    {
        ItemInfo info(id);

        if (!info.isNull() && !info.isRemoved())
        {
            infos.append(info);
        }
    }

    d->pendingItemIds.clear();

    emit signalPendingMetadata(0);

    if (infos.isEmpty())
    {
        return;
    }

    MetadataSynchronizer* const tool = new MetadataSynchronizer(infos,
                                           MetadataSynchronizer::WriteFromDatabaseToFile);
    tool->start();
}

void MetadataHubMngr::requestShutDown()
{
    QMutexLocker lock(&d->mutex);

    if (d->pendingItemIds.isEmpty())
    {
        return;
    }

    ItemInfoList infos;

    foreach (const qlonglong& id, d->pendingItemIds)
    {
        ItemInfo info(id);

        if (!info.isNull() && !info.isRemoved())
        {
            infos.append(info);
        }
    }

    d->pendingItemIds.clear();

    emit signalPendingMetadata(0);

    if (infos.isEmpty())
    {
        return;
    }

    QPointer<QProgressDialog> dialog = new QProgressDialog;
    dialog->setMinimum(0);
    dialog->setMaximum(0);
    dialog->setMinimumDuration(100);
    dialog->setLabelText(i18nc("@label", "Apply pending changes to metadata"));

    MetadataSynchronizer* const tool = new MetadataSynchronizer(infos,
                                           MetadataSynchronizer::WriteFromDatabaseToFile);

    connect(tool, SIGNAL(signalComplete()),
            dialog, SLOT(accept()));

    tool->start();

    dialog->exec();
}

} // namespace Digikam
