/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : unit test of qtav static configuration
 *
 * Copyright (C) 2022 by Steve Robbins <steve at sumost dot ca>
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

// Qt includes

#include <QTest>

// Local includes

#include "VideoDecoder.h"
#include "VideoEncoder.h"
#include "AudioDecoder.h"
#include "AudioEncoder.h"
#include "AVMuxer.h"
#include "LibAVFilter.h"
#include <VideoDecoder.h>


using namespace QtAV;

class CheckConfig : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void DecoderConfig()
    {
        auto videoCodecs = VideoDecoder::supportedCodecs();
        QVERIFY(videoCodecs.size() > 0);
        //QVERIFY(videoCodecs.contains(QLatin1String("mpegvideo")));

        auto audioCodecs = AudioDecoder::supportedCodecs();
        QVERIFY(audioCodecs.size() > 0);
        //QVERIFY(audioCodecs.contains(QLatin1String("mp3")));
    }
        
    void EncoderConfig()
    {
        auto videoCodecs = VideoEncoder::supportedCodecs();
        QVERIFY(videoCodecs.size() > 0);
        //QVERIFY(videoCodecs.contains(QLatin1String("mpeg4")));

        auto audioCodecs = AudioEncoder::supportedCodecs();
        QVERIFY(audioCodecs.size() > 0);
        //QVERIFY(audioCodecs.contains(QLatin1String("mp2")));
    }

    void MuxerConfig()
    {
        auto formats = AVMuxer::supportedFormats();
        QVERIFY(formats.size() > 0);
        //QVERIFY(formats.contains(QLatin1String("mpeg")));
        
        auto extensions = AVMuxer::supportedExtensions();
        QVERIFY(extensions.size() > 0);
        //QVERIFY(extensions.contains(QLatin1String("xbm")));
    }
    
    void LibAvConfig()
    {
        auto videoFilters = LibAVFilter::videoFilters();
        QVERIFY(videoFilters.size() > 0);
        //QVERIFY(videoFilters.contains(QLatin1String("chromakey")));
        
        auto audioFilters = LibAVFilter::audioFilters();
        QVERIFY(audioFilters.size() > 0);
        //QVERIFY(audioFilters.contains(QLatin1String("earwax")));
    }
};

QTEST_MAIN(CheckConfig)
#include "checkconfig_utest.moc"
