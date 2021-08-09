/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Filtering scanned image for detection
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_SCANFILTER_H
#define DIGIKAM_SCANFILTER_H

#include "iteminfo.h"

namespace Digikam
{

class Q_DECL_HIDDEN ScanFilter
{
public:

    enum FilterMode
    {
        /// Will read any given image
        ScanAll,

        /// Will skip any image that is already marked as scanned
        SkipAlreadyScanned,

        // TODO facesengine 5 review scan mode for classification
    };

public:

    explicit ScanFilter(FilterMode mode);
    ~ScanFilter();

public:

    /**
     * filter returns true if the item is filtered, and false if it's not
     */ 
    bool filter(const ItemInfo& info) const;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif //DIGIKAM_SCANFILTER_H