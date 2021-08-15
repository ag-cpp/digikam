/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
 *
 * Copyright (C) 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "opencvdnnfacerecognizer_p.h"

namespace Digikam
{

OpenCVDNNFaceRecognizer::OpenCVDNNFaceRecognizer(cv::Ptr<cv::ml::TrainData> data, Classifier method)
    : d(new Private(data, method))
{
}

OpenCVDNNFaceRecognizer::~OpenCVDNNFaceRecognizer()
{
    delete d;
}

void OpenCVDNNFaceRecognizer::setNbNeighbors(int k)
{
    d->kNeighbors = k;
}

void OpenCVDNNFaceRecognizer::setThreshold(float threshold)
{
    d->threshold = threshold;
}

QVector<int> OpenCVDNNFaceRecognizer::recognize(const QVector<cv::Mat>& inputEmbeddings)
{
    QVector<int> ids;

    cv::parallel_for_(cv::Range(0, inputEmbeddings.size()), Private::ParallelRecognizer(d, inputEmbeddings, ids));

    return ids;
}

} // namespace Digikam
