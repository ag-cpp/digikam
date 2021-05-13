/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dngwriter_p.h"

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include "digikam_debug.h"

// Local includes

#include "dngwriter.h"

namespace Digikam
{

DNGWriter::Private::Private()
    : identify    (nullptr),
      identifyMake(nullptr),
      activeWidth (0),
      activeHeight(0),
      outputHeight(0),
      outputWidth (0)
{
    reset();
}

DNGWriter::Private::~Private()
{
}

void DNGWriter::Private::clearMemory()
{
    delete identify;
    delete identifyMake;
}

void DNGWriter::Private::reset()
{
    cancel                  = false;
    jpegLossLessCompression = true;
    updateFileDate          = false;
    backupOriginalRawFile   = false;
    previewMode             = DNGWriter::MEDIUM;
}

void DNGWriter::Private::cleanup()
{
    if (::remove(QFile::encodeName(outputFile).constData()) != 0)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot remove " << outputFile;
    }
}

dng_date_time DNGWriter::Private::dngDateTime(const QDateTime& qDT) const
{
    dng_date_time dngDT;
    dngDT.fYear   = qDT.date().year();
    dngDT.fMonth  = qDT.date().month();
    dngDT.fDay    = qDT.date().day();
    dngDT.fHour   = qDT.time().hour();
    dngDT.fMinute = qDT.time().minute();
    dngDT.fSecond = qDT.time().second();

    return dngDT;
}

bool DNGWriter::Private::fujiRotate()
{
    QByteArray tmpData(rawData);
    int height             = identify->outputSize.height();
    int width              = identify->outputSize.width();
    unsigned short* tmp    = reinterpret_cast<unsigned short*>(tmpData.data());
    unsigned short* output = reinterpret_cast<unsigned short*>(rawData.data());

    for (int row = 0 ; row < height ; ++row)
    {
        for (int col = 0 ; col < width ; ++col)
        {
            output[col * height + row] = tmp[row * width + col];
        }
    }

    identify->orientation = DRawInfo::ORIENTATION_Mirror90CCW;
    identify->outputSize  = QSize(height, width);

    // TODO: rotate margins

    return true;
}

QString DNGWriter::Private::dngErrorCodeToString(int errorCode) const
{
    switch (errorCode)
    {
        default:
        case 100000: return QLatin1String("Unknown error");

        case 100003: return QLatin1String("Processing stopped by user (or host application) request");

        case 100004: return QLatin1String("Necessary host functionality is not present");

        case 100005: return QLatin1String("Out of memory");

        case 100006: return QLatin1String("File format is not valid");

        case 100007: return QLatin1String("Matrix has wrong shape, is badly conditioned, or similar problem");

        case 100008: return QLatin1String("Could not open file");

        case 100009: return QLatin1String("Error reading file");

        case 100010: return QLatin1String("Error writing file");

        case 100011: return QLatin1String("Unexpected end of file");

        case 100012: return QLatin1String("File is damaged in some way");

        case 100013: return QLatin1String("Image is too big to save as DNG");

        case 100014: return QLatin1String("Image is too big to save as TIFF");

        case 100015: return QLatin1String("DNG version is unsupported");
    }
}

} // namespace Digikam
