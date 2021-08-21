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
#include "faceitemretriever.h"
#include "faceembedding_manager.h"
#include "extractionworker.h"
#include "asyncbuffer.h"

namespace Digikam
{
    
class Q_DECL_HIDDEN TrainerWorker::Private
{
public:

    explicit Private(FacePipeline::Private* const dd)
        : imageRetriever(dd),
          buffer(1000),
          cancel(false)
    {
    }

    ~Private()
    {
    }

public:

    IdentitiesManager                               identitiesManager;
    FaceEmbeddingManager                            db;
    FaceItemRetriever                               imageRetriever;
    AsyncBuffer<FacePipelineExtendedPackage::Ptr>   buffer;
    bool                                            cancel;
};

// ----------------------------------------------------------------------------------------

TrainerWorker::TrainerWorker(FacePipeline::Private* const dd)
    : QThread(),
      d(new Private(dd))
{
}

TrainerWorker::~TrainerWorker()
{
    cancel();
    delete d;
}

void TrainerWorker::run()
{
    while (!d->cancel)
    {
        FacePipelineExtendedPackage::Ptr package = d->buffer.read();

        if (package == nullptr)
        {
            break;
        }

        FaceUtils            utils;

        foreach (const FacePipelineFaceTagsIface& face, package->databaseFaces)
        {
            if (face.roles & FacePipelineFaceTagsIface::ForTraining)
            {
                FaceTagsIface dbFace = face;
                dbFace.setType(FaceTagsIface::FaceForTraining);

                Identity identity = utils.identityForTag(dbFace.tagId(), d->identitiesManager);

                d->db.editIdentity(ExtractionWorker::encodeTagID(package->info.id(), dbFace), identity.id());
            }
        }

        package->databaseFaces.replaceRole(FacePipelineFaceTagsIface::ForTraining, FacePipelineFaceTagsIface::Trained);
        package->processFlags |= FacePipelinePackage::ProcessedByTrainer;

        emit processed(package);
    }
}

void TrainerWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    d->buffer.append(package);
}

void TrainerWorker::cancel()
{
    d->cancel = true;
    d->buffer.cancel();
    wait();
}

} // namespace Digikam
