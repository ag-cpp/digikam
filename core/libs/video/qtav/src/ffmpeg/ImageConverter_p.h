/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#ifndef QTAV_IMAGE_CONVERTER_P_H
#define QTAV_IMAGE_CONVERTER_P_H

#include "ImageConverter.h"

// Qt includes

#include <QVector>

// Local includee

#include "AVCompat.h"

namespace QtAV
{

class ImageConverter;

class Q_DECL_HIDDEN ImageConverterPrivate : public DPtrPrivate<ImageConverter>
{
public:

    ImageConverterPrivate()
        : w_in       (0),
          h_in       (0),
          w_out      (0),
          h_out      (0),
          fmt_in     (QTAV_PIX_FMT_C(YUV420P)),
          fmt_out    (QTAV_PIX_FMT_C(RGB32)),
          range_in   (ColorRange_Unknown),
          range_out  (ColorRange_Unknown),
          brightness (0),
          contrast   (0),
          saturation (0),
          update_data(true),
          out_offset (0)
    {
        bits.reserve(8);
        pitchs.reserve(8);
    }

    virtual bool setupColorspaceDetails(bool force = true)
    {
        Q_UNUSED(force);

        return true;
    }

    int              w_in;
    int              h_in;
    int              w_out;
    int              h_out;
    AVPixelFormat    fmt_in;
    AVPixelFormat    fmt_out;
    ColorRange       range_in;
    ColorRange       range_out;
    int              brightness;
    int              contrast;
    int              saturation;
    bool             update_data;
    int              out_offset;
    QByteArray       data_out;
    QVector<quint8*> bits;
    QVector<int>     pitchs;

private:

    Q_DISABLE_COPY(ImageConverterPrivate);
};

} // namespace QtAV

#endif // QTAV_IMAGE_CONVERTER_P_H
