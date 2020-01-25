/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2017-07-13
 * Description : Face recognition using deep learning
 *
 * Copyright (C) 2017      by Yingjie Liu <yingjiewudi at gmail dot com>
 * Copyright (C) 2017-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_OPENCV_DNN_FACE_RECOGNIZER_H
#define DIGIKAM_OPENCV_DNN_FACE_RECOGNIZER_H

// Qt include

#include <QImage>

// Local library

#include "digikam_opencv.h"

namespace Digikam
{

class OpenCVDNNFaceRecognizer
{

public:

    /**
     *  @brief FaceRecognizer:Master class to control entire recognition using Eigenfaces algorithm
     */
    explicit OpenCVDNNFaceRecognizer(bool debug=false);
    ~OpenCVDNNFaceRecognizer();

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

    /**
     *  Try to recognize the given image.
     *  Returns the identity id.
     *  If the identity cannot be recognized, returns -1.
     */
    int recognize(const cv::Mat& inputImage);

    /**
     *  Low-level codes to cluster images.
     */
    void cluster(const std::vector<cv::Mat>& images,
                 std::vector<int>& clusteredIndices,
                 QStringList dataset,
                 int nbOfClusters);

    /**
     *  Trains the given images, representing faces of the given matched identities.
     */
    void train(const std::vector<cv::Mat>& images,
               const std::vector<int>& labels,
               const QString& context,
               const std::vector<cv::Mat>& images_rgb);

public:

    static float m_threshold;

private:

    /// Hidden copy constructor and assignment operator.
    OpenCVDNNFaceRecognizer(const OpenCVDNNFaceRecognizer&);
    OpenCVDNNFaceRecognizer& operator=(const OpenCVDNNFaceRecognizer&);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_OPENCV_DNN_FACE_RECOGNIZER_H
