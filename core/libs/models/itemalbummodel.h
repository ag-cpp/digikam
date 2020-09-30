/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-08
 * Description : Qt item model for database entries, listing done with database job
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_ITEM_ALBUM_MODEL_H
#define DIGIKAM_ITEM_ALBUM_MODEL_H

// Local includes

#include "itemthumbnailmodel.h"
#include "album.h"

namespace Digikam
{

class ImageChangeset;
class CollectionImageChangeset;
class SearchChangeset;
class Album;

class ItemAlbumModel : public ItemThumbnailModel
{
    Q_OBJECT

public:

    explicit ItemAlbumModel(QObject* const parent = nullptr);
    ~ItemAlbumModel() override;

    QList<Album*> currentAlbums()       const;

    bool hasScheduledRefresh()          const;
    bool isRecursingAlbums()            const;
    bool isRecursingTags()              const;
    bool isListingOnlyAvailableImages() const;

public Q_SLOTS:

    /**
     * Call this method to populate the model with data from the given album.
     * If called with 0, the model will be empty.
     * Opening the same album again is a no-op.
     */
    void openAlbum(const QList<Album*>& albums);

    /**
     * Reloads the current album
     */
    void refresh();

    void setRecurseAlbums(bool recursiveListing);
    void setRecurseTags(bool recursiveListing);
    void setListOnlyAvailableImages(bool onlyAvailable);

    void setSpecialTagListing(const QString& specialListing);
/*
Q_SIGNALS:

    void listedAlbumChanged(QList<Album*> album);
*/
protected Q_SLOTS:

    void scheduleRefresh();
    void scheduleIncrementalRefresh();

    void slotResult();
    void slotData(const QList<ItemListerRecord>& records);

    void slotNextRefresh();
    void slotNextIncrementalRefresh();

    void slotCollectionImageChange(const CollectionImageChangeset& changeset);
    void slotSearchChange(const SearchChangeset& changeset);

    void slotAlbumAdded(Album* album);
    void slotAlbumDeleted(Album* album);
    void slotAlbumRenamed(Album* album);
    void slotAlbumsCleared();

    void incrementalRefresh();

    void slotImageChange(const ImageChangeset& changeset) override;
    void slotImageTagChange(const ImageTagChangeset& changeset) override;

protected:

    void startListJob(const QList<Album*>& albums);

private:

    // Hidden copy constructor and assignment operator.
    ItemAlbumModel(const ItemAlbumModel&);
    ItemAlbumModel& operator=(const ItemAlbumModel&);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_ALBUM_MODEL_H
