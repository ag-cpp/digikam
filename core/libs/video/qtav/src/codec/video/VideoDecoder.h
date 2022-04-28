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

#ifndef QTAV_VIDEO_DECODER_H
#define QTAV_VIDEO_DECODER_H

// Qt includes

#include <QStringList>

// Local includes

#include "AVDecoder.h"
#include "VideoFrame.h"

namespace QtAV
{

typedef int VideoDecoderId;

/*!
    Useful properties.
    A key is a string, a value can be int, bool or string. Both int and string are valid for enumerate
    properties. Flag properties must use int if more than 1 value is used.
    e.g. decoder->setProperty("display", 1) equals decoder->setProperty("display", "GLX")
    setOptions() also applies the properties.
    avcodec (also for VA-API, DXVA, VDA)
      Use AVCodecContext options
    CUDA
      surfaces: 0 is auto
      deinterlace: 0 "Weave", 1 "Bob", 2 "Adaptive"
    VA-API
      display: 0 "X11", 1 "GLX", 2 "DRM"
    DXVA, VA-API
      surfaces: 0 default
    DXVA, VA-API, VDA:
      sse4: bool
    CedarV
      neon: bool
    FFmpeg
      skip_loop_filter, skip_idct, skip_frame: -16 "None", 0: "Default", 8 "NoRef", 16 "Bidir", 32 "NoKey", 64 "All"
      threads: int, 0 is auto
      vismv(motion vector visualization): flag, 0 "NO", 1 "PF", 2 "BF", 4 "BB"
 */

class VideoDecoderPrivate;

class QTAV_EXPORT VideoDecoder : public AVDecoder
{
    Q_DISABLE_COPY(VideoDecoder)
    DPTR_DECLARE_PRIVATE(VideoDecoder)

public:

    static QStringList supportedCodecs();
    static VideoDecoder* create(VideoDecoderId id);

    /*!
     * \brief create
     * create a decoder from registered name. FFmpeg decoder will be created for empty name
     * \param name can be "FFmpeg", "CUDA", "VDA", "VAAPI", "DXVA", "Cedarv"
     * \return 0 if not registered
     */
    static VideoDecoder* create(const char* name = "FFmpeg");
    virtual VideoDecoderId id() const = 0;
    QString name() const override;                   // name from factory
    virtual VideoFrame frame() = 0;

public:

    typedef int Id;
    static QVector<VideoDecoderId> registered();

    template<class C> static bool Register(VideoDecoderId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    /*!
     * \brief next
     * \param id NULL to get the first id address
     * \return address of id or NULL if not found/end
     */
    static VideoDecoderId* next(VideoDecoderId* id = nullptr);
    static const char* name(VideoDecoderId id);
    static VideoDecoderId id(const char* name);

private:

    template<class C>
    static VideoDecoder* create()
    {
        return new C();
    }

    typedef VideoDecoder* (*VideoDecoderCreator)();

protected:

    static bool Register(VideoDecoderId id, VideoDecoderCreator, const char *name);

protected:

    VideoDecoder(VideoDecoderPrivate& d);

private:

    VideoDecoder();
};

extern QTAV_EXPORT VideoDecoderId VideoDecoderId_FFmpeg;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_CUDA;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_DXVA;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_D3D11;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_VAAPI;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_Cedarv;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_VDA;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_VideoToolbox;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_MediaCodec;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_MMAL;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_QSV;
extern QTAV_EXPORT VideoDecoderId VideoDecoderId_CrystalHD;

} // namespace QtAV

#endif // QTAV_VIDEO_DECODER_H
