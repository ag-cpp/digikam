/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-15-09
 * Description : a command line tool to convert RAW file to PNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QPointer>
#include <QScopedPointer>

// Local includes

#include "drawdecoder.h"
#include "drawdecodersettings.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qDebug() << "raw2png - RAW Camera Image to PNG Converter";
        qDebug() << "Usage: <rawfile>";
        return -1;
    }

    QString            filePath = QString::fromLatin1(argv[1]);
    QFileInfo          input(filePath);
    QString            previewFilePath(input.baseName() + QString::QString::fromLatin1(".preview.png"));
    QFileInfo          previewOutput(previewFilePath);
    QString            halfFilePath(input.baseName() + QString::fromLatin1(".half.png"));
    QFileInfo          halfOutput(halfFilePath);
    QString            fullFilePath(input.baseName() + QString::fromLatin1(".full.png"));
    QFileInfo          fullOutput(fullFilePath);
    QImage             image;
    QScopedPointer<DRawInfo> identify(new DRawInfo);

    // -----------------------------------------------------------

    qDebug() << "raw2png: Identify RAW image from " << input.fileName();

    QPointer<DRawDecoder> rawProcessor(new DRawDecoder);

    if (!rawProcessor->rawFileIdentify(*identify, filePath))
    {
        qDebug() << "raw2png: Identifying RAW image failed. Aborted...";
        return -1;
    }

    int width  = identify->imageSize.width();
    int height = identify->imageSize.height();

    qDebug() << "raw2png: Raw image info:";
    qDebug() << "--- Date:      " << identify->dateTime.toString(Qt::ISODate);
    qDebug() << "--- Make:      " << identify->make;
    qDebug() << "--- Model:     " << identify->model;
    qDebug() << "--- Size:      " << width << "x" << height;
    qDebug() << "--- Filter:    " << identify->filterPattern;
    qDebug() << "--- Colors:    " << identify->rawColors;

    // -----------------------------------------------------------

    qDebug() << "raw2png: Loading RAW image preview";

    if (!rawProcessor->loadRawPreview(image, filePath))
    {
        qDebug() << "raw2png: Loading RAW image preview failed. Aborted...";
        return -1;
    }

    qDebug() << "raw2png: Saving preview image to "
             << previewOutput.fileName() << " size ("
             << image.width() << "x" << image.height()
             << ")";
    image.save(previewFilePath, "PNG");

    // -----------------------------------------------------------

    qDebug() << "raw2png: Loading half RAW image";

    image = QImage();

    if (!rawProcessor->loadHalfPreview(image, filePath))
    {
        qDebug() << "raw2png: Loading half RAW image failed. Aborted...";
        return -1;
    }

    qDebug() << "raw2png: Saving half image to "
             << halfOutput.fileName() << " size ("
             << image.width() << "x" << image.height()
             << ")";

    image.save(halfFilePath, "PNG");

    // -----------------------------------------------------------

    qDebug() << "raw2png: Loading full RAW image";

    image = QImage();
    DRawDecoderSettings settings;
    settings.halfSizeColorImage    = false;
    settings.sixteenBitsImage      = false;
    settings.RGBInterpolate4Colors = false;
    settings.RAWQuality            = DRawDecoderSettings::BILINEAR;

    if (!rawProcessor->loadFullImage(image, filePath, settings))
    {
        qDebug() << "raw2png: Loading full RAW image failed. Aborted...";
        return -1;
    }

    qDebug() << "raw2png: Saving full RAW image to "
             << fullOutput.fileName() << " size ("
             << image.width() << "x" << image.height()
             << ")";

    image.save(fullFilePath, "PNG");

    return 0;
}
