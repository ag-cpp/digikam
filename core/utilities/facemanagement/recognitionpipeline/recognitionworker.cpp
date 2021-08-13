/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-11
 * Description : Integrated, multithread face recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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
#include "dimension_reducer.h"
#include "opencvdnnfacerecognizer.h"

namespace Digikam
{

class RecognitionWorker::Private
{
public:

    explicit Private()
    {
    }

    ~Private()
    {
        delete recognizer;
    }

public:

    QMap<QString, FaceEmbeddingData> faceembeddingMap;
    OpenCVDNNFaceRecognizer* recognizer;
};

RecognitionWorker::RecognitionWorker(QVector<FaceEmbeddingData>& data, QObject* parent)
    : ActionJob(parent),
      d(new Private)
{
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
    delete d;
}


void RecognitionWorker::run()
{
    while (!m_cancel)
    {
        // TODO
    }
}

QVector<FaceEmbeddingData>& RecognitionWorker::reduceDimension(QVector<FaceEmbeddingData>& data, int nbCPU) const
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

} // namespace Digikam
