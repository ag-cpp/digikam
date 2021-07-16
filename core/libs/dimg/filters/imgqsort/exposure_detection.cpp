/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - Noise detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * References  : https://cse.buffalo.edu/~siweilyu/papers/ijcv14.pdf
 * 
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

#include "exposure_detection.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExposureDetector::Private
{
public:
    explicit Private()
      : threshold_overexposed(245),
        threshold_demi_overexposed(235),
        threshold_underexposed(15),
        threshold_demi_underexposed(30),
        weight_over_exposure(15),
        weight_demi_over_exposure(1),
        weight_under_exposure(15),
        weight_demi_under_exposure(6)
    {
    }

    cv::Mat image;

    int threshold_overexposed;
    int threshold_demi_overexposed;
    int threshold_underexposed;
    int threshold_demi_underexposed;

    int weight_over_exposure;
    int weight_demi_over_exposure;
    
    int weight_under_exposure;
    int weight_demi_under_exposure;

};

ExposureDetector::ExposureDetector(const DImg& image)
    :  d(new Private)
{
    qInfo()<<"prepare for detection";

    d->image = prepareForDetection(image);
}

ExposureDetector::~ExposureDetector()
{
    delete d;
}

// Maybe this function will move to read_image() of imagequalityparser 
// in case all detector of IQS use cv::Mat
cv::Mat ExposureDetector::prepareForDetection(const DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    int type               = inputImage.sixteenBit() ? CV_16UC4 : CV_8UC4;
    cv::Mat cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), type, inputImage.bits());

    cv::cvtColor(cvImageWrapper, cvImage, cv::COLOR_RGBA2BGR);

    if (type == CV_16UC4)
    {
        cvImage.convertTo(cvImage, CV_8UC3, 1 / 256.0);
    }

    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);

    // std::cout<<cvImage;

    return cvImage;
}

void ExposureDetector::detect(float& overexposed, float& underexposed)
{ 
    qInfo()<<"start detection";

    overexposed = percent_overexposed();
    
    qInfo()<<"overexposed"<<overexposed;

    underexposed = percent_underexposed();

    qInfo()<<"underexposed"<<underexposed;
}

float ExposureDetector::percent_overexposed()
{
    int over_exposed_pixel = count_by_condition(d->threshold_overexposed, 255);

    int demi_over_exposed_pixel = count_by_condition(d->threshold_demi_overexposed,d->threshold_overexposed);

    int normal_pixel = d->image.total() - over_exposed_pixel - demi_over_exposed_pixel;

    return static_cast<float>(static_cast<float>(over_exposed_pixel * d->weight_over_exposure + demi_over_exposed_pixel * d->weight_demi_over_exposure) / 
                              static_cast<float>(normal_pixel + over_exposed_pixel * d->weight_over_exposure + demi_over_exposed_pixel * d->weight_demi_over_exposure));
}

float ExposureDetector::percent_underexposed()
{
    int under_exposed_pixel = count_by_condition(0,d->threshold_underexposed);

    int demi_under_exposed_pixel = count_by_condition(d->threshold_underexposed,d->threshold_demi_underexposed);

    int normal_pixel = d->image.total() - under_exposed_pixel - demi_under_exposed_pixel;

    return static_cast<float>(static_cast<float>(under_exposed_pixel * d->weight_under_exposure + demi_under_exposed_pixel * d->weight_demi_under_exposure) / 
                              static_cast<float>(normal_pixel + under_exposed_pixel * d->weight_under_exposure + demi_under_exposed_pixel * d->weight_demi_under_exposure));
}

int ExposureDetector::count_by_condition(int minVal, int maxVal)
{
    cv::Mat mat;

    cv::threshold(d->image,mat,minVal, 0, cv::THRESH_TOZERO );
    cv::threshold(mat     ,mat,maxVal, 0, cv::THRESH_TOZERO_INV );

    return cv::countNonZero(mat);
}

}
