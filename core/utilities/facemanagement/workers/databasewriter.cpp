/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "databasewriter.h"

// KDE includes

#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

DatabaseWriter::DatabaseWriter(FacePipeline::WriteMode mode, FacePipeline::Private* const d)
    : mode(mode),
      thumbnailLoadThread(d->createThumbnailLoadThread()),
      d(d)
{
}

void DatabaseWriter::process(FacePipelineExtendedPackage::Ptr package)
{
    if (package->databaseFaces.isEmpty())
    {
        // Detection / Recognition

        FaceUtils utils;

        // OverwriteUnconfirmed means that a new scan discarded unconfirmed results of previous scans
        // (assuming at least equal or new, better methodology is in use compared to the previous scan)

        if ((mode == FacePipeline::OverwriteUnconfirmed) && (package->processFlags & FacePipelinePackage::ProcessedByDetector))
        {
            QList<FaceTagsIface> oldEntries = utils.unconfirmedFaceTagsIfaces(package->info.id());
            qCDebug(DIGIKAM_GENERAL_LOG) << "Removing old entries" << oldEntries;
            utils.removeFaces(oldEntries);
        }

        // mark the whole image as scanned-for-faces

        utils.markAsScanned(package->info);

        if (!package->info.isNull() && !package->detectedFaces.isEmpty())
        {
            package->databaseFaces = utils.writeUnconfirmedResults(package->info.id(),
                                                                   package->detectedFaces,
                                                                   package->recognitionResults,
                                                                   package->image.originalSize());
            package->databaseFaces.setRole(FacePipelineFaceTagsIface::DetectedFromImage);

            if (!package->image.isNull())
            {
                utils.storeThumbnails(thumbnailLoadThread, package->filePath,
                                      package->databaseFaces.toFaceTagsIfaceList(), package->image);
            }
        }
    }
    else if (package->processFlags & FacePipelinePackage::ProcessedByRecognizer)
    {
        FaceUtils utils;

        for (int i = 0 ; i < package->databaseFaces.size() ; ++i)
        {
            if (package->databaseFaces[i].roles & FacePipelineFaceTagsIface::ForRecognition)
            {
                // Allow to overwrite existing recognition with new, possibly valid, "not recognized" status

                int tagId = FaceTags::unknownPersonTagId();

                // NOTE: See bug #338485 : check if index is not outside of container size.

                if ((i < package->recognitionResults.size()) &&
                    !package->recognitionResults[i].isNull())
                {
                    // Only perform this call if recognition as results, to prevent crash in QMap. See bug #335624

                    tagId = FaceTags::getOrCreateTagForIdentity(package->recognitionResults[i].attributesMap());
                }

                /**
                 * Implementation to ensure that:
                 * 1. Unconfirmed face count, for the associated image is incremented.
                 * 2. Region of Face gets mapped to Suggested Name in the Associated Image.
                 * It's important to manually do this, as ItemInfo only reads from Database
                 * when there's no cached information. Occasionally ItemInfo may not update,
                 * even if number of Unconfirmed Faces have changed.
                 *
                 * It's also important to check that the Face for which we're incrementing
                 * wasn't marked as Unconfirmed already. This can happen in photos with
                 * multiple Faces in it.
                 */
                if (package->databaseFaces[i].type() != FaceTagsIface::UnconfirmedName)
                {
                    package->info.incrementUnconfirmedFaceCount(true);

                    QString region = package->databaseFaces[i].region().toXml();
                    QString suggestedName = FaceTags::faceNameForTag(tagId);
                    package->info.addSuggestedName(region, suggestedName);
                }

                package->databaseFaces[i]        = FacePipelineFaceTagsIface(utils.changeSuggestedName(package->databaseFaces[i], tagId));
                package->databaseFaces[i].roles &= ~FacePipelineFaceTagsIface::ForRecognition;
           }
        }
    }
    else
    {
        // Editing database entries

        FaceUtils utils;
        FacePipelineFaceTagsIfaceList add;
        FacePipelineFaceTagsIfaceList::iterator it;

        for (it = package->databaseFaces.begin() ; it != package->databaseFaces.end() ; ++it)
        {
            if (it->roles & FacePipelineFaceTagsIface::ForConfirmation)
            {
                FacePipelineFaceTagsIface confirmed = FacePipelineFaceTagsIface(utils.confirmName(*it,  package->info, it->assignedTagId, it->assignedRegion));
                confirmed.roles                    |= FacePipelineFaceTagsIface::Confirmed | FacePipelineFaceTagsIface::ForTraining;
                add << confirmed;
            }
            else if (it->roles & FacePipelineFaceTagsIface::ForEditing)
            {
                if      (it->isNull())
                {
                    // add Manually

                    FaceTagsIface newFace = utils.unconfirmedEntry(package->info.id(), it->assignedTagId, it->assignedRegion);
                    utils.addManually(newFace);
                    add << FacePipelineFaceTagsIface(newFace);
                }
                else if (it->assignedRegion.isValid())
                {
                    add << FacePipelineFaceTagsIface(utils.changeRegion(*it, it->assignedRegion));
                }

                // Change Tag operation.
                else if (FaceTags::isPerson(it->assignedTagId))
                {
                    add << FacePipelineFaceTagsIface(utils.changeTag(*it, it->assignedTagId, package->info));
                }
                else
                {
                    utils.removeFace(*it);
                }

                it->roles &= ~FacePipelineFaceTagsIface::ForEditing;
                it->roles |= FacePipelineFaceTagsIface::Edited;
            }

            // Training is done by trainerWorker
        }

        if (!package->image.isNull())
        {
            utils.storeThumbnails(thumbnailLoadThread, package->filePath, add.toFaceTagsIfaceList(), package->image);
        }

        package->databaseFaces << add;
    }

    package->processFlags |= FacePipelinePackage::WrittenToDatabase;

    emit processed(package);
}

} // namespace Digikam
