/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A tool to export items to Rajce web service
 *
 * Copyright (C) 2011      by Lukas Krejci <krejci.l at centrum dot cz>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RAJCE_ALBUM_H
#define DIGIKAM_RAJCE_ALBUM_H

// Qt includes

#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QMetaType>

namespace DigikamGenericRajcePlugin
{

struct RajceAlbum
{
    RajceAlbum()
    {
        isHidden   = false;
        isSecure   = false;
        photoCount = 0;
        id         = 0;
    }

    bool      isHidden;
    bool      isSecure;

    unsigned  photoCount;
    unsigned  id;

    QString   name;
    QString   description;
    QString   url;
    QString   thumbUrl;
    QString   bestQualityThumbUrl;

    QDateTime createDate;
    QDateTime updateDate;
    QDateTime validFrom;
    QDateTime validTo;
};

} // namespace DigikamGenericRajcePlugin

QTextStream& operator<<(QTextStream& str, const DigikamGenericRajcePlugin::RajceAlbum& a);
QDebug operator<<(QDebug d, const DigikamGenericRajcePlugin::RajceAlbum& a);

Q_DECLARE_METATYPE(DigikamGenericRajcePlugin::RajceAlbum)

#endif // DIGIKAM_RAJCE_ALBUM_H
