/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
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

#include "trainerworker.h"

// Local includes

#include "digikam_debug.h"
#include "identities_manager.h"
#include "extractionworker.h"
#include "faceitemretriever.h"

namespace Digikam
{
class Q_DECL_HIDDEN TrainerWorker::Private
{
public:

    explicit Private(FacePipeline::Private* const dd)
        : imageRetriever(dd)
    {
    }

    ~Private()
    {
    }

public:
    IdentitiesManager   identitiesManager;
    ExtractionWorker    extractor;
    FaceItemRetriever   imageRetriever;
};

// ----------------------------------------------------------------------------------------

TrainerWorker::TrainerWorker(FacePipeline::Private* const dd)
    : d(new Private(dd))
{
}

TrainerWorker::~TrainerWorker()
{
    wait();    // protect detector
    delete d;
}

/**
 * TODO: investigate this method
 */
void TrainerWorker::process(FacePipelineExtendedPackage::Ptr package)
{
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << "TrainerWorker: processing one package";
*/
    // Get a list of faces with type FaceForTraining (probably type is ConfirmedFace)

    QList<FaceTagsIface> toTrain;
    QVector<QString>     tagIDs;
    QVector<int>         identities;
    FaceUtils            utils;

    foreach (const FacePipelineFaceTagsIface& face, package->databaseFaces)
    {
        if (face.roles & FacePipelineFaceTagsIface::ForTraining)
        {
            FaceTagsIface dbFace = face;
            dbFace.setType(FaceTagsIface::FaceForTraining);
            toTrain << dbFace;
            tagIDs  << d->extractor.encodeTagID(package->info.id(), dbFace);

            Identity identity = utils.identityForTag(dbFace.tagId(), d->identitiesManager);
            identities << identity.id();
        }
    }

    if (!toTrain.isEmpty())
    {
        QList<QImage> images;

        if (package->image.isNull())
        {
            images = d->imageRetriever.getThumbnails(package->filePath, toTrain);
        }
        else
        {
            images = d->imageRetriever.getDetails(package->image, toTrain);
        }

        d->extractor.extract(tagIDs, images, identities);
    }

    utils.removeFaces(toTrain);
    package->databaseFaces.replaceRole(FacePipelineFaceTagsIface::ForTraining, FacePipelineFaceTagsIface::Trained);
    package->processFlags |= FacePipelinePackage::ProcessedByTrainer;

    emit processed(package);
}

void TrainerWorker::aboutToDeactivate()
{
    d->imageRetriever.cancel();
}

} // namespace Digikam
