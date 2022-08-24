/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Aesthetic detection
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#ifndef DIGIKAM_AESTHETIC_DETECTOR_H
#define DIGIKAM_AESTHETIC_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class AestheticDetector : public DetectorDistortion
{
    Q_OBJECT

public:

    explicit AestheticDetector();
    ~AestheticDetector();

    float detect(const cv::Mat& image)                          const override;

public:
    static cv::dnn::Net model;

private:

    explicit AestheticDetector(QObject*);

    cv::Mat preprocess(const cv::Mat& image)                    const;
    float postProcess(const cv::Mat& modelOutput)               const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_AESTHETIC_DETECTOR_H