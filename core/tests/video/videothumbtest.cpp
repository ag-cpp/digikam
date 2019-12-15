/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-04-21
 * Description : Video thumbnails extraction CLI test tool
 *
 * Copyright (C) 2016-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDebug>

// Local includes

#include "videothumbnailer.h"
#include "videostripfilter.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        qDebug() << "videothumbtest - Load video files to extract thumbnails";
        qDebug() << "Usage: <videofiles>";
        return -1;
    }

    QApplication app(argc, argv);

    for (int i = 1 ; i < argc ; i++)
    {
        QString path = QString::fromLocal8Bit(argv[i]);
        VideoThumbnailer thumbnailer;
        VideoStripFilter videoStrip;
        QImage image;

        thumbnailer.addFilter(&videoStrip);
        thumbnailer.setThumbnailSize(256);
        thumbnailer.generateThumbnail(path, image);

        if (!image.isNull())
        {
            qDebug() << "Extracted thumbnail from" << path << image.size();
            image.save(QString::fromUtf8("./%1.png").arg(path), "PNG");
        }
        else
        {
           qDebug() << "Cannot extract thumbnail from" << path;
        }
    }

    return 0;
}
