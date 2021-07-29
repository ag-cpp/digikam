/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Parser
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
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

#include "imagequalityparser_p.h"

// Qt includes

#include <QScopedPointer>

// Local includes 

#include "noise_detector.h"
#include "exposure_detector.h"
#include "compression_detector.h"
#include "blur_detector.h"
#include "detector.h"
#include "imagequalitycalculator.h"

namespace Digikam
{

ImageQualityParser::ImageQualityParser(const DImg& image,
                                       const ImageQualityContainer& settings,
                                       PickLabel* const label)
    : d(new Private)
{
    d->imq     = settings;
    d->image   = image;
    d->label   = label;
}

ImageQualityParser::~ImageQualityParser()
{
    delete d;
}

void ImageQualityParser::startAnalyse()
{

    float blurLevel             = -1.0;
    float noiseLevel            = -1.0;
    float compressionLevel      = -1.0;
    float exposureLevel         = -1.0;
    float finalQuality          = -1.0;

    QScopedPointer<ImageQualityCalculator> calculator(new ImageQualityCalculator());

    const DetectorDistortion detector(d->image);

    // If blur option is selected in settings, run the blur detection algorithms

    if (d->running && d->imq.detectBlur)
    {
        blurLevel  = BlurDetector(detector, d->image).detect();
        
        qCDebug(DIGIKAM_DIMG_LOG) << "Amount of Blur present in image is:" << blurLevel;

        calculator->addDetectionResult(QLatin1String("BLUR"), blurLevel, d->imq.blurWeight);
    }

    if (d->running && d->imq.detectNoise)
    {
        noiseLevel = NoiseDetector(d->image).detect();
        
        qCDebug(DIGIKAM_DIMG_LOG) << "Amount of Noise present in image is:" << noiseLevel;

        calculator->addDetectionResult(QLatin1String("NOISE"), noiseLevel, d->imq.noiseWeight);
    }

    if (d->running && d->imq.detectCompression)
    {
        compressionLevel = CompressionDetector(detector).detect();

        qCDebug(DIGIKAM_DIMG_LOG) << "Amount of compression artifacts present in image is:" << compressionLevel;

        calculator->addDetectionResult(QLatin1String("COMPRESSION"), compressionLevel, d->imq.compressionWeight);
    }

    if (d->running && d->imq.detectExposure)
    {
        exposureLevel = ExposureDetector(detector).detect();

        qCDebug(DIGIKAM_DIMG_LOG) << "Under/Over exposure percents in image is: " << exposureLevel;

        calculator->addDetectionResult(QLatin1String("EXPOSURE"), exposureLevel, d->imq.exposureWeight);
    }

#ifdef TRACE

    // QFile filems("imgqsortresult.txt");

    // if (filems.open(QIODevice::Append | QIODevice::Text))
    // {
    //     QTextStream oms(&filems);
    //     oms << "File:" << d->image.originalFilePath() << endl;

    //     if (d->imq.detectBlur)
    //     {
    //         oms << "Blur Present:" << blur << endl;
    //         oms << "Blur Present(using LoG filter):"<< blur2 << endl;
    //     }

    //     if (d->imq.detectNoise)
    //     {
    //         oms << "Noise Present:" << noise << endl;
    //     }

    //     if (d->imq.detectCompression)
    //     {
    //         oms << "Compression Present:" << compressionLevel << endl;
    //     }

    //     if (d->imq.detectExposure)
    //     {
    //         oms << "Under-exposure Percents:" << underLevel << endl;
    //         oms << "Over-exposure Percents:"  << overLevel << endl;
    //     }

    //     filems.close();
    // }

#endif // TRACE

    // Calculating finalquality

    if (d->running)
    {
        finalQuality            =  calculator->calculateQuality();

        qCDebug(DIGIKAM_DIMG_LOG) << "Final Quality estimated: " << finalQuality;

        // Assigning PickLabels

        if      (finalQuality == -1.0)
        {
            *d->label = NoPickLabel;
        }
        else if ((int)finalQuality <= d->imq.rejectedThreshold)
        {
            *d->label = RejectedLabel;
        }
        else if (((int)finalQuality >   d->imq.rejectedThreshold) &&
                 ((int)finalQuality <=  d->imq.acceptedThreshold))
        {
            *d->label = PendingLabel;
        }
        else
        {
            *d->label = AcceptedLabel;
        }
    }
    else
    {
        *d->label = NoPickLabel;
    }
}

void ImageQualityParser::cancelAnalyse()
{
    d->running = false;
}

} // namespace Digikam
