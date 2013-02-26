/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling accesss to one image and associated data
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imageinfo.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QHash>

// KDE includes

#include <kdebug.h>

// Local includes

#include "globals.h"
#include "albumdb.h"
#include "databaseaccess.h"
#include "databaseinfocontainers.h"
#include "databaseoperationgroup.h"
#include "dimagehistory.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "imageinfodata.h"
#include "imageinfocache.h"
#include "imagelister.h"
#include "imagelisterrecord.h"
#include "imageinfolist.h"
#include "imagecomments.h"
#include "imagecopyright.h"
#include "imageextendedproperties.h"
#include "imageposition.h"
#include "imagescanner.h"
#include "imagetagpair.h"
#include "tagscache.h"
#include "template.h"
#include "infocontainer.h"

namespace Digikam
{

ImageInfoStatic* ImageInfoStatic::m_instance = 0;

void ImageInfoStatic::create()
{
    if (!m_instance)
    {
        m_instance = new ImageInfoStatic;
    }
}

void ImageInfoStatic::destroy()
{
    delete m_instance;
    m_instance = 0;
}

ImageInfoCache* ImageInfoStatic::cache()
{
    return &m_instance->m_cache;
}

// ---------------------------------------------------------------

ImageInfoData::ImageInfoData()
{
    id                     = -1;
    albumId                = -1;
    albumRootId            = -1;

    pickLabel              = NoPickLabel;
    colorLabel             = NoColorLabel;
    rating                 = -1;
    category               = DatabaseItem::UndefinedCategory;
    fileSize               = 0;

    longitude              = 0;
    latitude               = 0;
    altitude               = 0;

    hasCoordinates         = false;
    hasAltitude            = false;

    groupedImages          = 0;
    groupImage             = -1;

    defaultTitleCached     = false;
    defaultCommentCached   = false;
    pickLabelCached        = false;
    colorLabelCached       = false;
    ratingCached           = false;
    categoryCached         = false;
    formatCached           = false;
    creationDateCached     = false;
    modificationDateCached = false;
    fileSizeCached         = false;
    imageSizeCached        = false;
    tagIdsCached           = false;
    positionsCached        = false;
    groupedImagesCached    = false;
    groupImageCached       = false;

    invalid                = false;
}

ImageInfoData::~ImageInfoData()
{
}

// ---------------------------------------------------------------

ImageInfo::ImageInfo()
    : m_data(0)
{
}

ImageInfo::ImageInfo(const ImageListerRecord& record)
{
    m_data                         = ImageInfoStatic::cache()->infoForId(record.imageID);

    ImageInfoWriteLocker lock;
    bool newlyCreated              = m_data->albumId == -1;

    m_data->albumId                = record.albumID;
    m_data->albumRootId            = record.albumRootID;
    m_data->name                   = record.name;

    m_data->rating                 = record.rating;
    m_data->category               = record.category;
    m_data->format                 = record.format;
    m_data->creationDate           = record.creationDate;
    m_data->modificationDate       = record.modificationDate;
    m_data->fileSize               = record.fileSize;
    m_data->imageSize              = record.imageSize;

    m_data->ratingCached           = true;
    m_data->categoryCached         = true;
    m_data->formatCached           = true;
    m_data->creationDateCached     = true;
    m_data->modificationDateCached = true;
    // field is only signed 32 bit in the protocol. -1 indicates value is larger, reread
    m_data->fileSizeCached         = m_data->fileSize != -1;
    m_data->imageSizeCached        = true;

    if (newlyCreated)
    {
        ImageInfoStatic::cache()->cacheByName(m_data);
    }
}

ImageInfo::ImageInfo(qlonglong ID)
{
    m_data = ImageInfoStatic::cache()->infoForId(ID);

    // is this a newly created structure, need to populate?
    if (m_data->albumId == -1)
    {
        // retrieve immutable values now, the rest on demand
        ItemShortInfo info  = DatabaseAccess().db()->getItemShortInfo(ID);

        if (info.id)
        {
            ImageInfoWriteLocker lock;
            m_data->albumId     = info.albumID;
            m_data->albumRootId = info.albumRootID;
            m_data->name        = info.itemName;
            ImageInfoStatic::cache()->cacheByName(m_data);
        }
        else
        {
            // invalid image id
            ImageInfoData* const olddata = m_data.unassign();

            if (olddata)
            {
                ImageInfoStatic::cache()->dropInfo(olddata);
            }

            m_data = 0;
        }
    }
}

ImageInfo::ImageInfo(const KUrl& url)
{
    CollectionLocation location = CollectionManager::instance()->locationForUrl(url);

    if (location.isNull())
    {
        m_data = 0;
        qWarning() << "No location could be retrieved for url" << url;
        return;
    }

    KUrl _url(url.directory());
    QString album = CollectionManager::instance()->album(_url.toLocalFile());
    QString name  = url.fileName();

    // Cached?
    m_data = ImageInfoStatic::cache()->infoForPath(location.id(), album, name);

    if (!m_data)
    {

        ItemShortInfo info = DatabaseAccess().db()->getItemShortInfo(location.id(), album, name);

        if (!info.id)
        {
            m_data = 0;
            qWarning() << "No itemShortInfo could be retrieved from the database for image" << name;
            return;
        }

        m_data              = ImageInfoStatic::cache()->infoForId(info.id);

        ImageInfoWriteLocker lock;

        m_data->albumId     = info.albumID;
        m_data->albumRootId = info.albumRootID;
        m_data->name        = info.itemName;

        ImageInfoStatic::cache()->cacheByName(m_data);
    }
}

ImageInfo::~ImageInfo()
{
    ImageInfoData* const olddata = m_data.unassign();

    if (olddata)
    {
        ImageInfoStatic::cache()->dropInfo(olddata);
    }
}

ImageInfo::ImageInfo(const ImageInfo& info)
{
    m_data = info.m_data;
}

ImageInfo& ImageInfo::operator=(const ImageInfo& info)
{
    if (m_data == info.m_data)
    {
        return *this;
    }

    ImageInfoData* const olddata = m_data.assign(info.m_data);

    if (olddata)
    {
        ImageInfoStatic::cache()->dropInfo(olddata);
    }

    return *this;
}

bool ImageInfo::isNull() const
{
    return !m_data;
}

bool ImageInfo::operator==(const ImageInfo& info) const
{
    if (m_data && info.m_data)
    {
        // not null, compare id
        return m_data->id == info.m_data->id;
    }
    else
    {
        // both null?
        return m_data == info.m_data;
    }
}

bool ImageInfo::operator<(const ImageInfo& info) const
{
    if (m_data)
    {
        if (info.m_data)
            // both not null, sort by id
        {
            return m_data->id < info.m_data->id;
        }
        else
            // only other is null, this is greater than
        {
            return false;
        }
    }
    else
    {
        // this is less than if the other is not null
        return info.m_data;
    }
}

uint ImageInfo::hash() const
{
    if (m_data)
    {
        return ::qHash(m_data->id);
    }
    else
    {
        return ::qHash((int)0);
    }
}

/**
 * Access rules for all methods in this class:
 * ImageInfoData members shall be accessed only under DatabaseAccess lock.
 * The id and albumId are the exception to this rule, as they are
 * primitive and will never change during the lifetime of an object.
 */
qlonglong ImageInfo::id() const
{
    return m_data ? m_data->id : -1;
}

int ImageInfo::albumId() const
{
    return m_data ? m_data->albumId : -1;
}

int ImageInfo::albumRootId() const
{
    return m_data ? m_data->albumRootId : -1;
}

QString ImageInfo::name() const
{
    if (!m_data)
    {
        return QString();
    }

    ImageInfoReadLocker lock;
    return m_data->name;
}

#define RETURN_IF_CACHED(x)       \
    if (m_data->x##Cached)        \
    {                             \
        ImageInfoReadLocker lock; \
        if (m_data->x##Cached)    \
        {                         \
            return m_data->x;     \
        }                         \
    }

#define STORE_IN_CACHE_AND_RETURN(x, retrieveMethod) \
    ImageInfoWriteLocker lock;                       \
    m_data.constCastData()->x##Cached = true;        \
    if (!values.isEmpty())                           \
    {                                                \
        m_data.constCastData()->x = retrieveMethod;  \
    }                                                \
    return m_data->x;

qlonglong ImageInfo::fileSize() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(fileSize)

    QVariantList values = DatabaseAccess().db()->getImagesFields(m_data->id, DatabaseFields::FileSize);

    STORE_IN_CACHE_AND_RETURN(fileSize, values.first().toLongLong())
}

QString ImageInfo::title() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(defaultTitle)

    QString title;
    {
        DatabaseAccess access;
        ImageComments comments(access, m_data->id);
        title = comments.defaultComment(DatabaseComment::Title);
    }

    ImageInfoWriteLocker lock;
    m_data.constCastData()->defaultTitle       = title;
    m_data.constCastData()->defaultTitleCached = true;
    return m_data->defaultTitle;
}

QString ImageInfo::comment() const
{
    if (!m_data)
    {
        return QString();
    }

    RETURN_IF_CACHED(defaultComment)

    QString comment;
    {
        DatabaseAccess access;
        ImageComments comments(access, m_data->id);
        comment = comments.defaultComment();
    }

    ImageInfoWriteLocker lock;
    m_data.constCastData()->defaultComment       = comment;
    m_data.constCastData()->defaultCommentCached = true;
    return m_data->defaultComment;
}

int ImageInfo::pickLabel() const
{
    if (!m_data)
    {
        return NoPickLabel;
    }

    RETURN_IF_CACHED(pickLabel)

    int pickLabel = TagsCache::instance()->pickLabelFromTags(tagIds());

    ImageInfoWriteLocker lock;
    m_data.constCastData()->pickLabel       = (pickLabel == -1) ? NoPickLabel : pickLabel;
    m_data.constCastData()->pickLabelCached = true;
    return m_data->pickLabel;
}

int ImageInfo::colorLabel() const
{
    if (!m_data)
    {
        return NoColorLabel;
    }

    RETURN_IF_CACHED(colorLabel)

    int colorLabel = TagsCache::instance()->colorLabelFromTags(tagIds());

    ImageInfoWriteLocker lock;
    m_data.constCastData()->colorLabel       = (colorLabel == -1) ? NoColorLabel : colorLabel;
    m_data.constCastData()->colorLabelCached = true;
    return m_data->colorLabel;
}

int ImageInfo::rating() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(rating)

