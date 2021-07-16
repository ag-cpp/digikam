/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - noise detection
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

#ifndef DIGIKAM_EXPOSURE_DETECTOR_H
#define DIGIKAM_EXPOSURE_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"

namespace Digikam
{

class ExposureDetector 
{
public:

    explicit ExposureDetector(const DImg& image);
    ~ExposureDetector();

    float detect();

private:

    cv::Mat prepareForDetection(const DImg& inputImage) const;

    float percent_underexposed();
    float percent_overexposed();

    int count_by_condition(int minVal = 0, int maxVal = 255);
    
private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXPOSURE_DETECTOR_H