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

#include "imagequality_thread.h"

// Qt includes


// Local includes

namespace Digikam
{

ImageQualityThread::ImageQualityThread(QObject* const parent ,DetectorDistortion* detector,
                                       const cv::Mat& image, ImageQualityCalculator* calculator, 
                                       float weight_quality)
  : QThread(parent)
{
    m_image = image;
    m_detector = detector;
    m_calculator = calculator;
    m_weight = weight_quality;
}

void ImageQualityThread::run()
{
    float damageLevel = m_detector->detect(m_image);
    m_calculator->addDetectionResult(QString(), damageLevel, m_weight);
}

}