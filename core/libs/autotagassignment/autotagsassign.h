/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2023-09-02
 * Description : Class for autotagging engine
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung TRAN <quochungtran1999 at gmail dot com>
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
    std::vector<cv::Mat> prepareForDetection(const QList<QString>& inputImagePaths, int batchSize) const;

    QList<QString> generateTagsList(const QImage& inputImage);
    QList<QString> generateTagsList(const DImg& inputImage);
    QList<QString> generateTagsList(const QString& inputImagePath);
    
    /**
     * run in batch return the list of tags name corresponding to
     * NOTE: the batch size is fixed depending on the deep NN model we choose  
    */
    QList<QList<QString>> generateTagsList(const QList<QString>& inputImagePaths, int batchSize) const;

    QList<QString> getPredefinedTagsPath() const;

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