    QVariantList values = DatabaseAccess().db()->getImageInformation(m_data->id, DatabaseFields::Rating);
    STORE_IN_CACHE_AND_RETURN(rating, values.first().toLongLong())
}

QString ImageInfo::format() const
{
    if (!m_data)
    {
        return 0;
    }

    RETURN_IF_CACHED(format)
    QVariantList values = DatabaseAccess().db()->getImageInformation(m_data->id, DatabaseFields::Format);
    STORE_IN_CACHE_AND_RETURN(format, values.first().toString())
}

DatabaseItem::Category ImageInfo::category() const
{
    if (!m_data)
    {
        return DatabaseItem::UndefinedCategory;
    }

    RETURN_IF_CACHED(category)
    QVariantList values = DatabaseAccess().db()->getImagesFields(m_data->id, DatabaseFields::Category);
    STORE_IN_CACHE_AND_RETURN(category, (DatabaseItem::Category)values.first().toInt())
}

QDateTime ImageInfo::dateTime() const
{
    if (!m_data)
    {
        return QDateTime();
    }

    RETURN_IF_CACHED(creationDate)
    QVariantList values = DatabaseAccess().db()->getImageInformation(m_data->id, DatabaseFields::CreationDate);
    STORE_IN_CACHE_AND_RETURN(creationDate, values.first().toDateTime())
}

