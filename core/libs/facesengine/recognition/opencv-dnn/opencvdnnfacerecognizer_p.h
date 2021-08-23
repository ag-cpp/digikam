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

#ifndef OPENCV_DNN_FACERECOGNIZER_P_H
#define OPENCV_DNN_FACERECOGNIZER_P_H

#include "opencvdnnfacerecognizer.h"

// C++ includes

#include <iostream>

// Qt includes

#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"
#include "kd_tree.h"

namespace Digikam
{

float getMaxDistance(const cv::Mat& vectors) 
{
    float maxD = 0.0;

    for (int i = 0; i < vectors.rows; ++i)
    {
        for (int j = i+1; j < vectors.rows; ++j)
        {
            maxD = std::max(maxD, KDNode::sqrDistance(vectors.row(i).ptr<float>(), vectors.row(j).ptr<float>(), vectors.cols));
        }
    }

    return maxD;
}

class Q_DECL_HIDDEN OpenCVDNNFaceRecognizer::Private
{
public:

    Private(cv::Ptr<cv::ml::TrainData> data, Classifier method)
        : method        (method),
          tree          (nullptr),
          kNeighbors    (1),
          threshold     (50)
    {
        switch (method)
        {
            case SVM:
            {
                svm = cv::ml::SVM::create();
                svm->setType(cv::ml::SVM::NU_SVC);
                svm->setNu(0.1);
                svm->setKernel(cv::ml::SVM::RBF);
                svm->setC(10);

                trainSVM(data);

                break;
            }

            case KNN:
            {
                trainKNN(data);
                break;
            }

            default:
            {
                qFatal("Invalid classifier");
            }
        }

        maxDistance = getMaxDistance(data->getSamples());
    }

    ~Private()
    {
        delete tree;
    }

public:

    bool trainSVM(cv::Ptr<cv::ml::TrainData> data);
    bool trainKNN(cv::Ptr<cv::ml::TrainData> data);

    int predictSVM(const cv::Mat& faceEmbedding);
    int predictKNN(const cv::Mat& faceEmbedding) const;

public:

    Classifier                 method;

    cv::Ptr<cv::ml::SVM>       svm;

    KDTree*                    tree;
    int                        kNeighbors;
    float                      threshold;
    float                      maxDistance;

public:

    class ParallelRecognizer;
};

class OpenCVDNNFaceRecognizer::Private::ParallelRecognizer : public cv::ParallelLoopBody
{
public:

    ParallelRecognizer(OpenCVDNNFaceRecognizer::Private* d,
                       const QVector<cv::Mat>& embeddings,
                       QVector<int>& ids)
        : embeddings (embeddings),
          ids        (ids),
          d          (d)
    {
        ids.resize(embeddings.size());
    }

    void operator()(const cv::Range& range) const override
    {
        for(int i = range.start ; i < range.end ; ++i)
        {
            int id = -1;

            switch (d->method)
            {
                case SVM:
                {
                    id = d->predictSVM(embeddings[i]);
                    break;
                }

                case KNN:
                {
                    id = d->predictKNN(embeddings[i]);
                    break;
                }

                default:
                {
                    qCWarning(DIGIKAM_FACEDB_LOG) << "Not recognized classifying method";
                }
            }

            ids[i] = id;
        }
    }

private:

    const QVector<cv::Mat>&                 embeddings;
    QVector<int>&                           ids;

    OpenCVDNNFaceRecognizer::Private* const d;

private:

    Q_DISABLE_COPY(ParallelRecognizer)
};

bool OpenCVDNNFaceRecognizer::Private::trainSVM(cv::Ptr<cv::ml::TrainData> data)
{
    QElapsedTimer timer;
    timer.start();

    svm->train(data);

    qCDebug(DIGIKAM_FACEDB_LOG) << "Support vector machine trains in" << timer.elapsed() << "ms";

    return (svm->isTrained());
}

bool OpenCVDNNFaceRecognizer::Private::trainKNN(cv::Ptr<cv::ml::TrainData> data)
{
    QElapsedTimer timer;
    timer.start();

    cv::Mat samples = data->getSamples();
    cv::Mat labels = data->getResponses();

    tree = new KDTree(samples.cols);
    for (int i = 0; i < samples.rows; ++i) 
    {
        tree->add(samples.row(i), labels.row(i).at<int>(0));
    }

    qCDebug(DIGIKAM_FACEDB_LOG) << "KNN trains in" << timer.elapsed() << "ms";

    return true;
}

int OpenCVDNNFaceRecognizer::Private::predictSVM(const cv::Mat& faceEmbedding)
{
    return (int(svm->predict(faceEmbedding)));
}

int OpenCVDNNFaceRecognizer::Private::predictKNN(const cv::Mat& faceEmbedding) const
{
    if (!tree)
    {
        return -1;
    }

    // Look for K-nearest neighbor which have the cosine distance greater than the threshold.
    float searchRange = (1 - threshold) * maxDistance;

    QMap<double, QVector<int> > closestNeighbors = tree->getClosestNeighbors(faceEmbedding, searchRange, kNeighbors);
    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                     iter != closestNeighbors.cend();
                                                   ++iter)
    {
        for (QVector<int>::const_iterator node  = iter.value().cbegin();
                                          node != iter.value().cend();
                                        ++node)
        {
            int label = (*node);
            votingGroups[label].append(iter.key());
        }
    }

    double maxScore = 0.0;
    int prediction  = -1;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                   ++group)
    {
        double score = 0.0;

        for (int i = 0 ; i < group.value().size() ; ++i)
        {
            score += (searchRange - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}

} // namespace Digikam

#endif // OPENCV_DNN_FACERECOGNIZER_P_H
