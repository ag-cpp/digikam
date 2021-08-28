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

#include "recognitionworker.h"

// Local includes

#include "digikam_debug.h"
#include "faceembeddingcache.h"
#include "opencvdnnfacerecognizer.h"
#include "faceitemretriever.h"
#include "identities_manager.h"
#include "extractionworker.h"
#include "asyncbuffer.h"

namespace Digikam
{

class RecognitionWorker::Private
{
public:
    explicit Private(FacePipeline::Private* const dd)
        : imageRetriever(dd),
          recognizer(nullptr),
          buffer(100)
    {
    }

    ~Private()
    {
        delete recognizer;
    }

    void init()
    {
        faceembeddingMap = FaceEmbeddingCache::getData();

        qDebug() << "retrieve" << faceembeddingMap.size();

        cv::Mat predictors, labels;

        for (QMap<QString, FaceEmbeddingData>::iterator it = faceembeddingMap.begin(); it != faceembeddingMap.end(); ++it)
        {
            if (it.value().identity >= 0)
            {
                predictors.push_back(it.value().embedding);
                labels.push_back(it.value().identity);
            }
        }

        recognizer = new OpenCVDNNFaceRecognizer(cv::ml::TrainData::create(predictors, 0, labels));
        recognizer->setThreshold(threshold);
    }

    FaceItemRetriever                               imageRetriever;
    QMap<QString, FaceEmbeddingData>                faceembeddingMap;
    OpenCVDNNFaceRecognizer*                        recognizer;
    IdentitiesManager                               identitiesManager;
    AsyncBuffer<FacePipelineExtendedPackage::Ptr>   buffer;
    float                                           threshold;
};

RecognitionWorker::RecognitionWorker(FacePipeline::Private* const dd)
    : QThread(),
      d(new Private(dd))
{
}

RecognitionWorker::~RecognitionWorker()
{
    cancel();
    wait();
    qDebug() << "RecognitionWorker deleted";
    delete d;
}

void RecognitionWorker::run()
{
    while (true)
    {
        FacePipelineExtendedPackage::Ptr package = d->buffer.read();

        if (package == nullptr)
        {
            break;
        }

        if (!d->recognizer)
        {
            d->init();
        }

        QVector<cv::Mat> embeddings;

        foreach (const FacePipelineFaceTagsIface& face, package->databaseFaces)
        {
            QString tagID = ExtractionWorker::encodeTagID(package->info.id(), face);
            if (d->faceembeddingMap.contains(tagID))
            {
                embeddings << d->faceembeddingMap[tagID].embedding;
            }
            else
            {
                qDebug() << "Fail to find face embedding for tag" << tagID;
            }
        } 

        QVector<int> identitiesIDs = d->recognizer->recognize(embeddings);

        for (int i = 0; i < identitiesIDs.size(); ++i)
        {
            package->recognitionResults << d->identitiesManager.identity(identitiesIDs[i]);
        }

        package->processFlags |= FacePipelinePackage::ProcessedByRecognizer;

        emit processed(package);
    }

    qDebug() << "RecognitionWorker exited";
    emit canceled();
}

void RecognitionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    d->buffer.append(package);
}

void RecognitionWorker::cancel()
{
    d->buffer.cancel();
}

void RecognitionWorker::setThreshold(double threshold, bool)
{
    d->threshold = threshold;
}

} // namespace Digikam
