/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Parser - blur detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 *
 * References  : https://stackoverflow.com/questions/7765810/is-there-a-way-to-detect-if-an-image-is-blurry
 *               https://stackoverflow.com/questions/35768416/is-here-any-way-to-find-out-whether-an-image-is-blurry-or-not-using-laplacian-op
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

#include "imagequalityparser_p.h"

namespace Digikam
{

double ImageQualityParser::blurDetector() const
{
    cv::Mat cvImage = prepareForDetection(d->image);
    
    cv::Mat edgesMap = edgeDetection(cvImage);

    cv::Mat defocusMap = defocusDetection(edgesMap);
    defocusMap.convertTo(defocusMap,CV_8U);

    cv::Mat motionBlurMap = motionBlurDetection(edgesMap);
    motionBlurMap.convertTo(motionBlurMap,CV_8U);
    // cv::Mat weightsMat = getWeightsMat();

    // cv::Mat blurMap =  weightsMat.mul(defocusMap);

    cv::Mat blurMap ;
    cv::add(defocusMap,motionBlurMap,blurMap);

    int totalPixels = blurMap.rows * blurMap.cols;
    
    blurMap.convertTo(blurMap, CV_16UC1);

    int blurPixel = cv::countNonZero(blurMap);

    float percentBlur = float(blurPixel) / float(totalPixels);

    // qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur " << percentBlur;
    qInfo() <<  "percentage of blur " << percentBlur;
    return percentBlur;
}

cv::Mat ImageQualityParser::prepareForDetection(const DImg& inputImage) const
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

cv::Mat ImageQualityParser::edgeDetection(const cv::Mat& image)         const
{
    // Convert the image to grayscale
    cv::Mat image_gray;
    
    cvtColor( image, image_gray, COLOR_BGR2GRAY );

    // Use laplacian to detect edge
    Mat dst;
    int ddepth = CV_64F;
    
    cv::Laplacian( image_gray, dst, ddepth);
    return dst;
}
cv::Mat ImageQualityParser::defocusDetection(const cv::Mat& edgesMap, const int threshold,const  int sigmaBlur ,const int min_abs ,const int ordreLog )    const
{
    cv::Mat abs_map = cv::abs(edgesMap);

    abs_map.setTo(min_abs, abs_map < min_abs);

    // Log filter
    cv::log(abs_map,abs_map);

    abs_map *= 1/log(ordreLog);

    // smooth image to get blur map
    cv::blur(abs_map, abs_map, cv::Size(sigmaBlur,sigmaBlur));
    
    abs_map.convertTo(abs_map, CV_32FC1);
    cv::Mat res;
    cv::medianBlur(abs_map, res, sigmaBlur);

    res *= 255;

    // Mask blurred pixel and sharp pixel 
    cv::threshold(res,res,threshold,1,THRESH_BINARY_INV);
        
    return res;

}
cv::Mat ImageQualityParser::motionBlurDetection(const cv::Mat& edgesMap) const
{
    // cv::Mat res = cv::Mat::ones (edgesMap.height(), edgesMap.width(), 1);
    int nb_parts_row = 6;
    int nb_parts_col = 6;

    // Divide image
    QHash<QPair<int,int>, bool> mapMotionBlur;
    cv::Size part_size = cv::Size(int(edgesMap.size().width / nb_parts_row), 
                                  int(edgesMap.size().height  / nb_parts_col));

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Mat subImg = edgesMap(cv::Range(i*part_size.height,(i+1)*part_size.height ), 
                                      cv::Range(j*part_size.width, (j+1)*part_size.width ));
            mapMotionBlur.insert(QPair<int,int>(i,j),isMotionBlur(subImg,10,30,100));
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

bool    ImageQualityParser::isMotionBlur(const cv::Mat& frag, const int threshold_filter, const int threshold_hough , const int threshold_lines) const
{
    //convert to 8u
    cv::Mat tmp = frag;

    cv::threshold(tmp,tmp,threshold_filter,255,THRESH_BINARY);
    tmp.convertTo(tmp,CV_8U);
    
    
    std::vector<cv::Vec2f> lines; // will hold the results of the detection
    
    HoughLines(tmp, lines, 1, CV_PI/180, threshold_hough); // runs the actual detection

    // detect if region have paralle lines
    if (QVector<cv::Vec2f>::fromStdVector(lines).count() > threshold_lines )
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
        // qInfo() <<"inertia " << inertia;

        return inertia < 0.8;
    }
    return false;
}

cv::Mat ImageQualityParser::getBlurMap()                                const
{
    cv::Mat res = cv::Mat(d->image.height(), d->image.width(), 1);

    return res;
}

bool    ImageQualityParser::haveFocusMeta()                             const
{
    return false;
}
cv::Mat ImageQualityParser::getWeightsMat()                          const
{
    cv::Mat weightMat = cv::Mat::ones(d->image.height(), d->image.width(), CV_8U);

    if (!haveFocusMeta())
    {
        return weightMat;
    } 
    else
    {
        // use funtion read meta data here re-calculate weightMat
        return weightMat;
    } 
}


} // namespace Digikam
