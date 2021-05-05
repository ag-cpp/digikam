/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 16/08/2016
 * Description : Full object detection class representing the output of the
 *               shape predictor class, containing 64 facial point including
 *               eye, nose, and mouth.
 *
 * Copyright (C)      2016 by Omar Amin <Omar dot moh dot amin at gmail dot com>
 * Copyright (C) 2016-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FULL_OBJECT_DETECTION_H
#define DIGIKAM_FULL_OBJECT_DETECTION_H

// C++ includes

#include <vector>

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FullObjectDetection
{
public:

    FullObjectDetection();
    explicit FullObjectDetection(const cv::Rect& rect_);
    FullObjectDetection(const cv::Rect& rect_,
                        const std::vector<std::vector<float> >& parts_);

    const cv::Rect& get_rect()                        const;

    cv::Rect& get_rect();

    unsigned long num_parts()                         const;

    const std::vector<float>& part(unsigned long idx) const;

    std::vector<float>& part(unsigned long idx);

private:

    cv::Rect                         rect;
    std::vector<std::vector<float> > parts;
};

// -------------------------------------------------------------------

std::vector<cv::Rect> getEyes(const FullObjectDetection& shape);

} // namespace Digikam

#endif // DIGIKAM_FULL_OBJECT_DETECTION_H
