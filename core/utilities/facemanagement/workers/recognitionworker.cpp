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
#include "dimension_reducer.h"
#include "opencvdnnfacerecognizer.h"
#include "faceitemretriever.h"
#include "identities_manager.h"
#include "faceembedding_manager.h"
#include "extractionworker.h"

namespace Digikam
{

QVector<FaceEmbeddingData>& reduceDimension(QVector<FaceEmbeddingData>& data, int nbCPU)
{
    cv::Mat embeddings;

    for (int i = 0; i < data.size(); ++i)
    {
        embeddings.push_back(data[i].embedding);
    }

    cv::Mat projectedEmbedings = Digikam::DimensionReducer::reduceDimension(embeddings, 2, nbCPU);

    for (int i = 0; i < data.size(); ++i)
    {
        data[i].embedding = projectedEmbedings.row(i);
    }

    return data;
}

class RecognitionWorker::Private
{
public:
    explicit Private(FacePipeline::Private* const dd)
        : imageRetriever(dd)
    {
    }

    ~Private()
    {
        delete recognizer;
    }

    FaceItemRetriever                   imageRetriever;
    QMap<QString, FaceEmbeddingData>    faceembeddingMap;
    OpenCVDNNFaceRecognizer*            recognizer;
    IdentitiesManager                   identitiesManager;
};

RecognitionWorker::RecognitionWorker(FacePipeline::Private* const dd)
    : d(new Private(dd))
{
    // TODO pull data from db
    QVector<FaceEmbeddingData> data = FaceEmbeddingManager().getFaceEmbeddings();

    data = reduceDimension(data, 4);

    cv::Mat predictors, labels;

    for (int i = 0; i < data.size(); ++i)
    {
        d->faceembeddingMap[data[i].tagId] = data[i];

        if (data[i].identity >= 0)
        {
            predictors.push_back(data[i].embedding);
            labels.push_back(data[i].identity);
        }
    }

    d->recognizer = new OpenCVDNNFaceRecognizer(cv::ml::TrainData::create(predictors, 0, labels));
}

RecognitionWorker::~RecognitionWorker()
{
    wait();    // protect database$
    delete d;
}

void RecognitionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    QVector<cv::Mat> embeddings;

    foreach (const FacePipelineFaceTagsIface& face, package->databaseFaces)
    {
        QString tagID = ExtractionWorker::encodeTagID(package->info.id(), face);
        embeddings << d->faceembeddingMap[tagID].embedding;
    } 

    QVector<int> identitiesIDs = d->recognizer->recognize(embeddings);

    for (int i = 0; i < identitiesIDs.size(); ++i)
    {
        package->recognitionResults << d->identitiesManager.identity(identitiesIDs[i]);
    }

    package->processFlags       |= FacePipelinePackage::ProcessedByRecognizer;

    emit processed(package);
}

void RecognitionWorker::setThreshold(double threshold, bool)
{
    d->recognizer->setThreshold(threshold);
}

void RecognitionWorker::aboutToDeactivate()
{
    d->imageRetriever.cancel();
}

} // namespace Digikam
