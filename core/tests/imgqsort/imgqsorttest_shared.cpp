/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level - shared code
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imgqsorttest_shared.h"

// Qt includes

#include <QStringList>
#include <QDebug>

// Local includes

#include "dimg.h"
#include "previewloadthread.h"
#include "imagequalityparser.h"

using namespace Digikam;

ImageQualityContainer ImgQSortTest_ArrangeSettings (DetectionType type)
{
    ImageQualityContainer settings;

    settings.detectBlur         = false;
    settings.detectNoise        = false;
    settings.detectCompression  = false;
    settings.detectExposure     = false;

    switch (type)
    {
        case DetectNoise:
            settings.detectNoise        = true;
            break;

        case DetectCompression:
            settings.detectCompression  = true;
            break;

        case DetectExposure:
            settings.detectExposure     = true;
            break;
        
        case DetectBlur:
            settings.detectBlur         = true;
            break;

        default:
            settings.detectBlur         = true;
            settings.detectCompression  = true;
            settings.detectNoise        = true;
            settings.detectExposure     = true;
            break;
    }

    return settings;
}

QMultiMap<QString, int> ImgQSortTest_ParseTestImages(DetectionType type, const QFileInfoList& list)
{
    ImageQualityContainer settings = ImgQSortTest_ArrangeSettings(type);

    qDebug() << "Quality Detection Settings:" << settings;
    qInfo()  << "Detection type (0:Blur, 1:Noise, 2:Compression, 3:Exposure, 4: General)";
    qDebug() << "Process images for detection type "<<type <<" ( size " << list.size() << ")";

    QMultiMap<QString, int> results;

    foreach (const QFileInfo& inf, list)
    {
        QString path = inf.filePath();
        qDebug() << path;

        DImg dimg    = PreviewLoadThread::loadFastSynchronously(path, 1024);

        if (dimg.isNull())
        {
            qDebug() << path << "File cannot be loaded...";
        }

        PickLabel pick;
        ImageQualityParser parser(dimg, settings, &pick);
        parser.startAnalyse();

        results.insert( path.split(QLatin1String("/")).last(), pick);
    }

    qInfo() << "Quality Results (0:None, 1:Rejected, 2:Pending, 3:Accepted):";

    for (QMap<QString, int >::const_iterator it = results.constBegin() ; it != results.constEnd() ; ++it)
    {
        qInfo() << "==>" << it.key() << ":" << it.value();
    }

    return results;
}
