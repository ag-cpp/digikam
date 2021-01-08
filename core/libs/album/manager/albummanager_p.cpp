/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-06-15
 * Description : Albums manager interface - private containers.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "albummanager_p.h"

namespace Digikam
{

PAlbumPath::PAlbumPath()
    : albumRootId(-1)
{
}

PAlbumPath::PAlbumPath(int albumRootId, const QString& albumPath)
    : albumRootId(albumRootId),
      albumPath  (albumPath)
{
}

PAlbumPath::PAlbumPath(PAlbum* const album)
{
    if (album->isRoot())
    {
        albumRootId = -1;
    }
    else
    {
        albumRootId = album->albumRootId();
        albumPath   = album->albumPath();
    }
}

bool PAlbumPath::operator==(const PAlbumPath& other) const
{
    return ((other.albumRootId == albumRootId) &&
            (other.albumPath   == albumPath));
}

// -----------------------------------------------------------------------------------

AlbumManager::Private::Private()
    : changed                 (false),
      hasPriorizedDbPath      (false),
      dbFakeConnection        (false),
      showOnlyAvailableAlbums (false),
      longTimeMessageBoxResult(-1),
      albumListJob            (nullptr),
      dateListJob             (nullptr),
      tagListJob              (nullptr),
      personListJob           (nullptr),
      albumWatch              (nullptr),
      rootPAlbum              (nullptr),
      rootTAlbum              (nullptr),
      rootDAlbum              (nullptr),
      rootSAlbum              (nullptr),
      currentlyMovingAlbum    (nullptr),
      changingDB              (false),
      scanPAlbumsTimer        (nullptr),
      scanTAlbumsTimer        (nullptr),
      scanSAlbumsTimer        (nullptr),
      scanDAlbumsTimer        (nullptr),
      updatePAlbumsTimer      (nullptr),
      albumItemCountTimer     (nullptr),
      tagItemCountTimer       (nullptr)
{
}

QString AlbumManager::Private::labelForAlbumRootAlbum(const CollectionLocation& location)
{
    QString label = location.label();

    if (label.isEmpty())
    {
        label = location.albumRootPath();
    }

    return label;
}

// -----------------------------------------------------------------------------------

ChangingDB::ChangingDB(AlbumManager::Private* const d)
    : d(d)
{
    d->changingDB = true;
}

ChangingDB::~ChangingDB()
{
    d->changingDB = false;
}

} // namespace Digikam
