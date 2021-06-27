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


#ifndef DIGIKAM_FOCUSPOINTS_EXTRACTOR_H
#define DIGIKAM_FOCUSPOINTS_EXTRACTOR_H

// Qt includes 

#include <QObject>
#include <QVariant>
#include <QStringList>


// Local includes

#include "digikam_export.h"


namespace Digikam 
{

class DIGIKAM_EXPORT FocusPointsExtractor : public QObject
{
    Q_OBJECT

public:

    enum TypePoint
    {
        Inactive            = 0,
        Infocus             = 1,
        Selected            = 2,
        SelectedInFocus     = 3
    };

    struct FocusPoint {

        float       x_position;
        float       y_position;
        float       width;
        float       height;
        TypePoint   type;
    };

    /**
     * A list used to store focus points of a image extracted from meta data 
     * 
     * With extract() function, an exiftool parser is used to read data from
     * metadata and lists all focus points. Each focus point is defined by their
     * relative centers coordinate and relative size. Each point has own 
     * type (Inactive, Infocus, Selected, SelectedInFocus)
     */
    typedef QList<FocusPoint> ListAFPoints;

public:

    explicit FocusPointsExtractor(QObject* const parent,const QString& path);
    ~FocusPointsExtractor();

public:

    ListAFPoints af_infocus();
    ListAFPoints af_selected();
    ListAFPoints af_infocus_selected();
    

private:
    
    QVariant findValue(const QString& tagName,bool isList = false);
    QVariant findValue(const QString& tagNameRoot, const QString& key,bool isList = false);
    QVariant findValueFirstMatch(const QStringList& listTagNames,bool isList = false);
    QVariant findValueFirstMatch(const QString& tagNameRoot,const QStringList& keys, bool isList= false);
    
    void addPoint(const FocusPoint& point);
    
    void findAFPoints();
    
    void getAFPoints_canon();
    void getAFPoints_nikon();
    void getAFPoints_panasonic();
    void getAFPoints_sony();

private:

    class Private;
    Private* const d;

};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINTS_EXTRACTOR_H
