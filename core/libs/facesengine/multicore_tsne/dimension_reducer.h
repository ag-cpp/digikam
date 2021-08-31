/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-07-04
 * Description : Wrapper class for dimension reduction
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_FACESENGINE_DIMENSIONREDUCER_H
#define DIGIKAM_FACESENGINE_DIMENSIONREDUCER_H

// Local includes

#include "digikam_opencv.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DimensionReducer
{
public:

    static cv::Mat reduceDimension(cv::Mat data, int outputDimension, int nbCPU);
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_DIMENSIONREDUCER_H
