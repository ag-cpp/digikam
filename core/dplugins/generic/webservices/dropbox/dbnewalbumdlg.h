/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a tool to export images to Dropbox web service
 *
 * Copyright (C) 2013      by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_NEW_ALBUM_DLG_H
#define DIGIKAM_DB_NEW_ALBUM_DLG_H

// Local includes

#include "wsnewalbumdialog.h"

using namespace Digikam;

namespace DigikamGenericDropBoxPlugin
{

class DBFolder;

class DBNewAlbumDlg : public WSNewAlbumDialog
{
    Q_OBJECT

public:

    explicit DBNewAlbumDlg(QWidget* const parent, const QString& toolName);
    ~DBNewAlbumDlg() override;

    void getFolderTitle(DBFolder& folder);
};

} // namespace DigikamGenericDropBoxPlugin

#endif // DIGIKAM_DB_NEW_ALBUM_DLG_H
