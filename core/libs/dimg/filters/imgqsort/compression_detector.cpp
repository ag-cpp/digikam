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
 * References  : http://www.arpnjournals.org/jeas/research_papers/rp_2016/jeas_1216_5505.pdf
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

#include "compression_detector.h"

// Qt includes

#include <QtMath>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN CompressionDetector::Private
{
public:
    explicit Private()
      : threshold_edges_block(2),
        weight_edges_block(120.0),

        weight_mono_color(10.0),
        threshold_mono_color(0.1),

        alpha(0.026),
        beta(-0.002)
    {
    }

    cv::Mat image;

    int threshold_edges_block;
    float weight_edges_block;

    int part_size_mono_color;
    float mono_color_threshold;

    float weight_mono_color;
    float threshold_mono_color;

    float alpha;
    float beta;
};

CompressionDetector::CompressionDetector(const DImg& image)
    :  DetectorDistortion(DetectorDistortion(image)),
       d(new Private)
{
    d->image = getCvImage();
}

CompressionDetector::CompressionDetector(const DetectorDistortion& detector)
    :  DetectorDistortion(detector),
       d(new Private)
{
    d->image = getCvImage();
}


CompressionDetector::~CompressionDetector()
{
    delete d;
}

float CompressionDetector::detect() const
{
    cv::Mat gray_image;

    cv::cvtColor(d->image, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat verticalBlock = checkVertical(gray_image);

    cv::Mat horizontalBlock = checkHorizontal(gray_image);

    cv::Mat mono_color_map = detectMonoColorRegion();

    cv::Mat block_map = mono_color_map.mul(verticalBlock + horizontalBlock);

    int nb_pixels_edge_block = cv::countNonZero(block_map);

    int nb_pixels_mono_color = cv::countNonZero(mono_color_map);

    int nb_pixels_normal = d->image.total() - nb_pixels_edge_block - nb_pixels_edge_block;

    qInfo()<<"nb_pixels_edge_block"<<nb_pixels_edge_block<<"nb_pixels_mono_color"<<nb_pixels_mono_color<<"total pixel"<<d->image.total();

    float res = static_cast<float>((nb_pixels_mono_color * d->weight_mono_color + nb_pixels_edge_block * d->threshold_edges_block) /
                                   (nb_pixels_mono_color * d->weight_mono_color + nb_pixels_edge_block * d->threshold_edges_block + nb_pixels_normal));

    qInfo()<<"res"<<res;

    return res;
}

cv::Mat CompressionDetector::checkVertical(const cv::Mat& gray_image) const
{    
    cv::Mat res = cv::Mat::zeros(gray_image.size(),CV_8UC1 );

    for (int i = 2; i < gray_image.cols - 1; i ++)
    {
        cv::Mat a = (gray_image.col(i) - gray_image.col(i + 1)) - (gray_image.col(i - 1) - gray_image.col(i));
        
        cv::Mat b = (gray_image.col(i) - gray_image.col(i + 1)) - (gray_image.col(i + 1) - gray_image.col(i - 2));

        cv::threshold(a, a, d->threshold_edges_block, 1, cv::THRESH_BINARY);
        cv::threshold(b, b, d->threshold_edges_block, 1, cv::THRESH_BINARY);

        auto col = res.col(i);
        col = a & b;
    }

    return res;
}

cv::Mat CompressionDetector::checkHorizontal(const cv::Mat& gray_image) const
{
    cv::Mat res = cv::Mat::zeros(gray_image.size(),CV_8UC1 );

    for (int i = 2; i < gray_image.rows - 1; i ++)
    {
        cv::Mat a = (gray_image.row(i) - gray_image.row(i + 1)) - (gray_image.row(i - 1) - gray_image.row(i));
        
        cv::Mat b = (gray_image.row(i) - gray_image.row(i + 1)) - (gray_image.row(i + 1) - gray_image.row(i - 2));

        cv::threshold(a, a, d->threshold_edges_block, 1, cv::THRESH_BINARY);
        cv::threshold(b, b, d->threshold_edges_block, 1, cv::THRESH_BINARY);

        cv::Mat check = a & b;

        auto row = res.row(i);
        row = check;
    }

    return res;
}

cv::Mat CompressionDetector::detectMonoColorRegion() const
{
    cv::Mat median_image;

    cv::medianBlur(d->image, median_image, 5);

    cv::Mat mat_subtraction = cv::abs(d->image - median_image);

    std::vector<cv::Mat> rgbChannels(3);
    
    cv::split(mat_subtraction, rgbChannels);

    cv::Mat res = rgbChannels.at(0) + rgbChannels.at(1) + rgbChannels.at(2);

    cv::threshold(res,res,d->threshold_mono_color,1,cv::THRESH_BINARY_INV);

    return res;
}

float CompressionDetector::normalize(const float number)
{
    return 1.0 / (1.0 + qExp(-(number - d->alpha)/d->beta ));
}

}
