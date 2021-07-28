/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - blur detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * References  : 
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

#ifndef DIGIKAM_BLUR_DETECTOR_H
#define DIGIKAM_BLUR_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"
#include "detector.h"

namespace Digikam
{

class BlurDetector : public DetectorDistortion
{
public:

    BlurDetector(const DImg& image);
    explicit BlurDetector(const DetectorDistortion& detector, const DImg& image);
    ~BlurDetector();

    float detect() const;

private:
    cv::Mat prepareForDetection(const DImg& inputImage)         const;

    cv::Mat edgeDetection(const cv::Mat& image)                 const;
    cv::Mat detectDefocusMap(const cv::Mat& edgesMap)           const;
    cv::Mat detectMotionBlurMap(const cv::Mat& edgesMap)        const;
    bool    isMotionBlur(const cv::Mat& frag)                   const;

    bool    haveFocusRegion(const DImg& image)                  const;
    cv::Mat detectBackgroundRegion(const cv::Mat& image)        const;
    cv::Mat getWeightMap()                                      const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BLUR_DETECTOR_H