QDateTime ImageInfo::modDateTime() const
{
    if (!m_data)
    {
        return QDateTime();
    }

    RETURN_IF_CACHED(modificationDate)
    QVariantList values = DatabaseAccess().db()->getImagesFields(m_data->id, DatabaseFields::ModificationDate);
    STORE_IN_CACHE_AND_RETURN(modificationDate, values.first().toDateTime())
}

QSize ImageInfo::dimensions() const
{
    if (!m_data)
    {
        return QSize();
    }

    RETURN_IF_CACHED(imageSize)
    QVariantList values = DatabaseAccess().db()->getImageInformation(m_data->id, DatabaseFields::Width | DatabaseFields::Height);
    ImageInfoWriteLocker lock;
    m_data.constCastData()->imageSizeCached = true;

    if (values.size() == 2)
    {
        m_data.constCastData()->imageSize = QSize(values.at(0).toInt(), values.at(1).toInt());
    }

    return m_data->imageSize;
}

QList<int> ImageInfo::tagIds() const
{
    if (!m_data)
    {
        return QList<int>();
    }

    RETURN_IF_CACHED(tagIds)

    QList<int> ids = DatabaseAccess().db()->getItemTagIDs(m_data->id);
    ImageInfoWriteLocker lock;
    m_data.constCastData()->tagIds       = ids;
    m_data.constCastData()->tagIdsCached = true;
    return ids;
}

