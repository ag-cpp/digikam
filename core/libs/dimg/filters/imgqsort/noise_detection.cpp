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

#include "noise_detection.h"

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
    qInfo()<<"prepare for detection";
    d->image = prepareForDetection(image);
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

    cvImage.convertTo(cvImage,CV_32F);

    return cvImage;
}

NoiseDetector::Mat3D NoiseDetector::get_haarMat()
{
    qInfo()<<"get base haar";
    Mat3D res;

    const int size = d->size_filter;

    float mat_base[4][4] = {{0.5, 0.5, 0.5, 0.5},
                            {0.5, 0.5, -0.5, -0.5},
                            {0.70711,-0.70711, 0., 0.},
                            {0.0, 0., 0.70711, -0.70711}}; 
    
    for (int i = 0; i < d->size_filter; i++)
    {
        for (int j = 0; j < d->size_filter; j++)
        {
            float tmp[size][size];
            
            for (int h = 0; h < d->size_filter; h++)
            {
                for (int l = 0; l < d->size_filter; l++)
                {
                    tmp[h][l] = mat_base[i][h] * mat_base[j][l];
                }
            }
            cv::Mat filter_i = cv::Mat(d->size_filter,d->size_filter,CV_32FC1);
            
            std::memcpy(filter_i.data, tmp, size * size * sizeof(float));
            
            res.push_back(filter_i);
        }   
    }
    return res;
}

float NoiseDetector::detect()
{ 
    qInfo()<<"start detection";
    
    Mat3D fltrs = get_haarMat();

    // Decompose to channels

    Mat3D channels = decompose_by_filter(fltrs);
    
    // Calculate variance and kurtosis

    cv::Mat variance, kurtosis ;

    calculate_variance_kurtosis(channels, variance, kurtosis);

    // Calculate variance

    float V =  noise_variance(variance,kurtosis) ;

    qInfo()<<"variance noise"<<V;

    return V;
}

NoiseDetector::Mat3D NoiseDetector::decompose_by_filter(const Mat3D& filters)
{
    Mat3D channels;

    Mat3D filtersUsed = filters.mid(1); // do not use first filter
    
    for (const auto filter: filtersUsed)
    {
        cv::Mat tmp = cv::Mat(d->image.size().width, d->image.size().height, CV_32FC1 );
        
        cv::filter2D(d->image, tmp, -1, filter, cv::Point(-1,-1), 0.0, cv::BORDER_REPLICATE);
        
        channels.push_back(tmp);
    }

    return channels;
}

void NoiseDetector::calculate_variance_kurtosis(const Mat3D& channels, cv::Mat& variance, cv::Mat& kurtosis)
{
    // Get raw moments
    
    cv::Mat mu1 = raw_moment(channels,1) ;
    cv::Mat mu2 = raw_moment(channels,2) ;
    cv::Mat mu3 = raw_moment(channels,3) ;
    cv::Mat mu4 = raw_moment(channels,4) ;
    

    // Calculate variance and kurtosis projection
    
    variance = mu2 - pow_mat(mu1,2);

    kurtosis = (mu4 - 4.0*mu1.mul(mu3) + 6.0*pow_mat(mu1,2).mul(mu2) - 3.0*pow_mat(mu1,4)) / pow_mat(variance,2)-3.0;
}

float NoiseDetector::noise_variance(const cv::Mat& variance, const cv::Mat& kurtosis)
{
    cv::Mat sqrt_kurtosis;
    
    cv::sqrt(kurtosis, sqrt_kurtosis);

    cv::Scalar mean, stddev;
    
    float a = mean_mat(sqrt_kurtosis);

    float b = mean_mat(pow_mat(variance,-1));

    float c = mean_mat(pow_mat(variance,-2));

    float d = mean_mat(sqrt_kurtosis.mul(pow_mat(variance,-1)));

    float sqrtK = (a*c - b*d)/(c-b*b);

    float V = (1.0 - a/sqrtK)/b;

    return V;
}

cv::Mat NoiseDetector::raw_moment(const NoiseDetector::Mat3D& mat,int ordre)
{
    qInfo()<<"get raw moment";

    float taille_image = d->image.size().width * d->image.size().height;
    
    std::vector<float> vec;
    
    for (const auto mat2d : mat)
    {
        vec.push_back(cv::sum(pow_mat(mat2d,ordre))[0] / taille_image);
    }
    
    return cv::Mat(vec, true);
}

cv::Mat NoiseDetector::pow_mat(const cv::Mat& mat, float ordre)
{
    cv::Mat res = cv::Mat(d->image.size().width, d->image.size().height, CV_32FC1 );
    
    cv::pow(mat, ordre, res);

    return res;
}

float NoiseDetector::mean_mat(const cv::Mat& mat)
{
    cv::Scalar mean, stddev;
    
    cv::meanStdDev(mat,mean,stddev);

    return mean[0];
}

}