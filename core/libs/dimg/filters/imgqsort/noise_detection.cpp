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

    // Decompose to kernel
    
    cv::Mat image = d->image;
    image.convertTo(image,CV_32F);

    Mat3D channels;
    
    for (int k =1; k < d->size_filter * d->size_filter; k ++)
    {
        cv::Mat tmp = cv::Mat(d->image.size().width, d->image.size().height, CV_32FC1 );
        
        cv::filter2D(image, tmp, -1, fltrs.at(k-1), cv::Point(-1,-1), 0.0, cv::BORDER_REPLICATE);
        
        channels.push_back(tmp);
        // std::cout<<tmp;
    }

    // Get raw moments
    
    cv::Mat mu1 = raw_moment(channels,1) / (d->image.size().width * d->image.size().height);
    //std::cout<<"mu1"<<mu1;
    cv::Mat mu2 = raw_moment(channels,2) / (d->image.size().width * d->image.size().height);
    cv::Mat mu3 = raw_moment(channels,3) / (d->image.size().width * d->image.size().height);
    cv::Mat mu4 = raw_moment(channels,4) / (d->image.size().width * d->image.size().height);
    

    // Calculate kurtoisis projesction
    
    cv::Mat noiV = mu2 - mat_pow(mu1,2);

    cv::Mat noiK = (mu4 - 4*mu1.mul(mu3) + 6*mat_pow(mu1,2).mul(mu2) - 3*mat_pow(mu1,4))/(mat_pow(noiV,2))-3;

    cv::Mat sqrt_noiK;
    
    cv::sqrt(noiK, sqrt_noiK);

    cv::Scalar mean, stddev;
    
    //std::cout<<"noiV"<<noiV;
    //std::cout<<"noiK"<<noiK;

    //Calculate noise variance
    cv::meanStdDev(sqrt_noiK,mean,stddev);
    float a = mean[0];
    qInfo()<<"variance a"<<a;

    cv::meanStdDev(mat_pow(noiV,-1),mean,stddev);
    float b = mean[0];
    qInfo()<<"variance b"<<b;

    cv::meanStdDev(mat_pow(noiV,-2),mean,stddev);
    float c = mean[0];
    qInfo()<<"variance c"<<c;

    cv::meanStdDev(sqrt_noiK.mul(mat_pow(noiV,-1)),mean,stddev);
    float d = mean[0];
    qInfo()<<"variance d"<<d;

    float sqrtK = (a*c - b*d)/(c-b*b);

    float V = (1 - a/sqrtK)/b;

    qInfo()<<"variance noise"<<V;

    return V;
}

cv::Mat NoiseDetector::raw_moment(NoiseDetector::Mat3D mat,int ordre)
{
    qInfo()<<"get raw moment";
    float list[mat.count()];
    // std::list<float> list;
    int index = 0;
    
    for (const auto mat2d : mat)
    {
        list[index] = cv::sum(mat_pow(mat2d,ordre))[0] / (d->image.size().width * d->image.size().height);

        index++;
    }

    cv::Mat res = cv::Mat(mat.count(), 1, CV_32FC1);

    std::memcpy(res.data, list, mat.count() * sizeof(float));

    //std::cout<<res; 
    
    return res;
}

cv::Mat NoiseDetector::mat_pow(const cv::Mat mat, float ordre)
{
    cv::Mat res =  cv::Mat(d->image.size().width, d->image.size().height, CV_32FC1 );
    
    // mat.convertTo(mat,CV_32F);

    cv::pow(mat, ordre, res);

    // std::cout<<res;

    return res;
}

}