void ImageInfoList::loadTagIds() const
{
    QVector<QList<int> > allTagIds = DatabaseAccess().db()->getItemsTagIDs(toImageIdList());

    ImageInfoWriteLocker lock;

    for (int i=0; i<size(); i++)
    {
        const ImageInfo& info = at(i);
        const QList<int>& ids = allTagIds.at(i);

        if (!info.m_data)
        {
            continue;
        }

        info.m_data.constCastData()->tagIds       = ids;
        info.m_data.constCastData()->tagIdsCached = true;
    }
}

int ImageInfo::orientation() const
{
    if (!m_data)
    {
        return 0; // ORIENTATION_UNSPECIFIED
    }

    QVariantList values = DatabaseAccess().db()->getImageInformation(m_data->id, DatabaseFields::Orientation);

    if (values.isEmpty())
    {
        return 0;
    }

    return values.first().toInt();
}

DatabaseUrl ImageInfo::databaseUrl() const
{
    if (!m_data)
    {
        return DatabaseUrl();
    }

    QString album     = ImageInfoStatic::cache()->albumRelativePath(m_data->albumId);
    QString albumRoot = CollectionManager::instance()->albumRootPath(m_data->albumRootId);

    ImageInfoReadLocker lock;
    return DatabaseUrl::fromAlbumAndName(m_data->name, album, albumRoot, m_data->albumRootId);
}

KUrl ImageInfo::fileUrl() const
{
    return KUrl::fromPath(filePath());
}

QString ImageInfo::filePath() const
{
    if (!m_data)
    {
        return QString();
    }

    QString albumRoot = CollectionManager::instance()->albumRootPath(m_data->albumRootId);

    if (albumRoot.isNull())
    {
        return QString();
    }

    QString album = ImageInfoStatic::cache()->albumRelativePath(m_data->albumId);
    ImageInfoReadLocker lock;

    if (album == "/")
    {
        return albumRoot + album + m_data->name;
    }
    else
    {
        return albumRoot + album + '/' + m_data->name;
    }
}

bool ImageInfo::isVisible() const
{
    if (!m_data)
    {
        return false;
    }

    QVariantList value = DatabaseAccess().db()->getImagesFields(m_data->id, DatabaseFields::Status);

    if (!value.isEmpty())
    {
        return value.first().toInt() == DatabaseItem::Visible;
    }

    return false;
}

void ImageInfo::setVisible(bool isVisible)
{
    if (!m_data)
    {
        return;
    }

    if (m_data->albumId == 0)
    {
        kWarning() << "Attempt to make a Removed item visible with ImageInfo::setVisible";
        return;
    }

    DatabaseAccess().db()->setItemStatus(m_data->id, isVisible ? DatabaseItem::Visible : DatabaseItem::Hidden);
}

bool ImageInfo::hasDerivedImages() const
{
    if (!m_data)
    {
        return false;
    }

    return DatabaseAccess().db()->hasImagesRelatingTo(m_data->id, DatabaseRelation::DerivedFrom);
}

bool ImageInfo::hasAncestorImages() const
{
    if (!m_data)
    {
        return false;
    }

    return DatabaseAccess().db()->hasImagesRelatedFrom(m_data->id, DatabaseRelation::DerivedFrom);
}

QList<ImageInfo> ImageInfo::derivedImages() const
{
    if (!m_data)
    {
        return QList<ImageInfo>();
    }

    return ImageInfoList(DatabaseAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::DerivedFrom));
}

QList<ImageInfo> ImageInfo::ancestorImages() const
{
    if (!m_data)
    {
        return QList<ImageInfo>();
    }

    return ImageInfoList(DatabaseAccess().db()->getImagesRelatedFrom(m_data->id, DatabaseRelation::DerivedFrom));
}

