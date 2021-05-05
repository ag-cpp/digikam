/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-08
 * Description : FaceEngine database interface allowing easy manipulation of face tags
 *
 * Copyright (C) 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "faceutils.h"

// Qt includes

#include <QImage>
#include <QTimer>

// Local includes

#include "digikam_debug.h"
#include "coredbaccess.h"
#include "coredbconstants.h"
#include "coredboperationgroup.h"
#include "coredb.h"
#include "dimg.h"
#include "facetags.h"
#include "iteminfo.h"
#include "itemtagpair.h"
#include "fileactionmngr.h"
#include "tagproperties.h"
#include "tagscache.h"
#include "tagregion.h"
#include "thumbnailloadthread.h"
#include "albummanager.h"

namespace Digikam
{

// --- Constructor / Destructor -------------------------------------------------------------------------------------

FaceUtils::FaceUtils(QObject* const parent)
    : QObject(parent)
{
}

FaceUtils::~FaceUtils()
{
}

// --- Mark for scanning and training -------------------------------------------------------------------------------

bool FaceUtils::hasBeenScanned(qlonglong imageid) const
{
    return hasBeenScanned(ItemInfo(imageid));
}

bool FaceUtils::hasBeenScanned(const ItemInfo& info) const
{
    return info.tagIds().contains(FaceTags::scannedForFacesTagId());
}

void FaceUtils::markAsScanned(qlonglong imageid, bool hasBeenScanned) const
{
    markAsScanned(ItemInfo(imageid), hasBeenScanned);
}

void FaceUtils::markAsScanned(const ItemInfo& info, bool hasBeenScanned) const
{
    if (hasBeenScanned)
    {
        ItemInfo(info).setTag(FaceTags::scannedForFacesTagId());
    }
    else
    {
        ItemInfo(info).removeTag(FaceTags::scannedForFacesTagId());
    }
}

// --- Convert between FacesEngine results and FaceTagsIface ---

QList<FaceTagsIface> FaceUtils::toFaceTagsIfaces(qlonglong imageid,
                                                 const QList<QRectF>& detectedFaces,
                                                 const QList<Identity>& recognitionResults,
                                                 const QSize& fullSize) const
{
    QList<FaceTagsIface> faces;

    for (int i = 0 ; i < detectedFaces.size() ; ++i)
    {
        Identity identity;

        if (!recognitionResults.isEmpty())
        {
            identity = recognitionResults[i];
        }

        // We'll get the unknownPersonTagId if the identity is null

        int tagId                = FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
        QRect fullSizeRect       = TagRegion::relativeToAbsolute(detectedFaces[i], fullSize);
        FaceTagsIface::Type type = identity.isNull() ? FaceTagsIface::UnknownName : FaceTagsIface::UnconfirmedName;

        if (!tagId || !fullSizeRect.isValid())
        {
            faces << FaceTagsIface();
            continue;
        }
/*
        qCDebug(DIGIKAM_GENERAL_LOG) << "New Entry" << fullSizeRect << tagId;
*/
        faces << FaceTagsIface(type, imageid, tagId, TagRegion(fullSizeRect));
    }

    return faces;
}

// --- Images in faces and thumbnails ---

void FaceUtils::storeThumbnails(ThumbnailLoadThread* const thread,
                                const QString& filePath,
                                const QList<FaceTagsIface>& databaseFaces,
                                const DImg& image)
{
    foreach (const FaceTagsIface& face, databaseFaces)
    {
        QList<QRect> rects;
        rects << face.region().toRect();
        const int margin = faceRectDisplayMargin(face.region().toRect());
        rects << face.region().toRect().adjusted(-margin, -margin, margin, margin);

        foreach (const QRect& rect, rects)
        {
            QRect mapped  = TagRegion::mapFromOriginalSize(image, rect);
            QImage detail = image.copyQImage(mapped);
            thread->storeDetailThumbnail(filePath, rect, detail, true);
        }
    }
}

// --- Face detection: merging results ------------------------------------------------------------------------------------

QList<FaceTagsIface> FaceUtils::writeUnconfirmedResults(qlonglong imageid,
                                                        const QList<QRectF>& detectedFaces,
                                                        const QList<Identity>& recognitionResults,
                                                        const QSize& fullSize)
{
    // Build list of new entries

    QList<FaceTagsIface> newFaces = toFaceTagsIfaces(imageid, detectedFaces, recognitionResults, fullSize);

    if (newFaces.isEmpty())
    {
        return newFaces;
    }

    // list of existing entries

    QList<FaceTagsIface> currentFaces = databaseFaces(imageid);

    // merge new with existing entries

    for (int i = 0 ; i < newFaces.size() ; ++i)
    {
        FaceTagsIface& newFace = newFaces[i];
        QList<FaceTagsIface> overlappingEntries;

        foreach (const FaceTagsIface& oldFace, currentFaces)
        {
            double minOverlap = oldFace.isConfirmedName() ? 0.25 : 0.5;

            if (oldFace.region().intersects(newFace.region(), minOverlap))
            {
                overlappingEntries << oldFace;
                qCDebug(DIGIKAM_GENERAL_LOG) << "Entry" << oldFace.region() << oldFace.tagId()
                                             << "overlaps" << newFace.region() << newFace.tagId() << ", skipping";
            }
        }

        // The purpose if the next scope is to merge entries:
        // A confirmed face will never be overwritten.
        // If a name is set to an old face, it will only be replaced by a new face with a name.

        if (!overlappingEntries.isEmpty())
        {
            if (newFace.isUnknownName())
            {
                // we have no name in the new face. Do we have one in the old faces?

                for (int j = 0 ; j < overlappingEntries.size() ; ++j)
                {
                    const FaceTagsIface& oldFace = overlappingEntries.at(j);

                    if (oldFace.isUnknownName())
                    {
                        // remove old face
                    }
                    else
                    {
                        // skip new entry if any overlapping face has a name, and we do not

                        newFace = FaceTagsIface();
                        break;
                    }
                }
            }
            else
            {
                // we have a name in the new face. Do we have names in overlapping faces?

                for (int j = 0 ; j < overlappingEntries.size() ; ++j)
                {
                    FaceTagsIface& oldFace = overlappingEntries[j];

                    if      (oldFace.isUnknownName())
                    {
                        // remove old face
                    }
                    else if (oldFace.isUnconfirmedName())
                    {
                        if (oldFace.tagId() == newFace.tagId())
                        {
                            // remove smaller face

                            if (oldFace.region().intersects(newFace.region(), 1))
                            {
                                newFace = FaceTagsIface();
                                break;
                            }

                            // else remove old face
                        }
                        else
                        {
                            // assume new recognition is more trained, remove older face
                        }
                    }
                    else if (oldFace.isConfirmedName())
                    {
                        // skip new entry, confirmed has of course priority

                        newFace = FaceTagsIface();
                    }
                }
            }
        }

        // if we did not decide to skip this face, add is to the db now

        if (!newFace.isNull())
        {
            // list will contain all old entries that should still be removed

            removeFaces(overlappingEntries);

            ItemTagPair pair(imageid, newFace.tagId());

            // UnconfirmedName and UnknownName have the same attribute

            addFaceAndTag(pair, newFace, FaceTagsIface::attributesForFlags(FaceTagsIface::UnconfirmedName), false);

            // If the face is unconfirmed and the tag is not the unknown person tag, set the unconfirmed person property.

            if (newFace.isUnconfirmedType() && !FaceTags::isTheUnknownPerson(newFace.tagId()))
            {
                ItemTagPair unconfirmedPair(imageid, FaceTags::unconfirmedPersonTagId());
                unconfirmedPair.addProperty(ImageTagPropertyName::autodetectedPerson(),newFace.getAutodetectedPersonString());
            }
        }
    }

    return newFaces;
}

Identity FaceUtils::identityForTag(int tagId, FacialRecognitionWrapper& recognizer) const
{
    QMap<QString, QString> attributes = FaceTags::identityAttributes(tagId);
    Identity identity                 = recognizer.findIdentity(attributes);

    if (!identity.isNull())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Found FacesEngine identity" << identity.id() << "for tag" << tagId;
        return identity;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Adding new FacesEngine identity with attributes" << attributes;
    identity                          = recognizer.addIdentity(attributes);

    FaceTags::applyTagIdentityMapping(tagId, identity.attributesMap());

    return identity;
}

int FaceUtils::tagForIdentity(const Identity& identity) const
{
    return FaceTags::getOrCreateTagForIdentity(identity.attributesMap());
}

// --- Editing normal tags, reimplemented with FileActionMngr ---

void FaceUtils::addNormalTag(qlonglong imageId, int tagId)
{
    FileActionMngr::instance()->assignTag(ItemInfo(imageId), tagId);

    /**
     * Implementation for automatic assigning of face as
     * Tag Icon, if no icon exists currently.
     * Utilising a QTimer to ensure that a new TAlbum
     * is given time to be created, before assigning Icon.
     */
    QTimer::singleShot(200, this, [=]()
        {
            if (
                !FaceTags::isTheIgnoredPerson(tagId)  &&
                !FaceTags::isTheUnknownPerson(tagId)  &&
                !FaceTags::isTheUnconfirmedPerson(tagId)
               )
            {
                TAlbum* const album = AlbumManager::instance()->findTAlbum(tagId);

                // If Icon is NULL, set the newly added Face as the Icon.

                if (album && (album->iconId() == 0))
                {
                    QString err;

                    if (!AlbumManager::instance()->updateTAlbumIcon(album, QString(),
                                                                    imageId, err))
                    {
                        qCDebug(DIGIKAM_GENERAL_LOG) << err ;
                    }
                }
            }
        }
    );
}

void FaceUtils::removeNormalTag(qlonglong imageId, int tagId)
{
    FileActionMngr::instance()->removeTag(ItemInfo(imageId), tagId);

    if (
        !FaceTags::isTheIgnoredPerson(tagId)  &&
        !FaceTags::isTheUnknownPerson(tagId)  &&
        !FaceTags::isTheUnconfirmedPerson(tagId)
       )
    {
        int count = CoreDbAccess().db()->getNumberOfImagesInTagProperties(tagId,
                                         ImageTagPropertyName::tagRegion());

        /**
         * If the face just removed was the final face
         * associated with that Tag, reset Tag Icon.
         */
        if (count == 0)
        {
            TAlbum* const album = AlbumManager::instance()->findTAlbum(tagId);

            if (album && (album->iconId() != 0))
            {
                QString err;

                if (!AlbumManager::instance()->updateTAlbumIcon(album, QString(),
                                                                0, err))
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << err ;
                }
            }
        }
    }
}

