/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-02-20
 * Description : Synchronized container for maintenance data.
 *
 * SPDX-FileCopyrightText: 2017-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedata.h"

// Qt includes

#include <QRecursiveMutex>

// Local includes

#include "identity.h"

namespace Digikam
{

class Q_DECL_HIDDEN MaintenanceData::Private
{
public:

    explicit Private()
      : rebuildAllFingerprints(true)
    {
    }

    QList<qlonglong>              imageIdList;
    QList<int>                    thumbnailIdList;
    QList<QString>                imagePathList;
    QList<ItemInfo>               imageInfoList;
    QList<Identity>               identitiesList;
    QList<qlonglong>              similarityImageIdList;

    bool                          rebuildAllFingerprints;

    QRecursiveMutex               mutex;
};

MaintenanceData::MaintenanceData()
    : d(new Private)
{
}

MaintenanceData::~MaintenanceData()
{
    delete d;
}

void MaintenanceData::setImageIds(const QList<qlonglong>& ids)
{
    d->imageIdList = ids;
}

void MaintenanceData::setThumbnailIds(const QList<int>& ids)
{
    d->thumbnailIdList = ids;
}

void MaintenanceData::setImagePaths(const QList<QString>& paths)
{
    d->imagePathList = paths;
}

void MaintenanceData::setItemInfos(const QList<ItemInfo>& infos)
{
    d->imageInfoList = infos;
}

void MaintenanceData::setSimilarityImageIds(const QList<qlonglong>& ids)
{
    d->similarityImageIdList = ids;
}

void MaintenanceData::setIdentities(const QList<Identity>& identities)
{
    d->identitiesList = identities;
}

void MaintenanceData::setRebuildAllFingerprints(bool b)
{
    d->rebuildAllFingerprints = b;
}

qlonglong MaintenanceData::getImageId() const
{
    d->mutex.lock();
    qlonglong id = -1;

    if (!d->imageIdList.isEmpty())
    {
        id = d->imageIdList.takeFirst();
    }

    d->mutex.unlock();

    return id;
}

int MaintenanceData::getThumbnailId() const
{
    d->mutex.lock();
    int id = -1;

    if (!d->thumbnailIdList.isEmpty())
    {
        id = d->thumbnailIdList.takeFirst();
    }

    d->mutex.unlock();

    return id;
}

QString MaintenanceData::getImagePath() const
{
    d->mutex.lock();
    QString path;

    if (!d->imagePathList.isEmpty())
    {
        path = d->imagePathList.takeFirst();
    }

    d->mutex.unlock();

    return path;
}

ItemInfo MaintenanceData::getItemInfo() const
{
    d->mutex.lock();
    ItemInfo info;

    if (!d->imageInfoList.isEmpty())
    {
        info = d->imageInfoList.takeFirst();
    }

    d->mutex.unlock();

    return info;
}

Identity MaintenanceData::getIdentity() const
{
    d->mutex.lock();
    Identity identity;

    if (!d->identitiesList.isEmpty())
    {
        identity = d->identitiesList.takeFirst();
    }

    d->mutex.unlock();

    return identity;
}

qlonglong MaintenanceData::getSimilarityImageId() const
{
    d->mutex.lock();
    qlonglong id = -1;

    if (!d->similarityImageIdList.isEmpty())
    {
        id = d->similarityImageIdList.takeFirst();
    }

    d->mutex.unlock();

    return id;
}

bool MaintenanceData::getRebuildAllFingerprints() const
{
    return d->rebuildAllFingerprints;
}

} // namespace Digikam
