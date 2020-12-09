/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-24
 * Description : Container for IOJob data.
 *
 * Copyright (C) 2018 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "iojobdata.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>

// Local includes

#include "album.h"
#include "iteminfo.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN IOJobData::Private
{
public:

    explicit Private()
      : operation   (Unknown),
        fileConflict(Continue),
        srcAlbum    (nullptr),
        destAlbum   (nullptr),
        jobTime     (QDateTime::currentDateTime()),
        mutex       (QMutex::Recursive)
    {
    }

    int                operation;
    int                fileConflict;

    PAlbum*            srcAlbum;
    PAlbum*            destAlbum;

    DTrashItemInfoList trashItemList;
    QMap<QUrl, QUrl>   changeDestMap;
    QList<ItemInfo>    itemInfosList;
    QList<QUrl>        sourceUrlList;

    QUrl               destUrl;

    QString            progressId;
    QDateTime          jobTime;

    QMutex             mutex;
};

IOJobData::IOJobData(int operation,
                     const QList<ItemInfo>& infos,
                     PAlbum* const dest)
    : d(new Private)
{
    d->operation = operation;
    d->destAlbum = dest;

    setItemInfos(infos);

    if (d->destAlbum)
    {
        d->destUrl = d->destAlbum->fileUrl();
    }
}

IOJobData::IOJobData(int operation,
                     const QList<ItemInfo>& infos,
                     const QUrl& dest)
    : d(new Private)
{
    d->operation = operation;
    d->destUrl   = dest;

    setItemInfos(infos);
}

IOJobData::IOJobData(int operation,
                     const QList<QUrl>& urls,
                     PAlbum* const dest)
    : d(new Private)
{
    d->operation     = operation;
    d->sourceUrlList = urls;
    d->destAlbum     = dest;

    if (d->destAlbum)
    {
        d->destUrl = d->destAlbum->fileUrl();
    }
}

IOJobData::IOJobData(int operation,
                     PAlbum* const src,
                     PAlbum* const dest)
    : d(new Private)
{
    d->operation = operation;
    d->srcAlbum  = src;
    d->destAlbum = dest;

    if (d->srcAlbum)
    {
        d->sourceUrlList << d->srcAlbum->fileUrl();
    }

    if (d->destAlbum)
    {
        d->destUrl = d->destAlbum->fileUrl();
    }
}

IOJobData::IOJobData(int operation,
                     const QList<QUrl>& urls,
                     const QUrl& dest)
    : d(new Private)
{
    d->operation     = operation;
    d->sourceUrlList = urls;
    d->destUrl       = dest;
}

IOJobData::IOJobData(int operation,
                     const ItemInfo& info,
                     const QString& newName,
                     bool overwrite)
    : d(new Private)
{
    d->operation = operation;

    if (overwrite)
    {
        d->fileConflict = Overwrite;
    }

    setItemInfos(QList<ItemInfo>() << info);

    d->destUrl = info.fileUrl().adjusted(QUrl::RemoveFilename);
    d->destUrl.setPath(d->destUrl.path() + newName);
}

IOJobData::IOJobData(int operation,
                     const DTrashItemInfoList& infos)
    : d(new Private)
{
    d->operation     = operation;
    d->trashItemList = infos;

    // We need source URLs as dummy.

    foreach (const DTrashItemInfo& item, d->trashItemList)
    {
        d->sourceUrlList << QUrl::fromLocalFile(item.trashPath);
    }
}

IOJobData::~IOJobData()
{
    delete d;
}

void IOJobData::setItemInfos(const QList<ItemInfo>& infos)
{
    d->itemInfosList = infos;

    d->sourceUrlList.clear();

    foreach (const ItemInfo& info, d->itemInfosList)
    {
        d->sourceUrlList << info.fileUrl();
    }
}

void IOJobData::setSourceUrls(const QList<QUrl>& urls)
{
    d->sourceUrlList = urls;
}

void IOJobData::setDestUrl(const QUrl& srcUrl,
                           const QUrl& destUrl)
{
    QMutexLocker lock(&d->mutex);

    d->changeDestMap.insert(srcUrl, destUrl);
}

void IOJobData::setProgressId(const QString& id)
{
    d->progressId = id;
}

void IOJobData::setFileConflict(int fc)
{
    d->fileConflict = fc;
}

int IOJobData::operation() const
{
    return d->operation;
}

int IOJobData::fileConflict() const
{
    return d->fileConflict;
}

PAlbum* IOJobData::srcAlbum() const
{
    return d->srcAlbum;
}

PAlbum* IOJobData::destAlbum() const
{
    return d->destAlbum;
}

QUrl IOJobData::destUrl(const QUrl& srcUrl) const
{
    if (srcUrl.isEmpty())
    {
        return d->destUrl;
    }

    return d->changeDestMap.value(srcUrl, d->destUrl);
}

QUrl IOJobData::getNextUrl() const
{
    QMutexLocker lock(&d->mutex);

    QUrl url;

    if (!d->sourceUrlList.isEmpty())
    {
        url = d->sourceUrlList.takeFirst();
    }

    return url;
}

QString IOJobData::getProgressId() const
{
    return d->progressId;
}

QDateTime IOJobData::jobTime() const
{
    return d->jobTime;
}

ItemInfo IOJobData::findItemInfo(const QUrl& url) const
{
    foreach (const ItemInfo& info, d->itemInfosList)
    {
        if (info.fileUrl() == url)
        {
            return info;
        }
    }

    return ItemInfo();
}

QList<QUrl> IOJobData::sourceUrls() const
{
    return d->sourceUrlList;
}

QList<ItemInfo> IOJobData::itemInfos() const
{
    return d->itemInfosList;
}

DTrashItemInfoList IOJobData::trashItems() const
{
    return d->trashItemList;
}

} // namespace Digikam
