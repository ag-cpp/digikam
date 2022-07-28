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

#include "aesthetic_detector.h"

// Qt includes

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN AestheticDetector::Private
{

public:

    explicit Private()
    {
        model = cv::dnn::readNetFromTensorflow("/home/lephuock/Downloads/iqs_digikam/weights_inceptionresnetv2_08_0.910.hdf5.pb");
    }
    cv::dnn::Net model;
};

AestheticDetector::AestheticDetector()
    : DetectorDistortion(),
      d                 (new Private)
{
}

AestheticDetector::~AestheticDetector()
{
    delete d;
}

float AestheticDetector::detect(const cv::Mat& image) const
{
    cv::Mat input = preprocess(image);
    
    d->model.setInput(input);
    cv::Mat out = d->model.forward();
    
    return postProcess(out);
    
}

cv::Mat AestheticDetector::preprocess(const cv::Mat& image) const
{
    cv::Mat img_rgb;
    cv::cvtColor(image, img_rgb, cv::COLOR_BGR2RGB);
    cv::Mat cv_resized;
    cv::resize(img_rgb, cv_resized, cv::Size(224, 224), 0, 0, cv::INTER_NEAREST_EXACT);
    cv_resized.convertTo(cv_resized, CV_32FC3);
    cv_resized = cv_resized.mul(1.0 / float(127.5));
    subtract(cv_resized, cv::Scalar(1, 1, 1), cv_resized);

    cv::Mat blob = cv::dnn::blobFromImage(cv_resized, 1, cv::Size(224, 224), cv::Scalar(0, 0, 0), false, false);

    return blob;
}

float AestheticDetector::postProcess(const cv::Mat& modelOutput) const
{
    std::cout << "score : " << modelOutput << "\n";
    cv::Point maxLoc;
    cv::minMaxLoc(modelOutput, nullptr, nullptr, nullptr, &maxLoc);
    return float(maxLoc.x);
}

}
