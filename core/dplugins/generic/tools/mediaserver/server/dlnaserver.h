/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : a media server to export collections through DLNA.
 *               Implementation inspired on Platinum File Media Server.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DLNA_SERVER_H
#define DIGIKAM_DLNA_SERVER_H

// Qt includes

#include <QImage>
#include <QString>
#include <QByteArray>

// Qt includes

#include <QApplication>
#include <QStandardPaths>
#include <QBuffer>
#include <QIODevice>

// Local includes

#include "digikam_debug.h"
#include "dmediaserver.h"
#include "dlnaserverdelegate.h"

// Platinum includes

#include "Neptune.h"
#include "PltMediaServer.h"

namespace Digikam
{

/**
 * File Media Server for digiKam.
 * The DLNAMediaServer class is based on PLT_MediaServer implementation
 * for a file system backed Media Server with album contents.
 */
class DLNAMediaServer : public PLT_MediaServer,
                        public DLNAMediaServerDelegate
{

public:

    explicit DLNAMediaServer(const char*  friendly_name,
                             bool         show_ip = false,
                             const char*  uuid = nullptr,
                             NPT_UInt16   port = 0,
                             bool         port_rebind = false);

    void addAlbumsOnServer(const MediaServerMap& map);

    NPT_Result SetupIcons() override;

protected:

    ~DLNAMediaServer()      override;

private:

    QByteArray iconData(const QImage& img, int size, QString& uri, int& depth) const;
};

} // namespace Digikam

#endif // DIGIKAM_DLNA_SERVER_H