QList<QPair<qlonglong, qlonglong> > ImageInfo::relationCloud() const
{
    if (!m_data)
    {
        return QList<QPair<qlonglong, qlonglong> >();
    }

    return DatabaseAccess().db()->getRelationCloud(m_data->id, DatabaseRelation::DerivedFrom);
}

void ImageInfo::markDerivedFrom(const ImageInfo& ancestor)
{
    if (!m_data || ancestor.isNull())
    {
        return;
    }

    DatabaseAccess().db()->addImageRelation(m_data->id, ancestor.id(), DatabaseRelation::DerivedFrom);
}

bool ImageInfo::hasGroupedImages() const
{
    return numberOfGroupedImages();
}

int ImageInfo::numberOfGroupedImages() const
{
    if (!m_data)
    {
        return false;
    }

    RETURN_IF_CACHED(groupedImages)

    int groupedImages                           = DatabaseAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::Grouped).size();
    ImageInfoWriteLocker lock;
    m_data.constCastData()->groupedImages       = groupedImages;
    m_data.constCastData()->groupedImagesCached = true;
    return m_data->groupedImages;
}

qlonglong ImageInfo::groupImageId() const
{
    if (!m_data)
    {
        return -1;
    }

    RETURN_IF_CACHED(groupImage)

    QList<qlonglong> ids = DatabaseAccess().db()->getImagesRelatedFrom(m_data->id, DatabaseRelation::Grouped);
    // list size should be 0 or 1
    int groupImage       = ids.isEmpty() ? -1 : ids.first();

    ImageInfoWriteLocker lock;
    m_data.constCastData()->groupImage       = groupImage;
    m_data.constCastData()->groupImageCached = true;
    return m_data->groupImage;
}

void ImageInfoList::loadGroupImageIds() const
{
    QVector<QList<qlonglong> > allGroupIds = DatabaseAccess().db()->getImagesRelatedFrom(toImageIdList(), DatabaseRelation::Grouped);
    ImageInfoWriteLocker lock;

    for (int i=0; i<size(); i++)
    {
        const ImageInfo& info            = at(i);
        const QList<qlonglong>& groupIds = allGroupIds.at(i);

        if (!info.m_data)
        {
            continue;
        }

        info.m_data.constCastData()->groupImage       = groupIds.isEmpty() ? -1 : groupIds.first();
        info.m_data.constCastData()->groupImageCached = true;
    }
}

bool ImageInfo::isGrouped() const
{
    return groupImageId() != -1;
}

ImageInfo ImageInfo::groupImage() const
{
    qlonglong id = groupImageId();

    if (id == -1)
    {
        return ImageInfo();
    }

    return ImageInfo(id);
}

QList<ImageInfo> ImageInfo::groupedImages() const
{
    if (!m_data || !hasGroupedImages())
    {
        return QList<ImageInfo>();
    }

    return ImageInfoList(DatabaseAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::Grouped));
}

void ImageInfo::addToGroup(const ImageInfo& givenLeader)
{
    if (!m_data || givenLeader.isNull() || givenLeader.id() == m_data->id)
    {
        return;
    }

    // Take care: Once we start this, we cannot rely on change notifications and cache invalidation!
    DatabaseOperationGroup group;

    // Handle grouping on an already grouped image, and prevent circular grouping
    ImageInfo leader;
    QList<qlonglong> alreadySeen;
    alreadySeen << m_data->id;

    for (leader = givenLeader; leader.isGrouped(); )
    {
        ImageInfo nextLeader = leader.groupImage();
        // is the new leader currently grouped on this image, or do we have a circular grouping?
        if (alreadySeen.contains(nextLeader.id()))
        {
            // break loop (special case: remove b->a where we want to add a->b)
            leader.removeFromGroup();
            break;
        }
        else
        {
            alreadySeen << leader.id();
            leader = nextLeader;
        }
    }

    // Already grouped correctly?
    if (groupImageId() == leader.id())
    {
        return;
    }

    // All images grouped on this image need a new group leader
    QList<qlonglong> idsToBeGrouped  = DatabaseAccess().db()->getImagesRelatingTo(m_data->id, DatabaseRelation::Grouped);
    // and finally, this image needs to be grouped
    idsToBeGrouped << m_data->id;

    foreach(qlonglong ids, idsToBeGrouped)
    {
        // remove current grouping
        DatabaseAccess().db()->removeAllImageRelationsFrom(ids, DatabaseRelation::Grouped);
        // add the new grouping
        DatabaseAccess().db()->addImageRelation(ids, leader.id(), DatabaseRelation::Grouped);
    }
}

