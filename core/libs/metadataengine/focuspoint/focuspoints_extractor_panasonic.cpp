/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Extraction of focus points by exiftool data
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "focuspoints_extractor.h"

// Qt includes

#include <QDebug>

// Local includes

namespace Digikam
{

const float RATIO_POINT_IMAGE = 1/3; // this is a guess

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_panasonic()
{
    QString TagNameRoot = QLatin1String("MakerNotes.Panasonic.Camera");

    // Get size image
    QVariant imageWidth = findValue(QLatin1String("File.File.Image.ImageWidth"));
    QVariant imageHeight = findValue(QLatin1String("File.File.Image.ImageHeight"));

    if (imageWidth.isNull() || imageHeight.isNull())
    {
        return ListAFPoints();
    }

    // Get af point
    QStringList af_position =  findValue(TagNameRoot,QLatin1String("AFPointPosition")).toString().split(QLatin1String(" "));

    if (af_position.isEmpty())
    {
        return ListAFPoints();
    }
    float position_x = af_position[0].toFloat();
    float position_y = af_position[1].toFloat();
    
    // Add point
    FocusPoint point;

    point.x_position = position_x;
    point.y_position = position_y;
    point.width      = imageWidth.toFloat()  * RATIO_POINT_IMAGE;
    point.height     = imageHeight.toFloat() * RATIO_POINT_IMAGE;

    point.type = TypePoint::SelectedInFocus;

    return ListAFPoints() << point;

}

}
