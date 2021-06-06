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

void ImageQualityParser::cannyThreshold(int, void*) const
{
    return;
    // // Reduce noise with a kernel 3x3.

    // blur(d->src_gray, d->detected_edges, Size(3, 3));

    // // Canny detector.

    // Canny(d->detected_edges,
    //       d->detected_edges,
    //       d->lowThreshold,
    //       d->lowThreshold * d->ratio,
    //       d->kernel_size);
}

double ImageQualityParser::blurDetector() const
{
    cv::Mat cvImage = prepareForDetection(d->image);
    
    cv::Mat edgesMap = edgeDetection(cvImage);

    cv::Mat defocusMap = defocusDetection(edgesMap);
    defocusMap.convertTo(defocusMap, CV_16F);

    // Use motion blur detection here
    // cv::Mat motionBlurMap = motionBlurDetection(edgesMap);
    
    // cv::Mat weightsMat = getWeightsMat();

    // cv::Mat blurMap =  weightsMat.mul(defocusMap);

    cv::Mat blurMap = defocusMap;

    int totalPixels = blurMap.rows * blurMap.cols;

    qInfo()<<"format blurMap"<< blurMap.type();
    
    blurMap.convertTo(blurMap, CV_16UC1);

    int zeroPixels = cv::countNonZero(blurMap);
    qInfo() <<  "zero non Pixels of blur " << zeroPixels << "total pixels :" << totalPixels;

    float percentBlur = float(totalPixels - zeroPixels) / float(totalPixels);

    // qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur " << percentBlur;
    qInfo() <<  "percentage of blur " << percentBlur;
    return percentBlur;
}

short ImageQualityParser::blurDetector2() const
{
    return 0;
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
    qInfo()<<" image_gray type "<< image_gray.type();

    // Use laplacian to detect edge
    Mat dst;
    int ddepth = CV_64F;
    
    cv::Laplacian( image_gray, dst, ddepth);
    qInfo()<<" dst laplacian type "<< dst.type();
    return dst;
}
cv::Mat ImageQualityParser::defocusDetection(const cv::Mat& edgesMap)    const
{
    cv::Mat abs_map = cv::abs(edgesMap);

    abs_map.setTo(5, abs_map < 5);

    // Log filter
    cv::log(abs_map,abs_map);

    abs_map *= 1/log(10);

    // smooth image to get blur map
    cv::blur(abs_map, abs_map, cv::Size(5,5));
    qInfo()<<" abs_map blur type "<< abs_map.type();
    
    abs_map.convertTo(abs_map, CV_32FC1);
    cv::Mat res;
    cv::medianBlur(abs_map, res, 5);
    qInfo()<<" abs_map blur median type "<< abs_map.type();

    res *= 255;

    // Mask blurred pixel and sharp pixel 
    cv::threshold(res,res,200,255,THRESH_BINARY);
    qInfo()<<" abs_map theshold type "<< abs_map.type();
        
    return res;

}
cv::Mat ImageQualityParser::motionBlurDetection(const cv::Mat& edgesMap) const
{
    // cv::Mat res = cv::Mat::ones (edgesMap.height(), edgesMap.width(), 1);
    cv::Mat res = edgesMap;
    return res;
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
