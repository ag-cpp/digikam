/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-04-19
 * Description : time adjust settings container.
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_TIME_ADJUST_CONTAINER_H
#define DIGIKAM_TIME_ADJUST_CONTAINER_H

// Qt includes

#include <QUrl>
#include <QDateTime>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

/**
 * Container that store all timestamp adjustments.
 */
class DIGIKAM_EXPORT TimeAdjustContainer
{

public:

    enum UseDateSource
    {
        APPDATE = 0,
        FILENAME,
        FILEDATE,
        METADATADATE,
        CUSTOMDATE
    };

    enum UseMetaDateType
    {
        EXIFIPTCXMP = 0,
        EXIFCREATED,
        EXIFORIGINAL,
        EXIFDIGITIZED,
        IPTCCREATED,
        XMPCREATED
    };

    enum UseFileDateType
    {
        FILELASTMOD = 0,
        FILECREATED
    };

    enum AdjType
    {
        COPYVALUE = 0,
        ADDVALUE,
        SUBVALUE,
        INTERVAL
    };

public:

    TimeAdjustContainer();
    ~TimeAdjustContainer();

    /// Check if at least one option is selected
    bool atLeastOneUpdateToProcess()                                const;

    QDateTime calculateAdjustedDate(const QDateTime& originalTime);
    QDateTime getDateTimeFromUrl(const QUrl& url)                   const;

public:

    QDateTime customDate;
    QDateTime customTime;
    QDateTime intervalTime;
    QDateTime adjustmentTime;

    bool      updIfAvailable;
    bool      updEXIFModDate;
    bool      updEXIFOriDate;
    bool      updEXIFDigDate;
    bool      updEXIFThmDate;
    bool      updIPTCDate;
    bool      updXMPVideo;
    bool      updXMPDate;
    bool      updFileModDate;

    int       dateSource;
    int       metadataSource;
    int       fileDateSource;
    int       adjustmentType;
    int       adjustmentDays;
};

// -------------------------------------------------------------------

/**
 * Container that hold the time difference for clock photo dialog.
 */
class DeltaTime
{

public:

    explicit DeltaTime();

    ~DeltaTime();

    /// Check if at least one option is selected
    bool isNull() const;

public:

    bool deltaNegative;

    int  deltaDays;
    int  deltaHours;
    int  deltaMinutes;
    int  deltaSeconds;
};

} // namespace Digikam

#endif // DIGIKAM_TIME_ADJUST_CONTAINER_H
