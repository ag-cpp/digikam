/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-11-07
 * Description : Directory watch interface
 *
 * Copyright (C) 2011      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ALBUM_WATCH_H
#define DIGIKAM_ALBUM_WATCH_H

// Qt includes

#include <QObject>
#include <QString>
#include <QUrl>

namespace Digikam
{

class Album;
class PAlbum;
class AlbumManager;
class DbEngineParameters;

class AlbumWatch : public QObject
{
    Q_OBJECT

public:

    explicit AlbumWatch(AlbumManager* const parent = nullptr);
    ~AlbumWatch() override;

    void clear();
    void removeWatchedPAlbums(const PAlbum* const album);
    void setDbEngineParameters(const DbEngineParameters& params);

protected Q_SLOTS:

    void slotAlbumAdded(Album* album);
    void slotAlbumAboutToBeDeleted(Album* album);
    void slotQFSWatcherDirty(const QString& path);

private:

    void rescanDirectory(const QString& dir);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ALBUM_WATCH_H
