/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2017-06-10
 * Description : Face Recognition based on Fisherfaces
 *               https://docs.opencv.org/2.4/modules/contrib/doc/facerec/facerec_tutorial.html#Fisherfaces
 *               Turk, Matthew A and Pentland, Alex P. "Face recognition using Fisherfaces." 
 *               Computer Vision and Pattern Recognition, 1991. Proceedings {CVPR'91.},
 *               {IEEE} Computer Society Conference on 1991.
 *
 * Copyright (C) 2017      by Yingjie Liu <yingjiewudi at gmail dot com>
 * Copyright (C) 2017-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_OPENCV_FISHERFACE_RECOGNIZER_H
#define DIGIKAM_OPENCV_FISHERFACE_RECOGNIZER_H

// Qt include

#include <QImage>

// Local includes

#include "digikam_opencv.h"

namespace Digikam
{

class OpenCVFISHERFaceRecognizer
{

public:

    /**
     * @brief FaceRecognizer:Master class to control entire recognition using Fisherfaces algorithm
     */
    explicit OpenCVFISHERFaceRecognizer();
    ~OpenCVFISHERFaceRecognizer();

    void setThreshold(float threshold) const;

    /**
     * Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

    /**
     * Try to recognize the given image.
     * Returns the identity id.
     * If the identity cannot be recognized, returns -1.
     */
    int recognize(const cv::Mat& inputImage);

    /**
     * Trains the given images, representing faces of the given matched identities.
     */
    void train(const std::vector<cv::Mat>& images,
               const std::vector<int>& labels,
               const QString& context);

private:

    // Hidden copy constructor and assignment operator.
    OpenCVFISHERFaceRecognizer(const OpenCVFISHERFaceRecognizer&);
    OpenCVFISHERFaceRecognizer& operator=(const OpenCVFISHERFaceRecognizer&);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_OPENCV_FISHERFACE_RECOGNIZER_H
