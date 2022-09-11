/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Exposure detection
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXPOSURE_DETECTOR_H
#define DIGIKAM_EXPOSURE_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class ExposureDetector : public DetectorDistortion
{
    Q_OBJECT

public:

    explicit ExposureDetector();
    ~ExposureDetector();

    float detect(const cv::Mat& image)                  const override;

private:

    float percent_underexposed(const cv::Mat& image)    const;
    float percent_overexposed(const cv::Mat& image)     const;

    int count_by_condition(const cv::Mat& image,
                           int minVal, int maxVal)      const;

    explicit ExposureDetector(QObject*);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXPOSURE_DETECTOR_H
