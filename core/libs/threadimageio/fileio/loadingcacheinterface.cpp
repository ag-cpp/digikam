/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-06
 * Description : shared image loading and caching
 *
 * Copyright (C) 2005-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "loadingcacheinterface.h"

// Local includes

#include "loadingcache.h"

namespace Digikam
{

void LoadingCacheInterface::initialize()
{
    LoadingCache::cache();
}

void LoadingCacheInterface::cleanUp()
{
    LoadingCache::cleanUp();
}

void LoadingCacheInterface::fileChanged(const QString& filePath, bool notify)
{
    LoadingCache* const cache = LoadingCache::cache();
    LoadingCache::CacheLock lock(cache);

    cache->notifyFileChanged(filePath, notify);
}

void LoadingCacheInterface::connectToSignalFileChanged(QObject* const object, const char* slot)
{
    LoadingCache* const cache = LoadingCache::cache();

    QObject::connect(cache, SIGNAL(fileChanged(QString)),
                     object, slot,
                     Qt::QueuedConnection);

    // make it a queued connection because the signal is emitted when the CacheLock is held!
}

void LoadingCacheInterface::cleanCache()
{
    LoadingCache* const cache = LoadingCache::cache();
    LoadingCache::CacheLock lock(cache);

    cache->removeImages();
}

void LoadingCacheInterface::cleanThumbnailCache()
{
    LoadingCache* const cache = LoadingCache::cache();
    LoadingCache::CacheLock lock(cache);

    cache->removeThumbnails();
}

void LoadingCacheInterface::putImage(const QString& filePath, const DImg& img)
{
    LoadingCache* const cache = LoadingCache::cache();
    LoadingCache::CacheLock lock(cache);

    if (cache->isCacheable(img))
    {
        cache->putImage(filePath, img, filePath);
    }
}

void LoadingCacheInterface::setCacheOptions(int cacheSize)
{
    LoadingCache* const cache = LoadingCache::cache();
    LoadingCache::CacheLock lock(cache);

    cache->setCacheSize(cacheSize);
}

} // namespace Digikam
