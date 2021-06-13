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

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{
class Q_DECL_HIDDEN BlurDetector::Private
{
public:
    explicit Private()
      : min_abs(1),
        ordre_log_filtrer(120),
        sigma_smooth_image(5),
        filtrer_defocus(150),

        part_size(40),
        edges_filtrer(10),
        theta_resolution(CV_PI/600),
        min_line_length(20),
        threshold_hough(20),
        min_nb_lines(1),
        max_stddev(0.15),

        have_focus_region(false)
    {

    }

    cv::Mat image;

    float       min_abs;
    int         ordre_log_filtrer;
    int         sigma_smooth_image;
    int         filtrer_defocus;
    
    float       part_size;
    float       edges_filtrer;
    double      theta_resolution;
    double      min_line_length; 
    float       threshold_hough;
    int         min_nb_lines;
    float       max_stddev;

    bool        have_focus_region;

};

BlurDetector::BlurDetector(const DImg& image)
    :  d(new Private)
{
    d->image = prepareForDetection(image);

    d->have_focus_region = haveFocusRegion(image);
}

BlurDetector::~BlurDetector()
{
    delete d;
}

// Maybe this function will move to read_image() of imagequalityparser 
// in case all detector of IQS use cv::Mat
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

float BlurDetector::detect()
{ 
    cv::Mat edgesMap = edgeDetection(d->image);

    cv::Mat defocusMap = detectDefocusMap(edgesMap);
    defocusMap.convertTo(defocusMap,CV_8U);

    cv::Mat motionBlurMap = detectMotionBlurMap(edgesMap);
    motionBlurMap.convertTo(motionBlurMap,CV_8U);
    
    // Read metadata to find focus region here ( dont implement yet )
    // cv::Mat weightsMat = getWeightsMat();

    // cv::Mat blurMap =  weightsMat.mul(defocusMap);

    cv::Mat blurMap = defocusMap + motionBlurMap;
            
    int totalPixels = blurMap.total();
    
    int blurPixel = cv::countNonZero(blurMap);

    float percentBlur = float(blurPixel) / float(totalPixels);

    qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur " << percentBlur;
    qInfo() <<  "percentage of blur " << percentBlur;
    return percentBlur;
}

cv::Mat BlurDetector::edgeDetection(const cv::Mat& image)         const
{
    // Convert the image to grayscale
    cv::Mat image_gray;
    
    cvtColor( image, image_gray, cv::COLOR_BGR2GRAY );

    // Use laplacian to detect edge map
    cv::Mat dst;
    
    cv::Laplacian( image_gray, dst, CV_32F);
    
    return dst;
}

cv::Mat BlurDetector::detectDefocusMap(const cv::Mat& edgesMap)    const
{
    cv::Mat abs_map = cv::abs(edgesMap);
    
    abs_map.setTo(d->min_abs, abs_map < d->min_abs);
    
    // Log filter
    cv::log(abs_map,abs_map);
    
    abs_map /= log(d->ordre_log_filtrer);
    
    // Smooth image to get blur map
    cv::blur(abs_map, abs_map, cv::Size(d->sigma_smooth_image,d->sigma_smooth_image));

    cv::medianBlur(abs_map, abs_map, d->sigma_smooth_image);
    
    // Mask blurred pixel as 1 and sharp pixel 0
    cv::Mat res = abs_map * 255;
    cv::threshold(res,res,d->filtrer_defocus,1,cv::THRESH_BINARY_INV);
        
    return res;
}

cv::Mat BlurDetector::detectMotionBlurMap(const cv::Mat& edgesMap) const
{
    // Divide image
    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";
    int nb_parts_row = static_cast<int>(edgesMap.size().height / d->part_size);
    int nb_parts_col = static_cast<int>(edgesMap.size().width / d->part_size);
    
    QHash<QPair<int,int>, bool> mapMotionBlur;

    cv::Mat res = cv::Mat::zeros(edgesMap.size(), CV_8U);

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Mat subImg = edgesMap(cv::Range(i*d->part_size, (i+1)*d->part_size ), 
                                      cv::Range(j*d->part_size, (j+1)*d->part_size ));
            
            qCDebug(DIGIKAM_DIMG_LOG) << "Detect if each part is motion blur";
            if(isMotionBlur(subImg)) {
                res(cv::Rect(j*d->part_size, i*d->part_size, d->part_size,d->part_size)).setTo(1);
            }

        }
    }  

    return res;
}

bool    BlurDetector::isMotionBlur(const cv::Mat& frag) const
{
    //convert to 8u
    cv::Mat tmp;

    cv::threshold(frag,tmp,d->edges_filtrer,255,cv::THRESH_BINARY);
    tmp.convertTo(tmp,CV_8U);
    
    std::vector<cv::Vec4i> lines;
    HoughLinesP(tmp, lines, 1, d->theta_resolution, d->threshold_hough, d->min_line_length,10 );

    // detect if region is motion blurred by number of paralle lines
    if (static_cast<int>(lines.size()) > d->min_nb_lines )
    {
        std::vector<float> list_theta; 
        for (const auto line : lines)
        {
            float theta = (line[2] == line[0]) ? 0 : qAtan((line[3]-line[1])/(line[2] - line[0]));

            theta = (theta < 0) ? theta + CV_PI : theta;

            theta = (theta < CV_PI/20) ?  CV_PI - theta : theta;
            
            list_theta.push_back(theta);
        }

        // calculate Standard Deviation         
        cv::Scalar mean, stddev;

        cv::meanStdDev(list_theta,mean,stddev);

        qCDebug(DIGIKAM_DIMG_LOG) << "Standard Deviation for group of lines " << stddev[0];

        return stddev[0] < d->max_stddev;
    }
    return false;
}

bool BlurDetector::haveFocusRegion(const DImg& inputImage)         const
{
    // FIXME : not implmented yet
    // initialate reader metadata to extract information of focus region
    return false;
}

cv::Mat BlurDetector::getWeightMap()                               const
{
    // FIXME : not implemented yet
    // use infomation of focus region to construct matrix of weight
    return cv::Mat::ones(1,1,1);
}

}