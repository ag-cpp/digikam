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

#include "scanfilter.h"

#include "faceutils.h"

namespace Digikam
{
class Q_DECL_HIDDEN ScanFilter::Private
{
public:

    explicit Private(FilterMode mode)
        : mode(mode)
    {
    }

    ~Private()
    {
    }

public:

    FilterMode mode;
};

ScanFilter::ScanFilter(FilterMode mode)
    : d(new Private(mode))
{
}

ScanFilter::~ScanFilter()
{
    delete d;
}

bool ScanFilter::filter(const ItemInfo& info) const
{
    FaceUtils utils;

    switch (d->mode)
    {
        case ScanAll:
        {
            return false;
        }

        case SkipAlreadyScanned:
        {
            if (!utils.hasBeenScanned(info))
            {
                return false;
            }

            break;
        }
    }

    return true;
}

} // namespace Digikam