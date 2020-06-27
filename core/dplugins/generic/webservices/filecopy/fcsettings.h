/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-06-28
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
 * Copyright (C) 2020      by Marcel Mathis <maeseee at gmail dot com>
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

#ifndef DIGIKAM_FC_SETTINGS_H
#define DIGIKAM_FC_SETTINGS_H

#include <memory>

namespace DigikamGenericFileCopyPlugin
{

enum class ImageFormat
{
    JPEG = 0,
    PNG
};

struct ChangeImageProperties
{
    uint imageSize;
    ImageFormat imageFormat;
    uint imageCompression;
    bool removeMetadata;
};
using ChangeImagePropertiesPtr = std::shared_ptr<ChangeImageProperties>;

} // namespace DigikamGenericFileCopyPlugin

#endif // DIGIKAM_MAIL_SETTINGS_H
