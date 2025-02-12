/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-22
 * Description : Class to perform faces detection using OpenCV DNN module
 *
 * SPDX-FileCopyrightText: 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QImage>
#include <QList>
#include <QRect>

// Local includes

#include "digikam_opencv.h"
#include "dimg.h"
#include "dnnfacedetectorbase.h"

namespace Digikam
{

enum DetectorNNModel
{
    SSDMOBILENET = 0,   ///< SSD MobileNet neural network inference.
    YOLO,               ///< YOLO neural network inference.
    YUNET               ///< YuNet neural network inference.
};

class DIGIKAM_EXPORT OpenCVDNNFaceDetector
{

public:

    explicit OpenCVDNNFaceDetector(DetectorNNModel model = DetectorNNModel::SSDMOBILENET);
    ~OpenCVDNNFaceDetector();

    cv::Mat prepareForDetection(const DImg& inputImage, cv::Size& paddedSize)           const;
    cv::Mat prepareForDetection(const QImage& inputImage, cv::Size& paddedSize)         const;
    cv::Mat prepareForDetection(const QString& inputImagePath, cv::Size& paddedSize)    const;
    cv::Mat prepareForDetectionYuNet(cv::Mat& cvImage, cv::Size& paddedSize)            const;

    void setAccuracy(const float accuracy);

    QList<QRect> detectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize);
    std::vector<cv::Rect> cvDetectFaces(const cv::Mat& inputImage, const cv::Size& paddedSize);

    /**
     * Returns the image size (one dimension)
     * recommended for face detection. If the image is considerably larger, it will be rescaled automatically.
     */
    static int recommendedImageSizeForDetection();

private:

    cv::Mat prepareForDetection(cv::Mat& cvImage, cv::Size& paddedSize)              const;

private:

    // Disable
    OpenCVDNNFaceDetector(const OpenCVDNNFaceDetector&)            = delete;
    OpenCVDNNFaceDetector& operator=(const OpenCVDNNFaceDetector&) = delete;

private:

    DetectorNNModel      m_modelType;
    DNNFaceDetectorBase* m_inferenceEngine;

};

} // namespace Digikam
