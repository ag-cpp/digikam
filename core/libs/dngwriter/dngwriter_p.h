/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DNG_WRITER_PRIVATE_H
#define DIGIKAM_DNG_WRITER_PRIVATE_H

#include "dngwriter.h"

// Qt includes

#include <QImage>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QTemporaryFile>
#include <QDateTime>
#include <QPointer>
#include <QScopedPointer>

// Local includes

#include "drawdecoder.h"

// DNG SDK includes

#include "dng_camera_profile.h"
#include "dng_color_space.h"
#include "dng_exceptions.h"
#include "dng_file_stream.h"
#include "dng_globals.h"
#include "dng_host.h"
#include "dng_ifd.h"
#include "dng_image_writer.h"
#include "dng_info.h"
#include "dng_linearization_info.h"
#include "dng_memory_stream.h"
#include "dng_mosaic_info.h"
#include "dng_negative.h"
#include "dng_preview.h"
#include "dng_read_image.h"
#include "dng_render.h"
#include "dng_simple_image.h"
#include "dng_tag_codes.h"
#include "dng_tag_types.h"
#include "dng_tag_values.h"
#include "dng_xmp.h"
#include "dng_xmp_sdk.h"

// Local includes

#include "digikam_debug.h"
#include "dngwriterhost.h"
#include "dfileoperations.h"
#include "dmetadata.h"

#define CHUNK 65536

namespace Digikam
{

class Q_DECL_HIDDEN DNGWriter::Private
{

public:

    enum DNGBayerPattern
    {
        Unknown = 1,
        LinearRaw,
        Standard,
        Fuji,
        FourColor
    };

public:

    explicit Private();
    ~Private();

public:

    void          reset();
    void          cleanup();
    dng_date_time dngDateTime(const QDateTime& qDT)          const;

    bool fujiRotate(QByteArray& rawData, DRawInfo& identify) const;

public:

    bool    cancel;
    bool    jpegLossLessCompression;
    bool    updateFileDate;
    bool    backupOriginalRawFile;

    int     previewMode;

    QString inputFile;
    QString outputFile;
};

} // namespace Digikam

#endif // DIGIKAM_DNG_WRITER_PRIVATE_H
