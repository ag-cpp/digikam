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

#include "noise_detector.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{
class Q_DECL_HIDDEN NoiseDetector::Private
{
public:
    explicit Private()
      : size_filter(4)
    {

    }

    cv::Mat image;

    int size_filter;

};

NoiseDetector::NoiseDetector(const DImg& image)
    :  d(new Private)
{
    d->image = prepareForDetection(image);

    d->have_focus_region = haveFocusRegion(image);
}

NoiseDetector::~NoiseDetector()
{
    delete d;
}

// Maybe this function will move to read_image() of imagequalityparser 
// in case all detector of IQS use cv::Mat
cv::Mat NoiseDetector::prepareForDetection(const DImg& inputImage) const
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

    cv::cvtColor( cvImage, cvImage, cv::COLOR_BGR2GRAY );

    return cvImage;
}

NoiseDetector::Mat3D NoiseDetector::get_haarMat()
{
    Mat3D res = mat3D;
    for (int i = 0; i < d->size_filter * d->size_filter; i++)
    {
        res.push_back(cv::Mat::zeros(cv::Size(d->size_filter, d->size_filter), CV_64FC1);)
    }

    float mat_base[4][4] = {{0.5, 0.5, 0.5, 0.5},
                            {0.5, 0.5, -0.5, -0.5},
                            {0.70711,-0.70711, 0., 0.},
                            {0.0, 0., 0.70711, -0.70711}}; 
    cv::Mat mat_base_haar = Mat(d->size_filter, d->size_filter, CV_64FC1, &mat_base);
    int k = 0;

    for (int i = 0; i < d->size_filter; i++)
    {
        for (int j = 0; j < d->size_filter; j++)
        {
            for (int h = 0; h < d->size_filter; h++)
            {
                for (int l = 0; l < d->size_filter; l++)
                {
                    mat_base_haar.at(k).at(h,l) = mat_base_haar.at(i,h) * mat_base_haar.at(j,l)
                }
            }
            k++;
        }
    }
}

float NoiseDetector::detect()
{ 
    Mat3D fltrs = get_haarMat(d->size_filter);

    // Decompose to kernel
    
    Mat3D channels;
    
    for (int i = 0; i < d->size_filter * d->size_filter -1; i++)
    {
        channels.push_back(cv::Mat::zeros(cv::Size(d->size_filter, d->size_filter), CV_64FC1);)
    }

    for (int k =1; k < d->size_filter * d->size_filter; k ++)
    {
        cv::filter2D(d->image, channels.at(0), -1, fltrs.at(k-1), Point(-1,-1), 5.0, BORDER_CONSTANT);
    }

    cv::Mat mu1 = raw_moment(channels,1);
    cv::Mat mu2 = raw_moment(channels,2);
    cv::Mat mu3 = raw_moment(channels,3);
    cv::Mat mu4 = raw_moment(channels,4);

    cv::Mat noiV = mu2 - mat_pow(mu1,2);

    cv::Mat noiK = (mu4 - 4*mu1*mu3 + 6*mat_pow(mu1,2).mul(mu2) - 3*mat_pow(mu1,4))/(mat_pow(noiV,2))-3;

    cv::Mat sqrt_noiK;
    
    cv::sqrt(noiK, sqrt_noiK);

    cv::Scalar mean, stddev;

    cv::meanStdDev(sqrt_noiK,mean,stddev);
    float a = mean[0];

    cv::meanStdDev(mat_pow(noiV,-1),mean,stddev);
    float b = mean[0];

    cv::meanStdDev(mat_pow(noiV,-2),mean,stddev);
    float c = mean[0];

    cv::meanStdDev(sqrt_noiK.mul(mat_pow(noiV),-1),mean,stddev);
    float d = mean[0];

    float sqrtK = (a*c - b*d)/(c-b*b);

    float V = (1 - a/sqrtK)/b;

    return V;
}

cv::Mat NoiseDetector::raw_moment(NoiseDetector::Mat3D mat,int ordre)
{
    std::list<float> list;
    
    for (const auto mat2d : mat)
    {
        float tmp = cv::sum(mat_pow(mat2d,ordre))[0] / (d->image.size.width * d->image.size.height);

        list.push_back(tmp);
    }
    
    cv::Mat res = cv::Mat(d->size_filter * d->size_filter, 1 , CV_64FC1 , list);
}

cv::Mat NoiseDetector::mat_pow(const cv::Mat mat, float ordre)
{
    cv::Mat res;
    
    cv::pow(mat, ordre, res);

    return res;
}

}