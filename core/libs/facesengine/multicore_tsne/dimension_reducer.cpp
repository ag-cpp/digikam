/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-07-04
 * Description : Wrapper class for dimension reduction
 *
 * Copyright (C)      2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "dimension_reducer.h"

// Local includes

#include "tsne.h"

namespace Digikam
{

cv::Mat DimensionReducer::reduceDimension(cv::Mat data, int outputDimension, int nbCPU)
{
    cv::Mat projectedData(data.rows, outputDimension, CV_32F);

    TSNE::tsne_run_float(reinterpret_cast<float*>(data.data),
                          data.rows,
                          data.cols,
                          reinterpret_cast<float*>(projectedData.data),
                          outputDimension,
                          30,
                          0.5,
                          nbCPU);

    //cv::Mat normalizedData;
    //cv::normalize(projectedData, normalizedData, 1.0, 0.0, cv::NORM_L2);

    return projectedData;
}

} // namespace Digikam
