/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Image Quality Parser - Aesthetic detection
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "asthetic_detector.h"

// Qt includes

// Local includes

namespace Digikam
{

class Q_DECL_HIDDEN AestheticDetector::Private
{

public:

    explicit Private()
    {
        float tmp[10] = {0,1,2,3,4,5,6,7,8,9};
        scale = cv::Mat(1, 10, out.type(), tmp);

        model = cv::dnn::readNetFromTensorflow();
    }

    string model_path;
    cv::mat scale;
    cv::dnn::Net model;

    
};

AestheticDetector::AestheticDetector(const DImg& image)
    : DetectorDistortion(),
      d                 (new Private)
{
    QScopedPointer<FocusPointsExtractor> const extractor (new FocusPointsExtractor(nullptr, image.originalFilePath()));

    d->af_points         = extractor->get_af_points(FocusPoint::TypePoint::Selected);
    d->have_focus_region = !d->af_points.isEmpty();
}

AestheticDetector::~AestheticDetector()
{
    delete d;
}

float AestheticDetector::detect(const cv::Mat& image) const
{
    cv::Mat edgesMap      = edgeDetection(image);

    cv::Mat defocusMap    = detectDefocusMap(edgesMap);
    defocusMap.convertTo(defocusMap, CV_8U);

    cv::Mat motionBlurMap = detectMotionBlurMap(edgesMap);
    motionBlurMap.convertTo(motionBlurMap, CV_8U);

    cv::Mat weightsMat    = getWeightMap(image);

    cv::Mat blurMap       = defocusMap + motionBlurMap;

    cv::Mat res           = weightsMat.mul(blurMap);

    int totalPixels       = cv::countNonZero(weightsMat);

    int blurPixel         = cv::countNonZero(res);

    float percentBlur     = float(blurPixel) / float(totalPixels);

    qCDebug(DIGIKAM_DIMG_LOG) << "percentage of blur" << percentBlur;

    return percentBlur;
}


}
