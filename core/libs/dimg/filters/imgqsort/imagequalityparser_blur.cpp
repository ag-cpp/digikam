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

    qCDebug(DIGIKAM_DIMG_LOG) << "get here";

    // cv::Mat blurMap =  weightsMat.mul(defocusMap);
    cv::Mat blurMap = defocusMap;

    int totalPixels = blurMap.rows * blurMap.cols;

    int zeroPixels = cv::countNonZero(blurMap);

    double percentBlur = zeroPixels / totalPixels;

    qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur " << percentBlur;

    return percentBlur;


    // d->lowThreshold   = 0.4;
    // d->ratio          = 3;
    // double maxval     = 0.0;
    // cannyThreshold(0, nullptr);

    // double average    = mean(d->detected_edges)[0];
    // int* const maxIdx = new int[sizeof(d->detected_edges)];
    // minMaxIdx(d->detected_edges, nullptr, &maxval, nullptr, maxIdx);

    // double blurresult = average / maxval;

    // qCDebug(DIGIKAM_DIMG_LOG) << "The average of the edge intensity is " << average;
    // qCDebug(DIGIKAM_DIMG_LOG) << "The maximum of the edge intensity is " << maxval;
    // qCDebug(DIGIKAM_DIMG_LOG) << "The result of the edge intensity is  " << blurresult;

    // delete [] maxIdx;

    // return blurresult;
}

short ImageQualityParser::blurDetector2() const
{
    return 0;
    // // Algorithm using Laplacian of Gaussian Filter to detect blur.

    // Mat out;
    // Mat noise_free;
    // qCDebug(DIGIKAM_DIMG_LOG) << "Algorithm using LoG Filter started";

    // // To remove noise from the image.

    // GaussianBlur(d->src_gray, noise_free, Size(3, 3), 0, 0, BORDER_DEFAULT);

    // // Aperture size of 1 corresponds to the correct matrix.

    // int kernel_size = 3;
    // int scale       = 1;
    // int delta       = 0;
    // int ddepth      = CV_16S;

    // Laplacian(noise_free, out, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);

    // // noise_free:  The input image without noise.
    // // out:         Destination (output) image.
    // // ddepth:      Depth of the destination image. Since our input is CV_8U we define ddepth = CV_16S to avoid overflow.
    // // kernel_size: The kernel size of the Sobel operator to be applied internally. We use 3 here.

    // short maxLap = -32767;

    // for (int i = 0 ; i < out.rows ; ++i)
    // {
    //     for (int j = 0 ; j < out.cols ; ++j)
    //     {
    //         short value = out.at<short>(i, j);

    //         if (value > maxLap)
    //         {
    //             maxLap = value;
    //         }
    //     }
    // }

    // return maxLap;
}

cv::Mat ImageQualityParser::prepareForDetection(const DImg& inputImage) const
{
    if (inputImage.isNull() || !inputImage.size().isValid())
    {
        return cv::Mat();
    }

    cv::Mat cvImage;
    int type               = inputImage.sixteenBit() ? CV_16UC4 : CV_8UC4;
    cv::Mat cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), CV_8U, inputImage.bits());

    if (inputImage.hasAlpha())
    {
        cvtColor(cvImageWrapper, cvImage, cv::COLOR_BGR2GRAY);
    }
    else
    {
        cvtColor(cvImageWrapper, cvImage, cv::COLOR_BGR2GRAY);
    }

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
    
    cv::Laplacian( image, dst, ddepth);

    return dst;
}
cv::Mat ImageQualityParser::defocusDetection(const cv::Mat& edgesMap)    const
{
    cv::Mat abs_map = cv::abs(edgesMap);

    cv::log(abs_map,abs_map);

    abs_map *= 1/log(10);

    cv::blur(abs_map, abs_map, cv::Size(5,5));

    qInfo()<<"format abs_map"<< abs_map.type();
    abs_map.convertTo(abs_map, CV_32F);
    cv::Mat res;
    cv::medianBlur(abs_map, res, 5);

    return res;

}
cv::Mat ImageQualityParser::motionBlurDetection(const cv::Mat& edgesMap) const
{
    cv::Mat res = cv::Mat(d->image.height(), d->image.width(), 1);

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
    cv::Mat weightMat = cv::Mat(d->image.height(), d->image.width(), 1);

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
