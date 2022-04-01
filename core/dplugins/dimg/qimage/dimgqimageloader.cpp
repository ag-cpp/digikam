/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : A QImage loader for DImg framework.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2021 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#include "dimgqimageloader.h"

// Qt includes

#include <QImage>
#include <QByteArray>
#include <QColorSpace>
#include <QImageReader>

// Local includes

#include "digikam_debug.h"
#include "dimgloaderobserver.h"

namespace DigikamQImageDImgPlugin
{

DImgQImageLoader::DImgQImageLoader(DImg* const image)
    : DImgLoader  (image),
      m_hasAlpha  (false),
      m_sixteenBit(false)
{
}

DImgQImageLoader::~DImgQImageLoader()
{
}

bool DImgQImageLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool DImgQImageLoader::sixteenBit() const
{
    return m_sixteenBit;
}

bool DImgQImageLoader::isReadOnly() const
{
    return false;
}

} // namespace DigikamQImageDImgPlugin
