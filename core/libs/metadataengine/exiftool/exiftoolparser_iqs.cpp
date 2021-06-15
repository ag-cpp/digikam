/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool find infomation of focus region from exiftool metadata
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

#include "exiftoolparser_p.h"

namespace Digikam
{


ExifToolParser::ListAFPoints ExifToolParser::getAFInfo()
{
    QString model = currentData().value(QLatin1String("EXIF.IFD0.Camera.Make"))[0].toString();
    model = model.split(QLatin1String(" "))[0].toUpper();

    if (model == QLatin1String("CANON"))
    {   
        return getAFInfo_Canon();
    }
    if (model == QLatin1String("NIKON"))
    {
        return getAFInfo_Nikon();
    }    
    return ListAFPoints();
}

ExifToolParser::ListAFPoints ExifToolParser::getAFInfo_Canon()
{
    ExifToolParser::ExifToolData data = currentData();

    int image_width = data.value(QLatin1String("File.File.Image.ImageWidth"))[0].toFloat();
    int image_height = data.value(QLatin1String("File.File.Image.ImageHeight"))[0].toFloat();

    QString TagNameRoot = QLatin1String("MakerNotes.Canon.Camera.");

    QStringList AF_x_positions = data.value(TagNameRoot + QLatin1String("AFAreaXPositions"))[0].toString().split(QLatin1String(" "));
    QStringList AF_y_positions = data.value(TagNameRoot + QLatin1String("AFAreaYPositions"))[0].toString().split(QLatin1String(" "));
 
    ListAFPoints res = ListAFPoints();

    for (auto i =0; i < AF_x_positions.count(); i++)
    {
        res << QPair<int,int> (image_width  / 2 + AF_x_positions[i].toInt(),
                               image_height / 2 + AF_y_positions[i].toInt());
    }
    qInfo()<<"get here";
    return res;
}

ExifToolParser::ListAFPoints ExifToolParser::getAFInfo_Nikon()
{
    ExifToolParser::ExifToolData data = currentData();

    QString TagNameRoot = QLatin1String("MakerNotes.Nikon.Camera.");

    float image_width = data.value(QLatin1String("File.File.Image.ImageWidth"))[0].toFloat();
    float image_height = data.value(QLatin1String("File.File.Image.ImageHeight"))[0].toFloat();

    float AF_x_position = data.value(TagNameRoot + QLatin1String("AFAreaXPosition"))[0].toFloat();
    float AF_y_position = data.value(TagNameRoot + QLatin1String("AFAreaYPosition"))[0].toFloat();
    float AF_image_width = data.value(TagNameRoot + QLatin1String("AFImageWidth"))[0].toFloat();
    float AF_image_height = data.value(TagNameRoot + QLatin1String("AFImageHeight"))[0].toFloat();

    ListAFPoints res = ListAFPoints() << QPair<int,int>(static_cast<int>(AF_x_position / AF_image_width * image_width),
                                                        static_cast<int>(AF_y_position / AF_image_height * image_height));

    return res;
    
}

}