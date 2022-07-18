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
    
}


}
