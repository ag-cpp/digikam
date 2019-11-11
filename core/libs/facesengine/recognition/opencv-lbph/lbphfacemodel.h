/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-03-03
 * Description : https://docs.opencv.org/modules/contrib/doc/facerec/facerec_tutorial.html#local-binary-patterns-histograms
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

#ifndef DIGIKAM_FACESENGINE_LBPH_FACE_MODEL_H
#define DIGIKAM_FACESENGINE_LBPH_FACE_MODEL_H

// Qt include

#include <QList>

// Local includes

#include "digikam_opencv.h"
#include "opencvmatdata.h"
#include "facerec_borrowed.h"

namespace Digikam
{

class LBPHistogramMetadata
{
public:

    enum StorageStatus
    {
        Created,
        InDatabase
    };

public:

    explicit LBPHistogramMetadata();
    ~LBPHistogramMetadata();

public:

    int           databaseId;
    int           identity;
    QString       context;

    StorageStatus storageStatus;
};

// -------------------------------------------------------------------------------------------------------------------------------------

class LBPHFaceModel : public cv::Ptr<LBPHFaceRecognizer>
{
public:

    explicit LBPHFaceModel();
    ~LBPHFaceModel();

    LBPHFaceRecognizer*       ptr();
    const LBPHFaceRecognizer* ptr()                      const;

    int  radius()                                        const;
    void setRadius(int radius);

    int  neighbors()                                     const;
    void setNeighbors(int neighbors);

    int  gridX()                                         const;
    void setGridX(int grid_x);

    int  gridY()                                         const;
    void setGridY(int grid_y);

    QList<LBPHistogramMetadata> histogramMetadata()      const;
    OpenCVMatData               histogramData(int index) const;

    void setWrittenToDatabase(int index, int databaseId);

    void setHistograms(const QList<OpenCVMatData>& histograms, const QList<LBPHistogramMetadata>& histogramMetadata);

    /// Make sure to call this instead of FaceRecognizer::update directly!
    void update(const std::vector<cv::Mat>& images, const std::vector<int>& labels, const QString& context);

public:

    int databaseId;

protected:

    QList<LBPHistogramMetadata> m_histogramMetadata;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_LBPH_FACE_MODEL_H
