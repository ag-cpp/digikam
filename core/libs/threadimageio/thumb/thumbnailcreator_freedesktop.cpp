/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-07-20
 * Description : Loader for thumbnails - Freedesktop.org standard implementation
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "thumbnailcreator_p.h"

namespace Digikam
{

ThumbnailInfo ThumbnailCreator::fileThumbnailInfo(const QString& path)
{
    ThumbnailInfo info;
    info.filePath     = path;
    QFileInfo fileInfo(path);
    info.isAccessible = fileInfo.exists();
    info.fileName     = fileInfo.fileName();

    QMimeDatabase mimeDB;
    QString mimeType(mimeDB.mimeTypeForFile(path).name());

    if (mimeType.startsWith(QLatin1String("image/")))
    {
        info.mimeType = QLatin1String("image");
    }
    else if (mimeType.startsWith(QLatin1String("video/")))
    {
        info.mimeType = QLatin1String("video");
    }

    if (!info.isAccessible)
    {
        return info;
    }

    info.modificationDate = fileInfo.lastModified();
    return info;
}

ThumbnailImage ThumbnailCreator::loadFreedesktop(const ThumbnailInfo& info) const
{
    QString path;

    if (!info.customIdentifier.isNull())
    {
        path = info.customIdentifier;
    }
    else
    {
        path = info.filePath;
    }

    QString uri       = thumbnailUri(path);
    QString thumbPath = thumbnailPath(path);
    QImage qimage     = loadPNG(thumbPath);

    // NOTE: if thumbnail have not been generated by digiKam (konqueror for example),
    // force to recompute it, else we use it.
    if (!qimage.isNull())
    {
        if (qimage.text(QLatin1String("Thumb::MTime")) == QString::number(info.modificationDate.toTime_t()) &&
            qimage.text(QLatin1String("Software"))     == d->digiKamFingerPrint)
        {
            ThumbnailImage info;
            info.qimage = qimage;
            // is stored rotated. Not needed to rotate.
            info.exifOrientation = DMetadata::ORIENTATION_NORMAL;
            return info;
        }
    }

    return ThumbnailImage();
}

void ThumbnailCreator::storeFreedesktop(const ThumbnailInfo& info, const ThumbnailImage& image) const
{
    QImage qimage = image.qimage;

    QString path;

    if (!info.customIdentifier.isNull())
    {
        path = info.customIdentifier;
    }
    else
    {
        path = info.filePath;
    }

    QString uri       = thumbnailUri(path);
    QString thumbPath = thumbnailPath(path);

    // required by spec
    if (qimage.format() != QImage::Format_ARGB32)
    {
        qimage = qimage.convertToFormat(QImage::Format_ARGB32);
    }

    qimage.setText(QLatin1String("Thumb::URI"),   uri);
    qimage.setText(QLatin1String("Thumb::MTime"), QString::number(info.modificationDate.toTime_t()));
    qimage.setText(QLatin1String("Software"),     d->digiKamFingerPrint);

    QTemporaryFile temp;
    temp.setFileTemplate(thumbPath + QLatin1String("-digikam-") + QLatin1String("XXXXXX") + QLatin1String(".png"));
    temp.setAutoRemove(false);

    if (temp.open())
    {
        QString tempFileName = temp.fileName();

        if (qimage.save(tempFileName, "PNG", 0))
        {
            Q_ASSERT(!tempFileName.isEmpty());

            temp.close();

            // remove thumbPath file if it exist
            if (tempFileName != thumbPath && QFile::exists(tempFileName) && QFile::exists(thumbPath))
            {
                QFile::remove(thumbPath);
            }

            if (!QFile::rename(tempFileName, thumbPath))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot rename thumb file (" << tempFileName << ")";
                qCDebug(DIGIKAM_GENERAL_LOG) << "to (" << thumbPath << ")...";
            }
        }
    }
}

void ThumbnailCreator::deleteFromDiskFreedesktop(const QString& filePath) const
{
    QFile smallThumb(thumbnailPath(filePath, normalThumbnailDir()));
    QFile largeThumb(thumbnailPath(filePath, largeThumbnailDir()));

    smallThumb.remove();
    largeThumb.remove();
}

} // namespace Digikam
