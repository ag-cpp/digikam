/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2017-05-22
 * Description : Face recognition using deep learning
 *
 * Copyright (C) 2017      by Yingjie Liu <yingjiewudi at gmail dot com>
 * Copyright (C) 2017-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#ifndef DIGIKAM_DNN_FACE_MODEL_H
#define DIGIKAM_DNN_FACE_MODEL_H

// Qt include

#include <QList>

// Local includes

#include "digikam_opencv.h"
#include "opencvmatdata.h"
#include "facerec_dnnborrowed.h"
#include "dnnfaceextractor.h"

namespace Digikam
{

class DNNFaceVecMetadata
{
public:

    enum StorageStatus
    {
        Created,
        InDatabase
    };

public:

    explicit DNNFaceVecMetadata();
    ~DNNFaceVecMetadata();

public:

    int           databaseId;
    int           identity;
    QString       context;

    StorageStatus storageStatus;
};

// -------------------------------------------------------------------------------------------------------------------------------------

class DNNFaceModel : public cv::Ptr<DNNFaceRecognizer>
{
public:

    explicit DNNFaceModel();
    ~DNNFaceModel();

    DNNFaceRecognizer*       ptr();
    const DNNFaceRecognizer* ptr()                              const;

    /// Getter functions
    std::vector<std::vector<float> > getSrc()                   const;
    void setSrc(std::vector<std::vector<float> > new_src);

    cv::Mat getLabels()                                         const;
    void setLabels(cv::Mat new_labels);

    QList<DNNFaceVecMetadata> vecMetadata()                     const;
    std::vector<float>        vecData(int index)                const;

    void setWrittenToDatabase(int index, int databaseId);

    void setMats(const QList<std::vector<float> >& mats,
                 const QList<DNNFaceVecMetadata>& matMetadata);

    /// Make sure to call this instead of FaceRecognizer::update directly!
    void update(const std::vector<cv::Mat>& images,
                const std::vector<int>& labels,
                const QString& context,
                DNNFaceExtractor* const extractor);

protected:

    QList<DNNFaceVecMetadata> m_vecMetadata;
};

} // namespace Digikam

#endif // DIGIKAM_DNN_FACE_MODEL_H
