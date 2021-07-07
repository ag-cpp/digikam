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
#include "focuspoints_extractor.h"

namespace Digikam
{
class Q_DECL_HIDDEN BlurDetector::Private
{
public:
    explicit Private()
      : min_abs(1),
        ordre_log_filtrer(100),
        sigma_smooth_image(5),
        filtrer_defocus(95),

        part_size_motion_blur(20),
        edges_filtrer(10),
        theta_resolution(CV_PI/600),
        min_line_length(12),
        threshold_hough(10),
        min_nb_lines(2),
        max_stddev(0.6),

        part_size_mono_color(40),
        mono_color_threshold(10),

        have_focus_region(false)
    {

    }

    cv::Mat image;

    float                               min_abs;
    float                               ordre_log_filtrer;
    float                               sigma_smooth_image;
    float                               filtrer_defocus;
    
    int                                 part_size_motion_blur;
    float                               edges_filtrer;
    double                              theta_resolution;
    double                              min_line_length; 
    float                               threshold_hough;
    int                                 min_nb_lines;
    float                               max_stddev;

    int                                 part_size_mono_color;
    float                               mono_color_threshold;
    
    bool                                have_focus_region;
    FocusPointsExtractor::ListAFPoints  af_points;
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
    
    cv::Mat weightsMat = getWeightMap();

    cv::Mat blurMap = defocusMap + motionBlurMap;

    cv::Mat res = weightsMat.mul(blurMap);
            
    int totalPixels = cv::countNonZero(weightsMat);

    qInfo()<<"number pixels considered" << totalPixels << "tota pixels"<< weightsMat.total();
    
    int blurPixel = cv::countNonZero(res);

    float percentBlur = float(blurPixel) / float(totalPixels);

    qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur" << percentBlur;
    
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
    
    int nb_parts_row = static_cast<int>(edgesMap.size().height / d->part_size_motion_blur);
    int nb_parts_col = static_cast<int>(edgesMap.size().width / d->part_size_motion_blur);
    
    cv::Mat res = cv::Mat::zeros(edgesMap.size(), CV_8U);

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Rect rect{j*d->part_size_motion_blur, i*d->part_size_motion_blur, 
                          d->part_size_motion_blur,d->part_size_motion_blur};

            cv::Mat subImg = edgesMap(rect);
            
            qCDebug(DIGIKAM_DIMG_LOG) << "Detect if each part is motion blur";
            if(isMotionBlur(subImg)) 
            {
                res(rect).setTo(1);
            }
        }
    }

    return res;
}

bool    BlurDetector::isMotionBlur(const cv::Mat& frag) const
{
    // Convert to 8u
    cv::Mat tmp;

    cv::threshold(frag,tmp,d->edges_filtrer,255,cv::THRESH_BINARY);
    tmp.convertTo(tmp,CV_8U);
    
    std::vector<cv::Vec4i> lines;
    HoughLinesP(tmp, lines, 1, d->theta_resolution, d->threshold_hough, d->min_line_length,10 );

    // Detect if region is motion blurred by number of paralle lines
    
    // qInfo()<<"nb lines " << static_cast<int>(lines.size());

    if (static_cast<int>(lines.size()) > d->min_nb_lines )
    {
        std::vector<float> list_theta; 
        
        for (const auto line : lines)
        {
            float theta = (line[2] == line[0]) ? 0 : qAtan((line[3]-line[1])/(line[2] - line[0]));

            theta = (theta < 0) ? theta + CV_PI : theta;

            // theta = (theta < CV_PI/20) ?  CV_PI - theta : theta;
            
            list_theta.push_back(theta);
        }

        // Calculate Standard Deviation         
        cv::Scalar mean, stddev;

        cv::meanStdDev(list_theta,mean,stddev);

        // qInfo()<< "Standard Deviation for group of lines " << stddev[0];
        qCDebug(DIGIKAM_DIMG_LOG) << "Standard Deviation for group of lines " << stddev[0];

        return stddev[0] < d->max_stddev;
    }
    return false;
}

bool BlurDetector::haveFocusRegion(const DImg& image)              const
{
    QScopedPointer<FocusPointsExtractor> const extractor (new FocusPointsExtractor(nullptr, image.originalFilePath()));

    d->af_points = extractor->get_af_points(FocusPointsExtractor::TypePoint::Selected);
    
    return !d->af_points.isEmpty();
}

cv::Mat BlurDetector::getWeightMap()                               const
{
    cv::Mat res = cv::Mat::zeros(d->image.size(), CV_8UC1);

    if (d->have_focus_region)
    {
        for (const auto point : d->af_points)
        {
            qInfo()<<"point postion "<< static_cast<int>((point.x_position  - 0.5 * point.width) * d->image.size().width)
                   << static_cast<int>((point.y_position  - 0.5 * point.height) * d->image.size().height)
                   <<"size point" << static_cast<int>(point.width * d->image.size().width )
                   <<  static_cast<int>(point.height * d->image.size().height) ; 
            cv::Rect rect{static_cast<int>((point.x_position  - 0.5 * point.width) * d->image.size().width),
                          static_cast<int>((point.y_position  - 0.5 * point.height)* d->image.size().height),
                          static_cast<int>(point.width * d->image.size().width),
                          static_cast<int>(point.height * d->image.size().height)};

            res(rect).setTo(1);
        }
    }
    else
    {
        res = detectBackgroundRegion(d->image);
        
        cv::threshold(res, res, 0.5, 1, cv::THRESH_BINARY_INV);
    }
    
    return res;
}

cv::Mat BlurDetector::detectBackgroundRegion(const cv::Mat& image)    const
{
    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";

    int nb_parts_row = static_cast<int>(image.size().height / d->part_size_mono_color);
    int nb_parts_col = static_cast<int>(image.size().width / d->part_size_mono_color);
    
    cv::Mat res = cv::Mat::zeros(image.size(), CV_8U);

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Rect rect{j*d->part_size_mono_color, i*d->part_size_mono_color, 
                          d->part_size_mono_color, d->part_size_mono_color};

            cv::Mat subImg = image(rect);
            
            qCDebug(DIGIKAM_DIMG_LOG) << "Detect if each part is mono-color";
            
            cv::Scalar mean, stddev;

            cv::meanStdDev(subImg,mean,stddev);

            if (stddev[0] < d->mono_color_threshold) 
            {
                res(rect).setTo(1);
            }
        }
    }
    return res;
}

}