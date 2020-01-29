/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2013-05-26
 * Description : Facilitates cv::Mat storage in Face DB
 *               (where Qt interface requires Qt types).
 *
 * Copyright (C) 2013      by Marcel Wiesweg <marcel dot wiesweg at uk-essen dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_OPENCV_MAT_DATA_H
#define DIGIKAM_OPENCV_MAT_DATA_H

// Qt includes

#include <QByteArray>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT OpenCVMatData
{
public:

    OpenCVMatData();
    explicit OpenCVMatData(const cv::Mat& mat);

    /// Creates a deep copy of the QByteArray
    cv::Mat toMat() const;

    /// Creates a deep copy of the cv::Mat
    void setMat(const cv::Mat& mat);

    void clearData();

public:

    int        type;
    int        rows;
    int        cols;
    QByteArray data;
};

} // namespace Digikam

#endif // DIGIKAM_OPENCV_MAT_DATA_H
