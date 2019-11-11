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

#ifndef DIGIKAM_FACESENGINE_FISHER_FACE_MODEL_H
#define DIGIKAM_FACESENGINE_FISHER_FACE_MODEL_H

// Qt include

#include <QList>

// Local includes

#include "digikam_opencv.h"
#include "opencvmatdata.h"
#include "facerec_fisherborrowed.h"

namespace Digikam
{

class FisherFaceMatMetadata
{
public:

    enum StorageStatus
    {
        Created,
        InDatabase
    };

public:

    explicit FisherFaceMatMetadata();
    ~FisherFaceMatMetadata();

public:

    int           databaseId;
    int           identity;
    QString       context;

    StorageStatus storageStatus;
};

// -------------------------------------------------------------------------------------------------------------------------------------

class FisherFaceModel : public cv::Ptr<FisherFaceRecognizer>
{
public:

    explicit FisherFaceModel();
    ~FisherFaceModel();

    FisherFaceRecognizer*       ptr();
    const FisherFaceRecognizer* ptr()               const;

    // Getter function
    std::vector<cv::Mat> getSrc()                   const;
    void setSrc(std::vector<cv::Mat> new_src);

    cv::Mat getLabels()                             const;
    void setLabels(cv::Mat new_labels);

    QList<FisherFaceMatMetadata> matMetadata()      const;
    OpenCVMatData                matData(int index) const;

    void setWrittenToDatabase(int index, int databaseId);

    void setMats(const QList<OpenCVMatData>& mats, const QList<FisherFaceMatMetadata>& matMetadata);

    /// Make sure to call this instead of FaceRecognizer::update directly!
    void update(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context);

protected:

    QList<FisherFaceMatMetadata> m_matMetadata;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_FISHER_FACE_MODEL_H
