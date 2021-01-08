/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : a media server to export collections through DLNA.
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

#ifndef DIGIKAM_DMEDIA_SERVER_H
#define DIGIKAM_DMEDIA_SERVER_H

// Qt includes

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>
#include <QList>

namespace Digikam
{

/// A kind of map of albums with urls contents to share with DLNA media server.
typedef QMap<QString, QList<QUrl> > MediaServerMap;

class DMediaServer : public QObject
{
    Q_OBJECT

public:

    explicit DMediaServer(QObject* const parent = nullptr);
    ~DMediaServer() override;

    /**
     * Initialize the internal server instance and return true if all is ready to host contents.
     * If port = 0, the server will select one automatically, else it will use the specified one.
     */
    bool init(int port = 0);

    /**
     * To share a list of albums with dedicated urls list for each one.
     */
    void addAlbumsOnServer(const MediaServerMap& map);

private:

    class Private;
    Private* const d;

private:

    Q_DISABLE_COPY(DMediaServer)
};

} // namespace Digikam

#endif // DIGIKAM_DMEDIA_SERVER_H
