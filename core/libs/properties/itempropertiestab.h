/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-19
 * Description : A tab to display general item information
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_PROPERTIES_TAB_H
#define DIGIKAM_ITEM_PROPERTIES_TAB_H

// Qt includes

#include <QString>
#include <QColor>
#include <QUrl>
#include <QFileInfo>

// Local includes

#include "dexpanderbox.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ItemPropertiesTab : public DExpanderBox
{
    Q_OBJECT

public:

    explicit ItemPropertiesTab(QWidget* const parent);
    ~ItemPropertiesTab() override;

    void setCurrentURL(const QUrl& url = QUrl());

    void setPhotoInfoDisable(const bool b);
    void setVideoInfoDisable(const bool b);
    void showOrHideCaptionAndTags();

    void setFileModifiedDate(const QString& str);
    void setFileSize(const QString& str);
    void setFileOwner(const QString& str);
    void setFilePermissions(const QString& str);

    void setItemDimensions(const QString& str);
    void setImageRatio(const QString& str);
    void setImageMime(const QString& str);
    void setImageBitDepth(const QString& str);
    void setImageColorMode(const QString& str);
    void setHasSidecar(const QString& str);

    void setPhotoMake(const QString& str);
    void setPhotoModel(const QString& str);
    void setPhotoDateTime(const QString& str);
    void setPhotoLens(const QString& str);
    void setPhotoAperture(const QString& str);
    void setPhotoFocalLength(const QString& str);
    void setPhotoExposureTime(const QString& str);
    void setPhotoSensitivity(const QString& str);
    void setPhotoExposureMode(const QString& str);
    void setPhotoFlash(const QString& str);
    void setPhotoWhiteBalance(const QString& str);

    void setVideoAspectRatio(const QString& str);
    void setVideoAudioBitRate(const QString& str);
    void setVideoAudioChannelType(const QString& str);
    void setVideoAudioCodec(const QString& str);
    void setVideoDuration(const QString& str);
    void setVideoFrameRate(const QString& str);
    void setVideoVideoCodec(const QString& str);

    void setCaption(const QString& str);
    void setPickLabel(int pickId);
    void setColorLabel(int colorId);
    void setRating(int rating);
    void setTags(const QStringList& tagPaths, const QStringList& tagNames = QStringList());

    /**
     * Shortens the tag paths by sorting and then cutting identical paths from the second
     * and following paths (only the first item gives the full path).
     * If you want to retain information about which tag path is sorted where,
     * you can optionally give a QVariant list. This list shall contain an identifier
     * for the tag path at the same index and will be resorted as the returned list.
     */
    static QStringList shortenedTagPaths(const QStringList& tagPaths, QList<QVariant>* identifiers = nullptr);

    /**
     * This methods shortens make an model camera info to prevent bloating GUI
     * See bug #265231 for details.
     */
    static void shortenedMakeInfo(QString& make);
    static void shortenedModelInfo(QString& model);

    /**
     * Write a string with aspect ratio information formatted
     */
    static bool aspectRatioToString(int width, int height, QString& arString);

    /**
     * Return file permissions string.
     */
    static QString permissionsString(const QFileInfo& fi);

    /**
     * Return an human readable string of file size in 'bytes'.
     * If 'si' is true, a decade of bytes is interpreted on base of 1000 byte, else 1024.
     */
    static QString humanReadableBytesCount(qint64 bytes, bool si = false);

private:

    static double doubleToHumanReadableFraction(double val, long* num, long* den, long maxden = 2);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_PROPERTIES_TAB_H
