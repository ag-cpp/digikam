/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - file I/O helpers.
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011      by Leif Huhn <leif at dkstat dot com>
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

#include "dmetadata.h"

// Qt includes

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QUuid>
#include <QMimeDatabase>

// Local includes

#include "filereadwritelock.h"
#include "metaenginesettings.h"
#include "digikam_version.h"
#include "digikam_globals.h"
#include "digikam_debug.h"

namespace Digikam
{

bool DMetadata::load(const QString& filePath, Backend* backend)
{
    FileReadLocker lock(filePath);

    Backend usedBackend = NoBackend;
    bool hasLoaded      = false;
    QMimeDatabase mimeDB;

    if (
        !mimeDB.mimeTypeForFile(filePath).name().startsWith(QLatin1String("video/")) &&
        !mimeDB.mimeTypeForFile(filePath).name().startsWith(QLatin1String("audio/")) &&
        (QFileInfo(filePath).suffix().toUpper() != QLatin1String("INSV"))
       )
    {
        // Process images only with Exiv2 backend first, or Exiftool in 2nd, or libraw for RAW files,
        // or with libheif, or at end with ImageMagick.
        // Never process video files with Exiv2, the backend is very unstable and this feature will be removed.

        if (!(hasLoaded = MetaEngine::load(filePath)))
        {
            if (!(hasLoaded = loadUsingExifTool(filePath)))
            {
                if (!(hasLoaded = loadUsingRawEngine(filePath)))
                {

                    if (!(hasLoaded =
#ifdef HAVE_HEIF
                        loadUsingLibheif(filePath)
#else
                        false
#endif
                        ))
                    {
                        if (!(hasLoaded = loadUsingImageMagick(filePath)))
                        {
                            usedBackend = NoBackend;
                        }
                        else
                        {
                            usedBackend = ImageMagickBackend;
                        }
                    }
                    else
                    {
                        usedBackend = LibHeifBackend;
                    }
                }
                else
                {
                    usedBackend = LibRawBackend;
                }
            }
            else
            {
                usedBackend = ExifToolBackend;
            }
        }
        else
        {
            usedBackend = Exiv2Backend;
        }
    }
    else
    {
        // No image files (aka video or audio), process with ExifTool or ffmpeg backends.

        if (!(hasLoaded = loadUsingExifTool(filePath)))
        {
            if (!(hasLoaded = loadUsingFFmpeg(filePath)))
            {
                usedBackend = NoBackend;
            }
            else
            {
                usedBackend = FFMpegBackend;
            }
        }
        else
        {
            usedBackend = ExifToolBackend;
        }

        hasLoaded |= loadFromSidecarAndMerge(filePath);
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Loading metadata with"
                                    << backendName(usedBackend)
                                    << "backend from" << filePath;

    if (backend)
    {
        *backend = usedBackend;
    }

    return hasLoaded;
}

bool DMetadata::save(const QString& filePath, bool setVersion) const
{
    FileWriteLocker lock(filePath);

    Backend usedBackend = NoBackend;
    bool hasSaved       = false;

    if (!(hasSaved = MetaEngine::save(filePath, setVersion)))
    {
        if (!(hasSaved = saveUsingExifTool(filePath)))
        {
            usedBackend = NoBackend;
        }
        else
        {
            usedBackend = ExifToolBackend;
        }
    }
    else
    {
        usedBackend = Exiv2Backend;
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Saving metadata with"
                                    << backendName(usedBackend)
                                    << "backend to" << filePath;

    return hasSaved;
}

bool DMetadata::applyChanges(bool setVersion) const
{
    FileWriteLocker lock(getFilePath());

    Backend usedBackend = NoBackend;
    bool hasSaved       = false;

    if (!(hasSaved = MetaEngine::applyChanges(setVersion)))
    {
        if (!(hasSaved = saveUsingExifTool(QString())))
        {
            usedBackend = NoBackend;
        }
        else
        {
            usedBackend = ExifToolBackend;
        }
    }
    else
    {
        usedBackend = Exiv2Backend;
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "Apply metadata changes with"
                                    << backendName(usedBackend)
                                    << "backend to" << getFilePath();

    return hasSaved;
}

} // namespace Digikam
