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
        ordre_logFiltrer(120),
        sigma_smoothImage(5),
        filtrer_defocus(150),

        part_size(40),
        edges_filtrer(10),
        theta_resolution(CV_PI/600),
        min_lineLength(20),
        threshold_hough(20),
        min_nbLines(1),
        max_stddev(0.15),

        haveFocusRegion(false)
    {

    }

    cv::Mat image;

    float       min_abs;
    int         ordre_logFiltrer;
    int         sigma_smoothImage;
    int         filtrer_defocus;
    
    float       part_size;
    float       edges_filtrer;
    double      theta_resolution;
    double      min_lineLength; 
    float       threshold_hough;
    int         min_nbLines;
    float       max_stddev;

    bool        haveFocusRegion;

};

BlurDetector::BlurDetector(const DImg& image)
    :  d(new Private)
{
    d->image = prepareForDetection(image);

    d->haveFocusRegion = haveFocusRegion(image);
}

BlurDetector::~BlurDetector()
{

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

    cv::Mat blurMap ;
    
    cv::add(defocusMap,motionBlurMap,blurMap);
    
    blurMap.convertTo(blurMap, CV_8UC1);
    
    int totalPixels = blurMap.rows * blurMap.cols;
    
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
    
    // Smooth image to get blur map
    abs_map.convertTo(abs_map, CV_32F);

    cv::blur(abs_map, abs_map, cv::Size(d->sigma_smoothImage,d->sigma_smoothImage));

    cv::medianBlur(abs_map, abs_map, d->sigma_smoothImage);
    
    // Mask blurred pixel as 1 and sharp pixel 0
    cv::Mat res = abs_map * 255;
    cv::threshold(res,res,d->filtrer_defocus,1,cv::THRESH_BINARY_INV);
        
    return res;
}

cv::Mat BlurDetector::detectMotionBlurMap(const cv::Mat& edgesMap) const
{
    // Divide image
    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";
    int nb_parts_row = int(edgesMap.size().height / d->part_size);
    int nb_parts_col = int(edgesMap.size().width / d->part_size);
    
    QHash<QPair<int,int>, bool> mapMotionBlur;

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Mat subImg = edgesMap(cv::Range(i*d->part_size, (i+1)*d->part_size ), 
                                      cv::Range(j*d->part_size, (j+1)*d->part_size ));
            
            qCDebug(DIGIKAM_DIMG_LOG) << "Detect if each part is motion blur";
            mapMotionBlur.insert(QPair<int,int>(i,j),isMotionBlur(subImg));
        }
    }

    // Mask motion blurred pixel
    qCDebug(DIGIKAM_DIMG_LOG) << "mask motion blurred pixel";
    
    cv::Mat res = cv::Mat::zeros(edgesMap.size(), CV_8U);
    
    for (const auto& coordinate :mapMotionBlur.keys() )
    {
        if (mapMotionBlur.value(coordinate) )
        {
            cv::Mat tmp = cv::Mat::ones(cv::Size(d->part_size, d->part_size), CV_8U);
            
            tmp.copyTo(res(cv::Rect(coordinate.second * d->part_size, 
                                    coordinate.first  *d->part_size,
                                    d->part_size,d->part_size)));
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
    
    std::vector<cv::Vec4i> lines;
    HoughLinesP(tmp, lines, 1, d->theta_resolution, d->threshold_hough, d->min_lineLength,10 );

    // detect if region is motion blurred by number of paralle lines
    if (QVector<cv::Vec4i>::fromStdVector(lines).count() > d->min_nbLines )
    {
        QList<float> list_theta; 
        float sum = 0;
        for (const auto line : lines)
        {
            float theta = (line[2] == line[0]) ? 0 : qAtan((line[3]-line[1])/(line[2] - line[0]));

            theta = (theta < 0) ? theta + CV_PI : theta;

            theta = (theta < CV_PI/20) ?  CV_PI - theta : theta;
            
            list_theta.push_back(theta);
        
            sum += theta;
        }

        // calculate Standard Deviation 
        float mean_theta = sum / float(list_theta.count());
        
        float stddev = 0;
        
        for (const auto theta : list_theta)
        {
            stddev += pow(mean_theta-theta,2);
        }
        stddev /= float(list_theta.count());

        qCDebug(DIGIKAM_DIMG_LOG) << "Standard Deviation for group of lines " << stddev;

        return stddev < d->max_stddev;
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