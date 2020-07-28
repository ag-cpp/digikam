/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-05-22
 * Description : Wrapper of face recognition using OpenFace
 *
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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


#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

// Qt include

#include <QImage>

// Local library

#include "digikam_opencv.h"
#include "digikam_export.h"
#include "identity.h"

using namespace Digikam;

namespace RecognitionTest
{
// TODO remove export macro after testing
class DIGIKAM_GUI_EXPORT FaceRecognizer
{
public:

    enum Classifier
    {
        SVM = 0,
        OpenCV_KNN,
        Tree,
        DB,
    };

    /**
     *  @brief FaceRecognizer:Master class to control entire recognition using OpenFace algorithm
     */
    explicit FaceRecognizer(Classifier method = Tree);
    ~FaceRecognizer();

public:

    /**
     *  Returns a cvMat created from the inputImage, optimized for recognition
     */
    cv::Mat prepareForRecognition(const QImage& inputImage);

    /**
     * @brief insertData: insert new data to recognizer
     * @param position
     * @param label
     * @param context
     * @return
     */
    bool insertData(const cv::Mat& position, const int label, const QString& context = QString());

    /**
     * @brief train : register data corresponding an identity to recognizer
     * @param images
     * @param label
     * @param context
     */
    void train(const QList<QImage>& images, const int label, const QString& context);

    /**
     * Try to recognize the given image.
     * Returns the identity id.
     * If the identity cannot be recognized, returns -1.
     */
    // TODO verify workflow to economize this routine
    int recognize(const QImage& inputImage);

    QMap<double, QVector<int> > getClosestNodes(const cv::Mat& position,
                                                double sqRange,
                                                int maxNbNeighbors);

    void setNbNeighBors(int k);

private:

    /// Hidden copy constructor and assignment operator.
    FaceRecognizer(const FaceRecognizer&);
    FaceRecognizer& operator=(const FaceRecognizer&);

    class Private;
    Private* d;
};

}
#endif // FACERECOGNIZER_H
