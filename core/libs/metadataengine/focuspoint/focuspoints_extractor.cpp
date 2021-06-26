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

#include "exiftoolparser.h"

namespace Digikam
{
class Q_DECL_HIDDEN FocusPointsExtractor::Private
{
public:
    explicit Private() {};

    ListAFPoints                    af_points;

    ExifToolParser::ExifToolData    metadata;
};

FocusPointsExtractor::FocusPointsExtractor(QObject* const parent,const QString& image_path)
    :  QObject(parent),
       d(new Private)
{
    ExifToolParser* const exiftool = new ExifToolParser(this);

    exiftool->load(image_path);

    d->metadata = exiftool->currentData();

    d->af_points = ListAFPoints();

    findAFPoints();

    delete exiftool;
}

FocusPointsExtractor::~FocusPointsExtractor()
{
    delete d;
}

QVariant FocusPointsExtractor::findValue(const QString& tagName,bool isList)
{
    QVariantList result = d->metadata.value(tagName);
    

    if (result.empty())
    {
        return QVariant();
    }
    if (isList)
    {
        return result[0].toString().split(QLatin1String(" "));
    }
    else
    {
        return result[0];
    }
    
}

QVariant FocusPointsExtractor::findValue(const QString& tagNameRoot, const QString& key,bool isList)
{
    return findValue(tagNameRoot + QLatin1String(".") + key,isList);
}

QVariant FocusPointsExtractor::findValueFirstMatch(const QStringList& listTagNames,bool isList)
{
    for (const QString& tagName : listTagNames)
    {
        QVariant tmp = findValue(tagName,isList);
        
        if (!tmp.isNull())
        {
            return tmp;
        }
    }
    return QVariant();
}

QVariant FocusPointsExtractor::findValueFirstMatch(const QString& tagNameRoot,const QStringList& keys, bool isList)
{
    for (const QString& key : keys)
    {
        QVariant tmp = findValue(tagNameRoot,key,isList);
        
        if (!tmp.isNull())
        {
            return tmp;
        }
    }
    return QVariant();
}

void FocusPointsExtractor::findAFPoints()
{
    QString model = findValue(QLatin1String("EXIF.IFD0.Camera.Make"), true).toStringList()[0].toUpper();

    if (model.isNull())
    {
        return;
    }
    if (model == QLatin1String("CANON"))
    {   
        return getAFPoints_Canon();
    }
    if (model == QLatin1String("NIKON"))
    {
        return getAFPoints_Nikon();
    }
    return;
}

void FocusPointsExtractor::getAFPoints_defauts()
{

}


void FocusPointsExtractor::getAFPoints_Nikon()
{

}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::af_infocus()
{
    ListAFPoints points = ListAFPoints();
    for (const auto point : d->af_points)
    {
        if (point.type == TypePoint::Infocus || point.type == TypePoint::SelectedInFocus)
        {
            points.push_back(point);
        }
    }
    return points;
}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::af_selected()
{
    ListAFPoints points = ListAFPoints();
    for (const auto point : d->af_points)
    {
        if (point.type == TypePoint::Selected || point.type == TypePoint::SelectedInFocus)
        {
            points.push_back(point);
        }
    }
    return points;
}

FocusPointsExtractor::ListAFPoints FocusPointsExtractor::af_infocus_selected()
{
    ListAFPoints points = ListAFPoints();
    for (const auto point : d->af_points)
    {
        if (point.type == TypePoint::SelectedInFocus)
        {
            points.push_back(point);
        }
    }
    return points;
}

void FocusPointsExtractor::addPoint(const FocusPoint& point)
{
    d->af_points.push_back(point);
}

}