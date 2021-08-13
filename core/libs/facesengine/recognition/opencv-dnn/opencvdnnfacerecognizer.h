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

#ifndef OPENCV_DNN_FACERECOGNIZER_H
#define OPENCV_DNN_FACERECOGNIZER_H

#include <QVector>

// Local includes
#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

struct Q_DECL_HIDDEN FaceEmbeddingData
{
    int id;
    cv::Mat embedding;
    int identity;
    QString tagId;
};

class DIGIKAM_GUI_EXPORT OpenCVDNNFaceRecognizer
{
public:

    enum Classifier
    {
        SVM = 0,
        KNN,
    };

    /**
     *  @brief OpenCVDNNFaceRecognizer:Master class to control entire recognition using OpenFace algorithm
     */
    explicit OpenCVDNNFaceRecognizer(cv::Ptr<cv::ml::TrainData> data, Classifier method = KNN);
    ~OpenCVDNNFaceRecognizer();

public:

    /**
     * Try to recognize a list of given face embeddings.
     * Returns a list of identity ids.
     * If an identity cannot be recognized, returns -1.
     */
    QVector<int> recognize(const QVector<cv::Mat>& inputEmbeddings);

    /**
     * set K parameter of K-Nearest neighbors algorithm
     */
    void setNbNeighbors(int k);

    /**
     * set maximum square distance of 2 vector
     */
    void setThreshold(float threshold);

private:

    // Disable
    OpenCVDNNFaceRecognizer(const OpenCVDNNFaceRecognizer&)            = delete;
    OpenCVDNNFaceRecognizer& operator=(const OpenCVDNNFaceRecognizer&) = delete;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif // OPENCV_DNN_FACERECOGNIZER_H
