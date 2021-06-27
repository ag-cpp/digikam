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


void FocusPointsExtractor::getAFPoints_canon()
{
    QString TagNameRoot = QLatin1String("MakerNotes.Canon.Camera");

    // Get size image 
    QString model = findValue(QLatin1String("EXIF.IFD0.Camera.Make")).toString();
    
    QVariant imageWidth, imageHeight;
    
    if (model.toLower() == QLatin1String("canon eos 5d"))
    {
        imageWidth = findValueFirstMatch(QStringList()
                                        <<QLatin1String("MakerNotes.Canon.Image.CanonImageWidth")
                                        <<QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth")
                                        );
        imageHeight = findValueFirstMatch(QStringList()
                                        <<QLatin1String("MakerNotes.Canon.Image.CanonImageHeight")
                                        <<QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight")
                                        );
    }
    else
    {
        imageWidth = findValueFirstMatch(QStringList()
                                        <<QLatin1String("MakerNotes.Canon.Camera.AFImageWidth")
                                        <<QLatin1String("EXIF.ExifIFD.Image.ExifImageWidth")
                                        );
        imageHeight = findValueFirstMatch(QStringList()
                                        <<QLatin1String("MakerNotes.Canon.Camera.AFImageHeight")
                                        <<QLatin1String("EXIF.ExifIFD.Image.ExifImageHeight")
                                        );
    }
    
    if (imageWidth.isNull() || imageHeight.isNull())
    {
        qInfo()<<"return by lack of size image";
        return;
    }
    qInfo()<<"image size in metadata"<<imageWidth<<imageHeight;
    

    // Get size of af points
    QVariant afPointWidth = findValue(TagNameRoot,QLatin1String("AFAreaWidth"));
    QVariant afPointHeight = findValue(TagNameRoot,QLatin1String("AFAreaHeight"));
    QStringList afPointWidths = findValue(TagNameRoot,QLatin1String("AFAreaWidths"), true).toStringList();
    QStringList afPointHeights = findValue(TagNameRoot,QLatin1String("AFAreaHeights"), true).toStringList();
    
    if (((afPointWidth.isNull()) || (afPointHeight.isNull())) && 
        ((afPointWidths.isEmpty()) || (afPointHeights.isEmpty())))
    {
        qInfo()<<!afPointWidths.isEmpty()<< !afPointHeights.isEmpty();
        qInfo()<<"return by lack of size point";
        return;
    }
        

    // Get coordinate of af points
    QStringList af_x_positions = findValue(TagNameRoot,QLatin1String("AFAreaXPositions"), true).toStringList() ;
    QStringList af_y_positions = findValue(TagNameRoot,QLatin1String("AFAreaYPositions"), true).toStringList() ;

    if (af_x_positions.isEmpty() || af_y_positions.isEmpty())
    {
        qInfo()<<"return by lack of point";
        return;
    }

    // Get type of af points
    QStringList af_selected = findValueFirstMatch(TagNameRoot, QStringList()
                                               <<QLatin1String("AFPointsSelected")
                                               <<QLatin1String("AFPointsInFocus"),
                                                true).toStringList();
    QStringList af_infocus  = findValue(TagNameRoot,QLatin1String("AFPointsInFocus"),  true).toStringList();

    QVariant cameraType = findValue(TagNameRoot,QLatin1String("CameraType"));

    int yDirection = (cameraType.toString().toUpper() == QLatin1String("COMPACT")) ? -1 : 1;

    int nb_points = af_x_positions.count();

    for (int i=0; i< nb_points; i++)
    {
        qInfo()<<"study one point";
        FocusPoint point;
        point.x_position = 0.5 + af_x_positions[i].toFloat() /  imageWidth.toFloat();
        point.y_position = 0.5 + af_y_positions[i].toFloat() * yDirection /  imageHeight.toFloat();
        
        point.width = (afPointWidths[i].isEmpty()) ?  afPointWidth.toFloat() : afPointWidths[i].toFloat();
        point.height = (afPointHeights[i].isEmpty()) ?  afPointHeight.toFloat() : afPointHeights[i].toFloat();

        point.width /= imageWidth.toFloat();
        point.height /= imageHeight.toFloat();

        point.type = TypePoint::Inactive;
        
        bool isSelected = af_selected.contains(QString::number(i)); // durty trick 
        
        bool isInfocus  = af_infocus.contains(QString::number(i));
        
        if (isSelected && isInfocus)
        {
            point.type = TypePoint::SelectedInFocus;
        }
        else if (isSelected)
        {
            point.type = TypePoint::Selected;
        }
        else if (isInfocus)
        {
            point.type = TypePoint::Infocus;
        }

        addPoint(point);
        qInfo()<<"point info"<<point.x_position<<point.y_position<<point.width<<point.height;
    }

}


}