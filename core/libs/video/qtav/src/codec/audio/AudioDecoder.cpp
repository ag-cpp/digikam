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

#include "AudioDecoder.h"
#include "private/AVDecoder_p.h"
#include "private/AVCompat.h"
#include "AudioResampler.h"
#include "private/factory.h"
#include "digikam_debug.h"

namespace QtAV
{

FACTORY_DEFINE(AudioDecoder)
// TODO: why vc can not declare extern func in a class member? resolved as &func@@YAXXZ
extern bool RegisterAudioDecoderFFmpeg_Man();
void AudioDecoder::registerAll() {
    static bool done = false;
    if (done)
        return;
    done = true;
    RegisterAudioDecoderFFmpeg_Man();
}

QStringList AudioDecoder::supportedCodecs()
{
    static QStringList codecs;
    if (!codecs.isEmpty())
        return codecs;
    const AVCodec* c = NULL;
#if AVCODEC_STATIC_REGISTER
    void* it = NULL;
    while ((c = av_codec_iterate(&it))) {
#else
    avcodec_register_all();
    while ((c = av_codec_next(c))) {
#endif
        if (!av_codec_is_decoder(c) || c->type != AVMEDIA_TYPE_AUDIO)
            continue;
        codecs.append(QString::fromLatin1(c->name));
    }
    return codecs;
}

AudioDecoderPrivate::AudioDecoderPrivate()
    : AVDecoderPrivate()
    , resampler(0)
{
    resampler = AudioResampler::create(AudioResamplerId_FF);
    if (!resampler)
        resampler = AudioResampler::create(AudioResamplerId_Libav);
    if (resampler)
        resampler->setOutSampleFormat(AV_SAMPLE_FMT_FLT);
}

AudioDecoderPrivate::~AudioDecoderPrivate()
{
    if (resampler) {
        delete resampler;
        resampler = 0;
    }
}

AudioDecoder::AudioDecoder(AudioDecoderPrivate &d):
    AVDecoder(d)
{
}

QString AudioDecoder::name() const
{
    return QLatin1String(AudioDecoder::name(id()));
}

QByteArray AudioDecoder::data() const
{
    return d_func().decoded;
}

AudioResampler* AudioDecoder::resampler()
{
    return d_func().resampler;
}

} // namespace QtAV
