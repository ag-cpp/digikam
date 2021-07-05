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

// Internal function to create af point from meta data
namespace NikonInternal
{

FocusPointsExtractor::FocusPoint create_af_point(float imageWidth, float imageHeight, 
                                                 float afPointWidth, float afPointHeight, 
                                                 float af_x_position, float af_y_position)
{    
    FocusPointsExtractor::FocusPoint point;

    point.x_position = af_x_position / imageWidth;
    point.y_position = af_y_position / imageHeight;
    point.width      = afPointWidth  / imageWidth;
    point.height     = afPointHeight / imageHeight;
    
    point.type = FocusPointsExtractor::TypePoint::SelectedInFocus;

    return point;
}

}

// Main function to extract af point
FocusPointsExtractor::ListAFPoints FocusPointsExtractor::getAFPoints_nikon()
{
    QString TagNameRoot = QLatin1String("MakerNotes.Nikon.Camera");

    // Filter model
    QString model = findValue(QLatin1String("EXIF.IFD0.Camera.Model")).toString().toLower();

    if (!model.contains(QLatin1String("nikon z"), Qt::CaseInsensitive))
    {
        return ListAFPoints();
    }

    // Get image size
    QVariant imageWidth, imageHeight;
    
    imageWidth = findValueFirstMatch(QStringList()
                                    <<QLatin1String("MakerNotes.Nikon.Camera.AFImageWidth")
                                    <<QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth")
                                    );
    imageHeight = findValueFirstMatch(QStringList()
                                    <<QLatin1String("MakerNotes.Nikon.Camera.AFImageHeight")
                                    <<QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight")
                                    );

    if (imageWidth.isNull() || imageHeight.isNull())
    {
        return ListAFPoints();
    }
    
    // Get size of point
    QVariant afPointWidth = findValue(TagNameRoot,QLatin1String("AFAreaWidth"));
    QVariant afPointHeight = findValue(TagNameRoot,QLatin1String("AFAreaHeight"));

    
    if ((afPointWidth.isNull()) || (afPointHeight.isNull()))
    {
        return ListAFPoints();
    }
        

    // Get coordinate of af points
    QVariant af_x_position = findValue(TagNameRoot,QLatin1String("AFAreaXPosition")) ;
    QVariant af_y_position = findValue(TagNameRoot,QLatin1String("AFAreaYPosition")) ;

    if (af_x_position.isNull() || af_y_position.isNull())
    {
        return ListAFPoints();
    }

    return ListAFPoints() << NikonInternal::create_af_point(imageWidth.toFloat(), imageHeight.toFloat(),
                                                            afPointWidth.toFloat(), afPointHeight.toFloat(),
                                                            af_x_position.toFloat(), af_y_position.toFloat());
}

}