void ImageInfo::removeFromGroup()
{
    if (!m_data)
    {
        return;
    }

    if (!isGrouped())
    {
        return;
    }

    DatabaseAccess().db()->removeAllImageRelationsFrom(m_data->id, DatabaseRelation::Grouped);
}

void ImageInfo::clearGroup()
{
    if (!m_data)
    {
        return;
    }

    if (!hasGroupedImages())
    {
        return;
    }

    DatabaseAccess().db()->removeAllImageRelationsTo(m_data->id, DatabaseRelation::Grouped);
}

ImageComments ImageInfo::imageComments(DatabaseAccess& access) const
{
    if (!m_data)
    {
        return ImageComments();
    }

    return ImageComments(access, m_data->id);
}

ImageCopyright ImageInfo::imageCopyright() const
{
    if (!m_data)
    {
        return ImageCopyright();
    }

    return ImageCopyright(m_data->id);
}

ImageExtendedProperties ImageInfo::imageExtendedProperties() const
{
    if (!m_data)
    {
        return ImageExtendedProperties();
    }

    return ImageExtendedProperties(m_data->id);
}

ImagePosition ImageInfo::imagePosition() const
{
    if (!m_data)
    {
        return ImagePosition();
    }

    ImagePosition pos(m_data->id);

    if (!m_data->positionsCached)
    {
        ImageInfoWriteLocker lock;
        m_data.constCastData()->longitude       = pos.longitudeNumber();
        m_data.constCastData()->latitude        = pos.latitudeNumber();
        m_data.constCastData()->altitude        = pos.altitude();
        m_data.constCastData()->hasCoordinates  = pos.hasCoordinates();
        m_data.constCastData()->hasAltitude     = pos.hasAltitude();
        m_data.constCastData()->positionsCached = true;
    }

    return pos;
}

double ImageInfo::longitudeNumber() const
{
    if (!m_data)
    {
        return 0;
    }

    if (!m_data->positionsCached)
    {
        imagePosition();
    }

    return m_data->longitude;
}

double ImageInfo::latitudeNumber() const
{
    if (!m_data)
    {
        return 0;
    }

    if (!m_data->positionsCached)
    {
        imagePosition();
    }

    return m_data->latitude;
}

double ImageInfo::altitudeNumber() const
{
    if (!m_data)
    {
        return 0;
    }

    if (!m_data->positionsCached)
    {
        imagePosition();
    }

    return m_data->altitude;
}

bool ImageInfo::hasCoordinates() const
{
    if (!m_data)
    {
        return 0;
    }

    if (!m_data->positionsCached)
    {
        imagePosition();
    }

    return m_data->hasCoordinates;
}

bool ImageInfo::hasAltitude() const
{
    if (!m_data)
    {
        return 0;
    }

    if (!m_data->positionsCached)
    {
        imagePosition();
    }

    return m_data->hasAltitude;
}

ImageTagPair ImageInfo::imageTagPair(int tagId) const
{
    if (!m_data)
    {
        return ImageTagPair();
    }

    return ImageTagPair(*this, tagId);
}

QList<ImageTagPair> ImageInfo::availableImageTagPairs() const
{
    if (!m_data)
    {
        return QList<ImageTagPair>();
    }

    return ImageTagPair::availablePairs(*this);
}

DImageHistory ImageInfo::imageHistory() const
{
    if (!m_data)
    {
        return DImageHistory();
    }

    ImageHistoryEntry entry = DatabaseAccess().db()->getImageHistory(m_data->id);
    return DImageHistory::fromXml(entry.history);
}

void ImageInfo::setImageHistory(const DImageHistory& history)
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess().db()->setImageHistory(m_data->id, history.toXml());
}

