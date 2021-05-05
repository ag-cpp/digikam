/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2000-06-08
 * Description : Core database <-> thumbnail database interface
 *
 * Copyright (C) 2009      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CORE_DB_THUMB_INFO_PROVIDER_H
#define DIGIKAM_CORE_DB_THUMB_INFO_PROVIDER_H

// Local includes

#include "digikam_export.h"
#include "thumbnailinfo.h"
#include "loadsavethread.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ThumbsDbInfoProvider : public ThumbnailInfoProvider
{
public:

    ThumbnailInfo thumbnailInfo(const ThumbnailIdentifier& identifier) override;
};

// ----------------------------------------------------------------------------------------------

class DIGIKAM_DATABASE_EXPORT DatabaseLoadSaveFileInfoProvider : public LoadSaveFileInfoProvider
{
public:

    int   orientationHint(const QString& path) override;
    QSize dimensionsHint(const QString& path)  override;
};

} // namespace Digikam

#endif // DIGIKAM_CORE_DB_THUMB_INFO_PROVIDER_H
