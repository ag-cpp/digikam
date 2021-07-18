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

#include "noise_detection.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

const int SIZE_FILTER = 4;

// Band-pass filter Haar

MatrixFilterHaar::MatrixFilterHaar()
    : QObject(),
      m_isInit(false)
{
}

MatrixFilterHaar::~MatrixFilterHaar()
{
}

void MatrixFilterHaar::init()
{
    if (!m_isInit)
    {
        NoiseDetector::Mat3D res;

        const int size = SIZE_FILTER;

        float mat_base[size][size] = {{0.5, 0.5, 0.5, 0.5},
                                      {0.5, 0.5, -0.5, -0.5},
                                      {sqrt(2.0)/2.0,-sqrt(2.0)/2.0, 0., 0.},
                                      {0., 0., sqrt(2.0)/2.0, -sqrt(2.0)/2.0}}; 
        
        cv::Mat mat_base_opencv = cv::Mat(SIZE_FILTER, SIZE_FILTER, CV_32FC1, &mat_base);

        for (int i = 0; i < SIZE_FILTER; i++)
        {
            for (int j = 0; j < SIZE_FILTER; j++)
            {
                res.push_back(mat_base_opencv.row(i).t() * mat_base_opencv.row(j));
            }   
        }
        m_data = res;

        m_isInit = true;
    }
}

MatrixFilterHaar* MatrixFilterHaar::instance()
{
    static MatrixFilterHaar* instance = new MatrixFilterHaar();

    return instance;
}

NoiseDetector::Mat3D MatrixFilterHaar::get_data()
{
    return m_data;
}

class Q_DECL_HIDDEN NoiseDetector::Private
{
public:
    explicit Private()
      : size_filter(4),
        alpha(18.0),
        beta(7.0)
    {
    }

    cv::Mat image;

    int size_filter;

    float alpha;
    float beta;

};

// Main noise detection
NoiseDetector::NoiseDetector(const DImg& image)
    :  d(new Private)
{
    d->image = prepareForDetection(image);

    MatrixFilterHaar::instance()->init();
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

float NoiseDetector::detect() const
{    
    Mat3D fltrs = MatrixFilterHaar::instance()->get_data();

    // Decompose to channels

    Mat3D channels = decompose_by_filter(fltrs);
    
    // Calculate variance and kurtosis

    cv::Mat variance, kurtosis ;

    calculate_variance_kurtosis(channels, variance, kurtosis);

    // Calculate variance of noise

    float V =  noise_variance(variance,kurtosis) ;
    qInfo()<<"V"<<V;

    return normalize(V);
}

NoiseDetector::Mat3D NoiseDetector::decompose_by_filter(const Mat3D& filters) const
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

void NoiseDetector::calculate_variance_kurtosis(const Mat3D& channels, cv::Mat& variance, cv::Mat& kurtosis) const
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

float NoiseDetector::noise_variance(const cv::Mat& variance, const cv::Mat& kurtosis) const
{
    cv::Mat sqrt_kurtosis;
    
    cv::sqrt(kurtosis, sqrt_kurtosis);

    float a = mean_mat(sqrt_kurtosis);

    float b = mean_mat(pow_mat(variance,-1));

    float c = mean_mat(pow_mat(variance,-2));

    float d = mean_mat(sqrt_kurtosis.mul(pow_mat(variance,-1)));

    float sqrtK = (a*c - b*d)/(c-b*b);

    return (1.0 - a/sqrtK)/b;
}

cv::Mat NoiseDetector::raw_moment(const NoiseDetector::Mat3D& mat,int ordre) const
{
    float taille_image = d->image.size().width * d->image.size().height;
    
    std::vector<float> vec;
    
    for (const auto mat2d : mat)
    {
        vec.push_back(cv::sum(pow_mat(mat2d,ordre))[0] / taille_image);
    }
    
    return cv::Mat(vec, true);
}

cv::Mat NoiseDetector::pow_mat(const cv::Mat& mat, float ordre) const
{
    cv::Mat res = cv::Mat(d->image.size().width, d->image.size().height, CV_32FC1 );
    
    cv::pow(mat, ordre, res);

    return res;
}

float NoiseDetector::mean_mat(const cv::Mat& mat) const
{
    cv::Scalar mean, stddev;
    
    cv::meanStdDev(mat,mean,stddev);

    return mean[0];
}

// Normalize result to interval [0 - 1]
float NoiseDetector::normalize(const float number) const
{
    return 1.0 / (1.0 + qExp(-(number - d->alpha)/d->beta ));
}

}