bool ImageInfo::hasImageHistory() const
{
    if (!m_data)
    {
        return false;
    }

    return DatabaseAccess().db()->hasImageHistory(m_data->id);
}

QString ImageInfo::uuid() const
{
    if (!m_data)
    {
        return QString();
    }

    return DatabaseAccess().db()->getImageUuid(m_data->id);
}

void ImageInfo::setUuid(const QString& uuid)
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess().db()->setImageUuid(m_data->id, uuid);
}

HistoryImageId ImageInfo::historyImageId() const
{
    if (!m_data)
    {
        return HistoryImageId();
    }

    HistoryImageId id(uuid());
    id.setCreationDate(dateTime());
    id.setFileName(name());
    id.setPathOnDisk(filePath());

    if (DatabaseAccess().db()->isUniqueHashV2())
    {
        ItemScanInfo info = DatabaseAccess().db()->getItemScanInfo(m_data->id);
        id.setUniqueHash(info.uniqueHash, info.fileSize);
    }

    return id;
}

ImageCommonContainer ImageInfo::imageCommonContainer() const
{
    if (!m_data)
    {
        return ImageCommonContainer();
    }

    ImageCommonContainer container;
    ImageScanner::fillCommonContainer(m_data->id, &container);
    return container;
}

ImageMetadataContainer ImageInfo::imageMetadataContainer() const
{
    if (!m_data)
    {
        return ImageMetadataContainer();
    }

    ImageMetadataContainer container;
    ImageScanner::fillMetadataContainer(m_data->id, &container);
    return container;
}

VideoMetadataContainer ImageInfo::videoMetadataContainer() const
{
    if (!m_data)
    {
        return VideoMetadataContainer();
    }

    VideoMetadataContainer container;
    ImageScanner::fillVideoMetadataContainer(m_data->id, &container);
    return container;
}

PhotoInfoContainer ImageInfo::photoInfoContainer() const
{
    if (!m_data)
    {
        return PhotoInfoContainer();
    }

    ImageMetadataContainer meta = imageMetadataContainer();
    PhotoInfoContainer photoInfo;

    photoInfo.make              = meta.make;
    photoInfo.model             = meta.model;
    photoInfo.lens              = meta.lens;
    photoInfo.exposureTime      = meta.exposureTime;
    photoInfo.exposureMode      = meta.exposureMode;
    photoInfo.exposureProgram   = meta.exposureProgram;
    photoInfo.aperture          = meta.aperture;
    photoInfo.focalLength       = meta.focalLength;
    photoInfo.focalLength35mm   = meta.focalLength35;
    photoInfo.sensitivity       = meta.sensitivity;
    photoInfo.flash             = meta.flashMode;
    photoInfo.whiteBalance      = meta.whiteBalance;
    photoInfo.dateTime          = dateTime();

    return photoInfo;
}

VideoInfoContainer ImageInfo::videoInfoContainer() const
{
    if (!m_data)
    {
        return VideoInfoContainer();
    }

    VideoMetadataContainer meta = videoMetadataContainer();
    VideoInfoContainer videoInfo;

    videoInfo.aspectRatio       = meta.aspectRatio;
    videoInfo.audioBitRate      = meta.audioBitRate;
    videoInfo.audioChannelType  = meta.audioChannelType;
    videoInfo.audioCompressor   = meta.audioCompressor;
    videoInfo.duration          = meta.duration;
    videoInfo.frameRate         = meta.frameRate;
    videoInfo.videoCodec        = meta.videoCodec;

    return videoInfo;
}

Template ImageInfo::metadataTemplate() const
{
    if (!m_data)
    {
        return Template();
    }

    Template t;
    imageCopyright().fillTemplate(t);

    ImageExtendedProperties ep = imageExtendedProperties();
    t.setLocationInfo(ep.location());
    t.setIptcSubjects(ep.subjectCode());
    return t;
}

void ImageInfo::setMetadataTemplate(const Template& t)
{
    if (!m_data)
    {
        return;
    }

    removeMetadataTemplate();
    imageCopyright().setFromTemplate(t);

    ImageExtendedProperties ep = imageExtendedProperties();
    ep.setLocation(t.locationInfo());
    ep.setSubjectCode(t.IptcSubjects());
}

