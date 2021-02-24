/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Listing information from database.
 *
 * Copyright (C) 2005      by Renchi Raju    <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2015      by Mohamed_Anwer  <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_ITEM_LISTER_H
#define DIGIKAM_ITEM_LISTER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"
#include "iteminfo.h"
#include "iteminfolist.h"
#include "coredbaccess.h"
#include "coredburl.h"
#include "itemlisterrecord.h"
#include "itemlisterreceiver.h"
#include "dbjobinfo.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemLister
{

public:

    explicit ItemLister();
    ~ItemLister();

    /**
     * Adjust the setting if album or tags will be listed recursively (i.e. including subalbums / subtags)
     */
    void setRecursive(bool recursive);

    /**
     * Adjust the setting if images from collections that are currently not in the state "available"
     * will be included in the listing. Default: true.
     */
    void setListOnlyAvailable(bool listOnlyAvailable);

    /**
     * Convenience method for Album, Tag and Date URLs, _not_ for Search URLs.
     */
    void list(ItemListerReceiver* const receiver,
              const CoreDbUrl& url);

    /**
     * List those images whose date lies in the range beginning with startDate (inclusive)
     * and ending before endDate (exclusive).
     */
    void listDateRange(ItemListerReceiver* const receiver,
                       const QDate& startDate,
                       const QDate& endDate);

private:

    /// Hidden copy constructor and assignment operator.
    ItemLister(const ItemLister&);
    ItemLister& operator=(const ItemLister&);

    // -----------------------------------------------------------------------------

    /** @name Operations with PAlbum
     */

    //@{

public:

    /**
     * List images in the Album (physical album) specified by albumRoot, album.
     * The results will be fed to the specified receiver.
     */
    void listPAlbum(ItemListerReceiver* const receiver,
                    int albumRootId,
                    const QString& album);

private:

    QSet<int> albumRootsToList() const;

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations with TAlbum
     */

    //@{

public:

    /**
     * List the images which have assigned the tags specified by tagIds
     * Updated to support multiple tags
     */
    void listTag(ItemListerReceiver* const receiver,
                 const QList<int>& tagIds);

    /**
     * Execute the search specified by search XML describing a Tag Properties search.
     * Two special add-ons: Non-unique by image id; if enabled, uses the extended ImageRecord protocol
     * to pass the property value in the record's extraValue.
     * @param receiver the receiver for the searches
     * @param xml SearchXml describing the query
     */
    void listImageTagPropertySearch(ItemListerReceiver* const receiver,
                                    const QString& xml);

    QString tagSearchXml(int tagId,
                         const QString& type,
                         bool includeChildTags) const;

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations with SAlbum
     */

    //@{

public:

    /**
     * Execute the search specified by search XML
     * @param receiver the receiver for the searches
     * @param xml SearchXml describing the query
     * @param limit the limit the count of the result set. If limit = 0, then no limit is set.
     * @param referenceImageId the id of a reference image in the search query.
     */
    void listSearch(ItemListerReceiver* const receiver,
                    const QString& xml,
                    int limit = 0,
                    qlonglong referenceImageId = -1);

    /**
     * Execute the search specified by search XML describing a Haar search
     * @param receiver the receiver for the searches
     * @param xml SearchXml describing the query
     */
    void listHaarSearch(ItemListerReceiver* const receiver,
                        const QString& xml);

    /**
     * List the images whose coordinates are between coordinates contained
     * in areaCoordinates(lat1, lat2, lng1, lng2).
     */
    void listAreaRange(ItemListerReceiver* const receiver,
                       double lat1,
                       double lat2,
                       double lon1,
                       double lon2);

private:

    /**
     * This method generates image records for the receiver that contain the similarities.
     * @param receiver for the searches
     * @param imageSimilarityMap the map of image ids and their similarities in the HAAR search
     */
    void listFromHaarSearch(ItemListerReceiver* const receiver,
                            const QMap<qlonglong,
                            double>& imageSimilarityMap);
    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations with FAlbum
     */

    //@{

public:

    /**
     * List the images which have faces. An image with n faces will be listed n times.
     * FIXME: Obviously an ugly way. Should be trashed later in favor of a better method.
     */
    void listFaces(ItemListerReceiver* const receiver,
                   int personId);

private:

    void listFromIdList(ItemListerReceiver* const receiver,
                        const QList<qlonglong>& imageIds);

    //@}

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_LISTER_H
