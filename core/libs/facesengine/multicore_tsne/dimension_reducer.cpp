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

    return projectedData;
}

cv::Mat DimensionReducer::reduceDimension(cv::Mat trainData, cv::Mat newData, int outputDimension, int nbCPU)
{
    cv::Mat buffer = trainData.clone();
    buffer.push_back(newData);

    cv::Mat projectedData(buffer.rows, outputDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(buffer.data),
                         buffer.rows,
                         buffer.cols,
                         reinterpret_cast<float*>(projectedData.data),
                         outputDimension,
                         30,
                         0.5,
                         nbCPU);

    cv::Mat output;
    projectedData(cv::Range(std::max(0, projectedData.rows - newData.rows), projectedData.rows), cv::Range(0, projectedData.cols)).copyTo(output);

    return output;
}

// -------------------------------------------------------------

class DimensionReducer::Private
{
public:

    explicit Private(int capacity, int targetNbDimension, int nbCPU)
        : capacity         (capacity),
          targetNbDimension(targetNbDimension),
          nbCPU            (nbCPU)
    {
    }

public:
    cv::Mat buffer;
    int     capacity;
    int     targetNbDimension;
    int     nbCPU;
};

DimensionReducer::DimensionReducer(int bufferCapacity, int targetNbDimension, int nbCore)
    : d(new Private(bufferCapacity, targetNbDimension, nbCore))
{
}

DimensionReducer::~DimensionReducer()
{
    delete d;
}

cv::Mat DimensionReducer::project(cv::Mat data) 
{
    d->buffer = d->buffer(cv::Range(std::max(0, d->buffer.rows - d->capacity + data.rows), d->buffer.rows), cv::Range(0, d->buffer.cols)); 
    d->buffer.push_back(data);

    cv::Mat tempBuffer;
    cv::Mat projectedData(tempBuffer.rows, d->targetNbDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(tempBuffer.data),
                          tempBuffer.rows,
                          tempBuffer.cols,
                          reinterpret_cast<float*>(projectedData.data),
                          d->targetNbDimension,
                          30,
                          0.5,
                          d->nbCPU);

    cv::Mat output;
    projectedData(cv::Range(std::max(0, projectedData.rows - data.rows), projectedData.rows), cv::Range(0, projectedData.cols)).copyTo(output);

    d->buffer = tempBuffer;

    return output;
}

} // namespace Digikam
