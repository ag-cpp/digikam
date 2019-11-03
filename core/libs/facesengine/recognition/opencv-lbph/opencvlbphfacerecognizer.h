/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-03-03
 * Description : http://docs.opencv.org/modules/contrib/doc/facerec/facerec_tutorial.html#local-binary-patterns-histograms
 *               Ahonen T, Hadid A. and Pietikäinen M. "Face description with local binary
 *               patterns: Application to face recognition." IEEE Transactions on Pattern
 *               Analysis and Machine Intelligence, 28(12):2037-2041.
 *
 * Copyright (C) 2013      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2013 by Mahesh Hegde <maheshmhegade at gmail dot com>
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

#ifndef DIGIKAM_OPENCV_LBPH_FACE_RECOGNIZER_H
#define DIGIKAM_OPENCV_LBPH_FACE_RECOGNIZER_H

// Qt include

#include <QImage>

// Local includes

#include "digikam_opencv.h"
#include "opencvmatdata.h"

namespace Digikam
{

class OpenCVLBPHFaceRecognizer
{

public:

    /**
     * @brief FaceRecognizer:Master class to control entire recognition using LBPH algorithm
     */
    explicit OpenCVLBPHFaceRecognizer();
    ~OpenCVLBPHFaceRecognizer();

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

    OpenCVLBPHFaceRecognizer(const OpenCVLBPHFaceRecognizer&); // Disable

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_OPENCV_LBPH_FACE_RECOGNIZER_H
