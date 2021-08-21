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
#include "asyncbuffer.h"

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

    qDebug() << "Reduce dimension";

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
        : imageRetriever(dd),
          buffer(1000),
          cancel(false)
    {
    }

    ~Private()
    {
        delete recognizer;
    }

    FaceItemRetriever                               imageRetriever;
    QMap<QString, FaceEmbeddingData>                faceembeddingMap;
    OpenCVDNNFaceRecognizer*                        recognizer;
    IdentitiesManager                               identitiesManager;
    AsyncBuffer<FacePipelineExtendedPackage::Ptr>   buffer;
    bool                                            cancel;
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
    delete d;
}

void RecognitionWorker::run()
{
    QVector<FaceEmbeddingData> data = FaceEmbeddingManager().getFaceEmbeddings();
    qDebug() << "Start projection";
    data = reduceDimension(data, 1);

    cv::Mat predictors, labels;

    for (int i = 0; i < data.size(); ++i)
    {
        d->faceembeddingMap[data[i].tagId] = data[i];

        qDebug() << data[i].identity << data[i].tagId << data[i].embedding.at<float>(0) << data[i].embedding.at<float>(1);

        if (data[i].identity >= 0)
        {
            predictors.push_back(data[i].embedding);
            labels.push_back(data[i].identity);
        }
    }

    qDebug() << "training size" << predictors.rows;

    d->recognizer = new OpenCVDNNFaceRecognizer(cv::ml::TrainData::create(predictors, 0, labels));
    d->recognizer->setThreshold(d->threshold);

    while (!d->cancel)
    {
        FacePipelineExtendedPackage::Ptr package = d->buffer.read();

        if (package == nullptr)
        {
            break;
        }

        QVector<cv::Mat> embeddings;

        foreach (const FacePipelineFaceTagsIface& face, package->databaseFaces)
        {
            QString tagID = ExtractionWorker::encodeTagID(package->info.id(), face);
            embeddings << d->faceembeddingMap[tagID].embedding;
            qDebug() << "recognize" << d->faceembeddingMap[tagID].embedding.at<float>(0) << d->faceembeddingMap[tagID].embedding.at<float>(1);
        } 

        QVector<int> identitiesIDs = d->recognizer->recognize(embeddings);
        qDebug() << "Recognition result" << identitiesIDs;

        for (int i = 0; i < identitiesIDs.size(); ++i)
        {
            package->recognitionResults << d->identitiesManager.identity(identitiesIDs[i]);
        }

        package->processFlags |= FacePipelinePackage::ProcessedByRecognizer;

        emit processed(package);
    }
}

void RecognitionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    d->buffer.append(package);
}

void RecognitionWorker::cancel()
{
    d->cancel = true;
    d->buffer.cancel();
    wait();
}

void RecognitionWorker::setThreshold(double threshold, bool)
{
    d->threshold = threshold;
}

} // namespace Digikam
