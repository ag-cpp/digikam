/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#include "ImageConverter.h"
#include "ImageConverter_p.h"
#include "private/AVCompat.h"
#include "private/mkid.h"
#include "private/factory.h"
#include "digikam_debug.h"

namespace QtAV
{
ImageConverterId ImageConverterId_FF = mkid::id32base36_6<'F', 'F', 'm', 'p', 'e', 'g'>::value;
FACTORY_REGISTER(ImageConverter, FF, "FFmpeg")

class ImageConverterFFPrivate Q_DECL_FINAL: public ImageConverterPrivate
{
public:
    ImageConverterFFPrivate()
        : sws_ctx(0)
        , update_eq(true)
    {}
    ~ImageConverterFFPrivate() {
        if (sws_ctx) {
            sws_freeContext(sws_ctx);
            sws_ctx = 0;
        }
    }
    virtual bool setupColorspaceDetails(bool force = true) Q_DECL_FINAL;

    SwsContext *sws_ctx;
    bool update_eq;
};

ImageConverterFF::ImageConverterFF()
    :ImageConverter(*new ImageConverterFFPrivate())
{
}

bool ImageConverterFF::check() const
{
    if (!ImageConverter::check())
        return false;
    DPTR_D(const ImageConverterFF);
    if (sws_isSupportedInput((AVPixelFormat)d.fmt_in) <= 0) {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << QString::asprintf("Input pixel format not supported (%s)", av_get_pix_fmt_name((AVPixelFormat)d.fmt_in));
        return false;
    }
    if (sws_isSupportedOutput((AVPixelFormat)d.fmt_out) <= 0) {
        qCWarning(DIGIKAM_QTAV_LOG_WARN) << QString::asprintf("Output pixel format not supported (%s)", av_get_pix_fmt_name((AVPixelFormat)d.fmt_out));
        return false;
    }
    return true;
}

bool ImageConverterFF::convert(const quint8 *const src[], const int srcStride[], quint8 *const dst[], const int dstStride[])
{
    DPTR_D(ImageConverterFF);
    //Check out dimension. equals to in dimension if not setted. TODO: move to another common func
    if (d.w_out == 0 || d.h_out == 0) {
        if (d.w_in == 0 || d.h_in == 0)
            return false;
        setOutSize(d.w_in, d.h_in);
    }
//TODO: move those code to prepare()
    d.sws_ctx = sws_getCachedContext(d.sws_ctx
            , d.w_in, d.h_in, (AVPixelFormat)d.fmt_in
            , d.w_out, d.h_out, (AVPixelFormat)d.fmt_out
            , (d.w_in == d.w_out && d.h_in == d.h_out) ? SWS_POINT : SWS_FAST_BILINEAR //SWS_BICUBIC
            , NULL, NULL, NULL
            );
    //int64_t flags = SWS_CPU_CAPS_SSE2 | SWS_CPU_CAPS_MMX | SWS_CPU_CAPS_MMX2;
    //av_opt_set_int(d.sws_ctx, "sws_flags", flags, 0);
    if (!d.sws_ctx)
        return false;
    d.setupColorspaceDetails(false);
    int result_h = sws_scale(d.sws_ctx, src, srcStride, 0, d.h_in, dst, dstStride);
    if (result_h != d.h_out) {
        qCDebug(DIGIKAM_QTAV_LOG) << QString::asprintf("convert failed: %d, %d", result_h, d.h_out);
        return false;
    }
    Q_UNUSED(result_h);
    for (int i = 0; i < d.pitchs.size(); ++i) {
        d.bits[i] = dst[i];
        d.pitchs[i] = dstStride[i];
    }
    return true;
}

bool ImageConverterFFPrivate::setupColorspaceDetails(bool force)
{
    if (!sws_ctx) {
        update_eq = true;
        return false;
    }
    if (force)
        update_eq = true;
    if (!update_eq) {
        return true;
    }
    const int srcRange = range_in == ColorRange_Limited ? 0 : 1;
    int dstRange = range_out == ColorRange_Limited ? 0 : 1;
    // TODO: color space
    bool supported = sws_setColorspaceDetails(sws_ctx, sws_getCoefficients(SWS_CS_DEFAULT)
                             , srcRange, sws_getCoefficients(SWS_CS_DEFAULT)
                             , dstRange
                             , ((brightness << 16) + 50)/100
                             , (((contrast + 100) << 16) + 50)/100
                             , (((saturation + 100) << 16) + 50)/100
                             ) >= 0;
    //sws_init_context(d.sws_ctx, NULL, NULL);
    update_eq = false;
    return supported;
}

} // namespace QtAV