void ImageInfo::removeMetadataTemplate()
{
    if (!m_data)
    {
        return;
    }

    imageCopyright().removeAll();

    ImageExtendedProperties ep = imageExtendedProperties();
    ep.removeLocation();
    ep.removeSubjectCode();
}

void ImageInfo::setPickLabel(int pickId)
{
    if (!m_data || pickId < FirstPickLabel || pickId > LastPickLabel)
    {
        return;
    }

    QList<int> currentTagIds   = tagIds();
    QVector<int> pickLabelTags = TagsCache::instance()->pickLabelTags();

    // Pick Label is an exclusive tag.
    // Perform "switch" operation atomic
    {
        DatabaseAccess access;

        foreach(int tagId, currentTagIds)
        {
            if (pickLabelTags.contains(tagId))
            {
                removeTag(tagId);
            }
        }

        setTag(pickLabelTags[pickId]);
    }

    ImageInfoWriteLocker lock;
    m_data->pickLabel       = pickId;
    m_data->pickLabelCached = true;
}

void ImageInfo::setColorLabel(int colorId)
{
    if (!m_data || colorId < FirstColorLabel || colorId > LastColorLabel)
    {
        return;
    }

    QList<int> currentTagIds    = tagIds();
    QVector<int> colorLabelTags = TagsCache::instance()->colorLabelTags();

    // Color Label is an exclusive tag.
    // Perform "switch" operation atomic
    {
        DatabaseAccess access;

        foreach(int tagId, currentTagIds)
        {
            if (colorLabelTags.contains(tagId))
            {
                removeTag(tagId);
            }
        }

        setTag(colorLabelTags[colorId]);
    }

    ImageInfoWriteLocker lock;
    m_data->colorLabel       = colorId;
    m_data->colorLabelCached = true;
}

void ImageInfo::setRating(int value)
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess().db()->changeImageInformation(m_data->id, QVariantList() << value, DatabaseFields::Rating);

    ImageInfoWriteLocker lock;
    m_data->rating       = value;
    m_data->ratingCached = true;
}

void ImageInfo::setOrientation(int value)
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess().db()->changeImageInformation(m_data->id, QVariantList() << value, DatabaseFields::Orientation);
}

void ImageInfo::setDateTime(const QDateTime& dateTime)
{
    if (!m_data || !dateTime.isValid())
    {
        return;
    }

    DatabaseAccess().db()->changeImageInformation(m_data->id, QVariantList() << dateTime, DatabaseFields::CreationDate);

    ImageInfoWriteLocker lock;
    m_data->creationDate       = dateTime;
    m_data->creationDateCached = true;
}

void ImageInfo::setTag(int tagID)
{
    if (!m_data || tagID <= 0)
    {
        return;
    }

    DatabaseAccess().db()->addItemTag(m_data->id, tagID);
}

void ImageInfo::removeTag(int tagID)
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess access;
    access.db()->removeItemTag(m_data->id, tagID);
    access.db()->removeImageTagProperties(m_data->id, tagID);
}

void ImageInfo::removeAllTags()
{
    if (!m_data)
    {
        return;
    }

    DatabaseAccess().db()->removeItemAllTags(m_data->id, tagIds());
}

void ImageInfo::addTagPaths(const QStringList& tagPaths)
{
    if (!m_data)
    {
        return;
    }

    QList<int> tagIds = TagsCache::instance()->tagsForPaths(tagPaths);
    DatabaseAccess().db()->addTagsToItems(QList<qlonglong>() << m_data->id, tagIds);
}

ImageInfo ImageInfo::copyItem(int dstAlbumID, const QString& dstFileName)
{
    if (!m_data)
    {
        return ImageInfo();
    }

    {
        ImageInfoReadLocker lock;

        if (dstAlbumID == m_data->albumId && dstFileName == m_data->name)
        {
            return (*this);
        }
    }

    int id = DatabaseAccess().db()->copyItem(m_data->albumId, m_data->name, dstAlbumID, dstFileName);

    if (id == -1)
    {
        return ImageInfo();
    }

    return ImageInfo(id);
}

QDebug& operator<<(QDebug& stream, const ImageInfo& info)
{
    return stream << "ImageInfo [id = " << info.id() << ", databaseurl = "
                  << info.databaseUrl() << "]";
}

}  // namespace Digikam
