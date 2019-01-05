/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free dot fr>
 * Copyright (C) 2012-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_JALBUM_INFO_H
#define DIGIKAM_JALBUM_INFO_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QDebug>

// Local includes

#include "jalbumconfig.h"
#include "dinfointerface.h"

namespace Digikam
{

/**
 * This class stores all the export settings. It is initialized by the
 * Wizard and read by the Generator.
 */
class JalbumInfo : public JalbumConfig
{
public:

    enum ImageGetOption
    {
        ALBUMS = 0,
        IMAGES
    };

public:

    explicit JalbumInfo(DInfoInterface* const iface = 0);
    ~JalbumInfo();

public:

    ImageGetOption            m_getOption;      // Type of image selection (albums or images list).

    DInfoInterface::DAlbumIDs m_albumList;      // Albums list for ImageGetOption::ALBUMS selection.

    QList<QUrl>               m_imageList;      // Images list for ImageGetOption::IMAGES selection.

    DInfoInterface*           m_iface;          // Interface to handle items information.

private:

};

//! qDebug() stream operator. Writes property @a t to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const JalbumInfo& t);

} // namespace Digikam

#endif // DIGIKAM_JALBUM_INFO_H
