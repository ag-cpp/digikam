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
      : threshold(2),
        part_size_mono_color(20),
        mono_color_threshold(10.0),

        alpha(0.027),
        beta(-0.0015)
    {
    }

    cv::Mat image;

    int threshold;
    int part_size_mono_color;
    float mono_color_threshold;

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

    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);

    return cvImage;
}

float CompressionDetector::detect()
{
    cv::Mat verticalBlock = checkVertical();

    cv::Mat horizontalBlock = checkHorizontal();

    cv::Mat mono_color_map = detectMonoColorRegion();

    cv::Mat block_map = mono_color_map.mul(verticalBlock + horizontalBlock);

    qInfo()<<"percent pixel block" <<static_cast<float> (cv::countNonZero(block_map)) / static_cast<float> (block_map.total());

    float precent_edges_block = static_cast<float> (cv::countNonZero(block_map)) / static_cast<float> (block_map.total());

    return normalize(precent_edges_block);
}

cv::Mat CompressionDetector::checkVertical()
{
    cv::Mat res = cv::Mat::zeros(d->image.size(),CV_8UC1 );

    for (int i = 2; i < d->image.cols - 1; i ++)
    {
        cv::Mat a = (d->image.col(i) - d->image.col(i + 1)) - (d->image.col(i - 1) - d->image.col(i));
        
        cv::Mat b = (d->image.col(i) - d->image.col(i + 1)) - (d->image.col(i + 1) - d->image.col(i - 2));

        cv::threshold(a, a, d->threshold, 1, cv::THRESH_BINARY);
        cv::threshold(b, b, d->threshold, 1, cv::THRESH_BINARY);

        auto col = res.col(i);
        col = a & b;
    }

    return res;
}

cv::Mat CompressionDetector::checkHorizontal()
{
    cv::Mat res = cv::Mat::zeros(d->image.size(),CV_8UC1 );

    for (int i = 2; i < d->image.rows - 1; i ++)
    {
        cv::Mat a = (d->image.row(i) - d->image.row(i + 1)) - (d->image.row(i - 1) - d->image.row(i));
        
        cv::Mat b = (d->image.row(i) - d->image.row(i + 1)) - (d->image.row(i + 1) - d->image.row(i - 2));

        cv::threshold(a, a, d->threshold, 1, cv::THRESH_BINARY);
        cv::threshold(b, b, d->threshold, 1, cv::THRESH_BINARY);

        cv::Mat check = a & b;

        auto row = res.row(i);
        row = check;
    }

    return res;
}

cv::Mat CompressionDetector::detectMonoColorRegion()
{
    qCDebug(DIGIKAM_DIMG_LOG) << "Divide image to small parts";

    int nb_parts_row = static_cast<int>(d->image.size().height / d->part_size_mono_color);
    int nb_parts_col = static_cast<int>(d->image.size().width  / d->part_size_mono_color);
    
    cv::Mat res = cv::Mat::zeros(d->image.size(), CV_8U);

    for (int i = 0; i < nb_parts_row; i++)
    {
        for (int j = 0; j < nb_parts_col; j++)
        {
            cv::Rect rect{j*d->part_size_mono_color, i*d->part_size_mono_color, 
                          d->part_size_mono_color, d->part_size_mono_color};

            cv::Mat subImg = d->image(rect);
            
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

float CompressionDetector::normalize(const float number)
{
    return 1.0 / (1.0 + qExp(-(number - d->alpha)/d->beta ));
}

}
