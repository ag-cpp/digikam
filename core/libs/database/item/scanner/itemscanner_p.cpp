/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-09-19
 * Description : Scanning a single item - private containers.
 *
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "itemscanner_p.h"

namespace Digikam
{

ItemScannerCommit::ItemScannerCommit()
    : operation(NoOp),
      copyImageAttributesId(-1),
      commitItemInformation(false),
      commitImageMetadata(false),
      commitVideoMetadata(false),
      commitItemPosition(false),
      commitItemComments(false),
      commitItemCopyright(false),
      commitFaces(false),
      commitIPTCCore(false),
      hasColorTag(false),
      hasPickTag(false)
{
}

// ---------------------------------------------------------------------------------------

LessThanByProximityToSubject::LessThanByProximityToSubject(const ItemInfo& subject)
    : subject(subject)
{
}

bool LessThanByProximityToSubject::operator()(const ItemInfo& a, const ItemInfo& b)
{
    if (a.isNull() || b.isNull())
    {
        // both null: false
        // only a null: a greater than b (null infos at end of list)
        //  (a && b) || (a && !b) = a
        // only b null: a less than b
        if (a.isNull())
        {
            return false;
        }

        return true;
    }

    if (a == b)
    {
        return false;
    }

    // same collection
    if (a.albumId() != b.albumId())
    {
        // same album
        if (a.albumId() == subject.albumId())
        {
            return true;
        }

        if (b.albumId() == subject.albumId())
        {
            return false;
        }

        if (a.albumRootId() != b.albumRootId())
        {
            // different collection
            if (a.albumRootId() == subject.albumRootId())
            {
                return true;
            }

            if (b.albumRootId() == subject.albumRootId())
            {
                return false;
            }
        }
    }

    if (a.modDateTime() != b.modDateTime())
    {
        return (a.modDateTime() < b.modDateTime());
    }

    if (a.name() != b.name())
    {
        return qAbs(a.name().compare(subject.name())) < qAbs(b.name().compare(subject.name()));
    }

    // last resort
    return (a.id() < b.id());
}

// ---------------------------------------------------------------------------

ItemScanner::Private::Private()
    : hasImage(false),
      hasMetadata(false),
      loadedFromDisk(false),
      metadata(new DMetadata),
      scanMode(ModifiedScan),
      hasHistoryToResolve(false)
{
    timer.start();
}

ItemScanner::Private::~Private()
{
    delete metadata;
}

} // namespace Digikam
