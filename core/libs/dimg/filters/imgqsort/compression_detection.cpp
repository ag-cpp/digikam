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

#include "compression_detection.h"

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
    :  d(new Private)
{
    d->image = prepareForDetection(image);
}

CompressionDetector::~CompressionDetector()
{
    delete d;
}

// Maybe this function will move to read_image() of imagequalityparser 
// in case all detector of IQS use cv::Mat
cv::Mat CompressionDetector::prepareForDetection(const DImg& inputImage) const
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

auto accessRow = [](cv::Mat mat) {
    return [mat](int index) {
        return mat.row(index);
    };
};

auto accessCol = [](cv::Mat mat) {
    return [mat](int index) {
        return mat.col(index);
    };
};

float CompressionDetector::detect()
{
    cv::Mat gray_image;

    cv::cvtColor(d->image, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat verticalBlock = checkEdgesBlock(gray_image, gray_image.cols, accessCol);

    cv::Mat horizontalBlock = checkEdgesBlock(gray_image, gray_image.rows, accessRow);

    cv::Mat mono_color_map = detectMonoColorRegion();

    cv::Mat block_map = mono_color_map.mul(verticalBlock + horizontalBlock);

    int nb_pixels_edge_block = cv::countNonZero(block_map);

    int nb_pixels_mono_color = cv::countNonZero(mono_color_map);

    int nb_pixels_normal = d->image.total() - nb_pixels_edge_block - nb_pixels_edge_block;

    float res = static_cast<float>((nb_pixels_mono_color * d->weight_mono_color + nb_pixels_edge_block * d->threshold_edges_block) /
                                   (nb_pixels_mono_color * d->weight_mono_color + nb_pixels_edge_block * d->threshold_edges_block + nb_pixels_normal));

    return res;
}

template <typename Function>
cv::Mat CompressionDetector::checkEdgesBlock(const cv::Mat& gray_image, int blockSize, Function accessEdges) const
{
    cv::Mat res = cv::Mat::zeros(gray_image.size(),CV_8UC1 );

    auto accessGrayImageAt = accessEdges(gray_image);
    auto accessResAt = accessEdges(res);

    for (int i = 2; i < blockSize - 1; i ++)
    {
        cv::Mat a = (accessGrayImageAt(i) - accessGrayImageAt(i + 1)) - (accessGrayImageAt(i - 1) - accessGrayImageAt(i));
        
        cv::Mat b = (accessGrayImageAt(i) - accessGrayImageAt(i + 1)) - (accessGrayImageAt(i + 1) - accessGrayImageAt(i - 2));

        accessResAt(i) = (a >= d->threshold_edges_block) & (b >= d->threshold_edges_block);
    }

    return res;
}

cv::Mat CompressionDetector::detectMonoColorRegion() const
{
    cv::Mat median_image;

    cv::medianBlur(d->image, median_image, 5);

    cv::Mat abs_difference = cv::abs(d->image - median_image);

    std::vector<cv::Mat> rgbChannels(3);
    
    cv::split(abs_difference, rgbChannels);

    cv::Mat res = rgbChannels[0] + rgbChannels[1] + rgbChannels[2];

    cv::threshold(res,res,d->threshold_mono_color,1,cv::THRESH_BINARY_INV);

    return res;
}

float CompressionDetector::normalize(const float number)
{
    return 1.0 / (1.0 + qExp(-(number - d->alpha)/d->beta ));
}

}
