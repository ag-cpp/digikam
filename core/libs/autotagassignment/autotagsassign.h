/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-07-22
 * Description : Class to perform faces detection using OpenCV DNN module
 *
 * SPDX-FileCopyrightText: 20 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H
#define DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H

// Qt includes

#include <QImage>
#include <QList>
#include <QRect>

// Local includes

#include "digikam_opencv.h"
#include "dimg.h"
#include "dnnbasedetectormodel.h"

namespace Digikam
{

enum DetectorModel
{
    YOLOV5NANO = 0,   ///< YOLO neural network inference.
    YOLOV5XLARGE
    // add another model       
};

class DIGIKAM_EXPORT autoTagsAssign
{

public:

    explicit autoTagsAssign(DetectorModel model = DetectorModel::YOLOV5NANO);
    ~autoTagsAssign();

    cv::Mat prepareForDetection(const DImg& inputImage)        const;
    cv::Mat prepareForDetection(const QImage& inputImage)      const;
    cv::Mat prepareForDetection(const QString& inputImagePath) const;

    QList<QString> generateTagsList(const QImage& inputImage);
    QList<QString> generateTagsList(const DImg& inputImage);
    QList<QString> generateTagsList(const QString& inputImagePath);

private:

    // Disable
    autoTagsAssign(const autoTagsAssign&)            = delete;
    autoTagsAssign& operator=(const autoTagsAssign&) = delete;

private:

    DetectorModel         m_modelType;
    DNNBaseDetectorModel* m_inferenceEngine;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_OPENCV_DNN_FACE_DETECTOR_H
