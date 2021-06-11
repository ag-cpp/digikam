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

#include "blur_detector.h"

namespace Digikam
{
class Q_DECL_HIDDEN BlurDetector::Private
{
public:
    explicit Private()
      : filtrer_defocus(12),
        sigma_smoothImage(12),
        min_abs(12),
        nb_parts_row(12),
        nb_parts_col(12),
        edges_filtrer(12),
        threshold_hough(12),
        min_nbLines(12),
        max_inertia(0.1)
    {

    }

    cv::Mat image;

    int filtrer_defocus;
    int sigma_smoothImage;
    float min_abs;
    int ordre_logFiltrer;

    int nb_parts_row;
    int nb_parts_col;

    float edges_filtrer;
    float threshold_hough;
    int min_nbLines;
    float max_inertia;


};

BlurDetector::BlurDetector(const DImg& image)
    :  d(new Private)
{
    d->image = prepareForDetection(image);
}

BlurDetector::~BlurDetector()
{

}

cv::Mat BlurDetector::prepareForDetection(const DImg& inputImage) const
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

    return cvImage;
}

cv::Mat BlurDetector::edgeDetection(const cv::Mat& image)         const
{
    // Convert the image to grayscale
    cv::Mat image_gray;
    
    cvtColor( image, image_gray, cv::COLOR_BGR2GRAY );

    // Use laplacian to detect edge
    cv::Mat dst;
    
    cv::Laplacian( image_gray, dst, CV_64F);
    
    return dst;
}

cv::Mat BlurDetector::detectDefocusMap(const cv::Mat& edgesMap)    const
{
    cv::Mat abs_map = cv::abs(edgesMap);
    
    abs_map.setTo(d->min_abs, abs_map < d->min_abs);
    // Log filter
    cv::log(abs_map,abs_map);
    
    abs_map *= 1/log(d->ordre_logFiltrer);
    abs_map.convertTo(abs_map, CV_32F);
    
    // smooth image to get blur map
    cv::blur(abs_map, abs_map, cv::Size(d->sigma_smoothImage,d->sigma_smoothImage));
    
    cv::Mat res;
    cv::medianBlur(abs_map, res, d->sigma_smoothImage);
    
    

    res *= 255;
    // Mask blurred pixel and sharp pixel 
    cv::threshold(res,res,d->filtrer_defocus,1,cv::THRESH_BINARY_INV);
        
    return res;

}

cv::Mat BlurDetector::detectMotionBlurMap(const cv::Mat& edgesMap) const
{
    // Divide image
    QHash<QPair<int,int>, bool> mapMotionBlur;
    cv::Size part_size = cv::Size(int(edgesMap.size().width / d->nb_parts_row), 
                                  int(edgesMap.size().height  / d->nb_parts_col));

    for (int i = 0; i < d->nb_parts_row; i++)
    {
        for (int j = 0; j < d->nb_parts_col; j++)
        {
            cv::Mat subImg = edgesMap(cv::Range(i*part_size.height,(i+1)*part_size.height ), 
                                      cv::Range(j*part_size.width, (j+1)*part_size.width ));
            mapMotionBlur.insert(QPair<int,int>(i,j),isMotionBlur(subImg));
        }
    }

    // Mask motion blurred pixel
    cv::Mat res = cv::Mat::zeros(edgesMap.size(), CV_8U);
    for (const auto& coordinate :mapMotionBlur.keys() )
    {
        if (mapMotionBlur.value(coordinate) )
        {
            cv::Mat tmp = cv::Mat::ones(part_size,CV_8U);
            tmp.copyTo(res(cv::Rect(coordinate.first*part_size.width, 
                                    coordinate.second*part_size.height,
                                    part_size.width,part_size.height)));
        }
    } 

    return res;
}

bool    BlurDetector::isMotionBlur(const cv::Mat& frag) const
{
    //convert to 8u
    cv::Mat tmp = frag;

    cv::threshold(tmp,tmp,d->edges_filtrer,255,cv::THRESH_BINARY);
    tmp.convertTo(tmp,CV_8U);
    
    
    std::vector<cv::Vec2f> lines; 
    
    HoughLines(tmp, lines, 1, CV_PI/180, d->threshold_hough); 

    // detect if region have paralle lines
    if (QVector<cv::Vec2f>::fromStdVector(lines).count() > d->min_nbLines )
    {
        QList<float> list_theta; 
        float sum = 0;
        for (const auto line : lines)
        {
            float theta = (line[1] <= 0) ?  line[1] + CV_PI : line[1];
            sum += theta;
            list_theta.push_back(theta);
        }
        float avg = sum / list_theta.count();

        float inertia = 0;
        for (const float theta : list_theta)
        {
            inertia += abs(avg - theta);
        }
        inertia /= list_theta.count();
        qInfo() <<"inertia " << inertia;
        if (inertia < d->max_inertia) {qInfo()<<"part motion blur";} 
        return inertia < d->max_inertia;
    }
    return false;
}

}