void FaceUtils::removeNormalTags(qlonglong imageId, const QList<int>& tagIds)
{
    FileActionMngr::instance()->removeTags(ItemInfo(imageId), tagIds);
}

// --- Utilities ---

QSize FaceUtils::rotateFaces(const ItemInfo& info,
                             int newOrientation,
                             int oldOrientation)
{
    /**
     *  Get all faces from database and rotate them
     */
    QList<FaceTagsIface> facesList = databaseFaces(info.id());

    if (facesList.isEmpty())
    {
        return QSize();
    }

    QSize newSize = info.dimensions();

    foreach (const FaceTagsIface& dface, facesList)
    {
        QRect faceRect = dface.region().toRect();

        TagRegion::reverseToOrientation(faceRect,
                                        oldOrientation,
                                        info.dimensions());

        newSize = TagRegion::adjustToOrientation(faceRect,
                                                 newOrientation,
                                                 info.dimensions());

        changeRegion(dface, TagRegion(faceRect));
    }

    return newSize;
}

int FaceUtils::faceRectDisplayMargin(const QRect& rect)
{
    /*
     * Do not change that value unless you know what you do.
     * There are a lot of pregenerated thumbnails in user's databases,
     * expensive to regenerate, depending on this very value.
     */
    int margin = qMax(rect.width(), rect.height());
    margin    /= 10;

    return margin;
}

} // Namespace Digikam
