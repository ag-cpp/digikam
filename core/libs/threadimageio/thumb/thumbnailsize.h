/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-07
 * Description : thumbnails size interface
 *
 * Copyright (C) 2004      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_THUMB_NAIL_SIZE_H
#define DIGIKAM_THUMB_NAIL_SIZE_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT ThumbnailSize
{

public:

    enum Size
    {
        Step        = 8,
        Tiny        = 32,
        VerySmall   = 64,
        MediumSmall = 80,
        /**
         * Most usable small size of thumbnails to prevent overloaded
         * overlays show under thumbs (as Pick label and Group indicator)
         * See bugs #321337 and #275381 for details.
         */
        Small       = 100,
        Medium      = 142,
        Large       = 160,
        Huge        = 256,
        HD          = 512,
        MAX         = 1024
    };

public:

    ThumbnailSize();
    explicit ThumbnailSize(int size);
    ThumbnailSize(const ThumbnailSize& thumbsize);

    ~ThumbnailSize();

    ThumbnailSize& operator=(const ThumbnailSize& thumbsize);
    bool operator==(const ThumbnailSize& thumbsize) const;
    bool operator!=(const ThumbnailSize& thumbsize) const;

    int size()                                      const;

public:

    static void setUseLargeThumbs(bool val);
    static bool getUseLargeThumbs();
    static void readSettings(const KConfigGroup& group);
    static void saveSettings(KConfigGroup& group, bool val);
    static int  maxThumbsSize();

private:

    int  m_Size;
};

} // namespace Digikam

#endif // DIGIKAM_THUMB_NAIL_SIZE_H
