/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-01
 * Description : ItemInfo common data
 *
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_INFO_CACHE_H
#define DIGIKAM_ITEM_INFO_CACHE_H

// Qt includes

#include <QHash>
#include <QObject>
#include <QMultiHash>
#include <QExplicitlySharedDataPointer>

// Local includes

#include "coredbwatch.h"

namespace Digikam
{

class AlbumShortInfo;
class ItemInfoData;

// NOTE: No need to EXPORT this class
class ItemInfoCache : public QObject
{
    Q_OBJECT

public:

    ItemInfoCache();
    ~ItemInfoCache() override;

    /**
     * Return an ItemInfoData object for the given image id.
     * A new object is created, or an existing object is returned.
     * If a new object is created, the id field will be initialized.
     */
    QExplicitlySharedDataPointer<ItemInfoData> infoForId(qlonglong id);

    /**
     * Call this when the data has been dereferenced,
     * before deletion.
     */
    void dropInfo(const QExplicitlySharedDataPointer<ItemInfoData>& infoPtr);

    /**
     * Call this to put data in the hash by file name if you have newly created data
     * and the name is filled.
     * Call under write lock.
     */
    void cacheByName(const QExplicitlySharedDataPointer<ItemInfoData>& infoPtr);

    /**
     * Return an ItemInfoData object for the given album root, relativePath and file name triple.
     * Works if previously cached with cacheByName.
     * Returns 0 if not found.
     */
    QExplicitlySharedDataPointer<ItemInfoData> infoForPath(int albumRootId,
                                                           const QString& relativePath,
                                                           const QString& name);

    /**
     * Returns the cached relativePath for the given album id.
     */
    QString albumRelativePath(int albumId);

    /**
     * Returns the cached grouped count for the given image id.
     */
    int getImageGroupedCount(qlonglong id);

    /**
     * Invalidate the cache and all its cached data
     */
    void invalidate();

private Q_SLOTS:

    void slotImageChanged(const ImageChangeset& changeset);
    void slotImageTagChanged(const ImageTagChangeset& changeset);
    void slotAlbumChange(const AlbumChangeset&);

private:

    // Disable
    explicit ItemInfoCache(QObject*);

    QList<AlbumShortInfo>::const_iterator findAlbum(int id);
    void                                  checkAlbums();

private:

    QMultiHash<QString, ItemInfoData*> m_nameHash;
    QHash<qlonglong, ItemInfoData*>    m_infoHash;
    QHash<ItemInfoData*, QString>      m_dataHash;
    volatile bool                      m_needUpdateAlbums;
    volatile bool                      m_needUpdateGrouped;
    QList<qlonglong>                   m_grouped;
    QList<AlbumShortInfo>              m_albums;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_INFO_CACHE_H
