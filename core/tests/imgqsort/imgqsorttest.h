/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMGQSORT_TEST_H
#define DIGIKAM_IMGQSORT_TEST_H

// Qt includes

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QString>

// Local includes

#include "imgqsorttest_shared.h"

class ImgQSortTest : public QObject
{
    Q_OBJECT

public:

    explicit ImgQSortTest(QObject* const parent = nullptr);

private:

    QDir imageDir() const;
    void testParseTestImages(const QString& testcase_name, DetectionType mode);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void testParseTestImagesForExposureDetection();
    void testParseTestImagesForBlurDetection();
    void testParseTestImagesForCompressionDetection();

    void testParseTestImagesForBlurDetection_SharpImage();
    void testParseTestImagesForBlurDetection_MotionBlurImage();
    void testParseTestImagesForBlurDetection_DefocusImage();
    void testParseTestImagesForBlurDetection_BlurBackGroundImage();

    void testParseTestImagesForNoiseDetection();
    void testParseTestImagesForImageHighSO();
    void testParseTestImagesForVariousTypeNoise();
};

// pair name image - quality expected
using PairImageQuality = QPair<QString, int>;
using DataTestCases = QMultiMap<QString, PairImageQuality> ;

DataTestCases const dataTestCases = 
    {   
        {QLatin1String("blurDetection"), PairImageQuality(QLatin1String("test_blurred_1.jpg"),3)},
        {QLatin1String("blurDetection"), PairImageQuality(QLatin1String("test_blurred_2.jpg"),3)},
        {QLatin1String("blurDetection"), PairImageQuality(QLatin1String("test_blurred_5.jpg"),2)},
        {QLatin1String("blurDetection"), PairImageQuality(QLatin1String("test_blurred_9.jpg"),1)},

        {QLatin1String("noiseDetection"), PairImageQuality(QLatin1String("test_noised_1.jpg"),3)},
        {QLatin1String("noiseDetection"), PairImageQuality(QLatin1String("test_noised_2.jpg"),3)},
        {QLatin1String("noiseDetection"), PairImageQuality(QLatin1String("test_noised_5.jpg"),2)},
        {QLatin1String("noiseDetection"), PairImageQuality(QLatin1String("test_noised_9.jpg"),1)},

        {QLatin1String("exposureDetection"), PairImageQuality(QLatin1String("test_overexposed_1.jpg"),3)},
        {QLatin1String("exposureDetection"), PairImageQuality(QLatin1String("test_overexposed_9.jpg"),1)},
        {QLatin1String("exposureDetection"), PairImageQuality(QLatin1String("test_underexposed_1.jpg"),3)},
        {QLatin1String("exposureDetection"), PairImageQuality(QLatin1String("test_underexposed_9.jpg"),1)},

        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_1.jpg"),3)},
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_2.jpg"),3)},
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_5.jpg"),2)},
        {QLatin1String("compressionDetection"), PairImageQuality(QLatin1String("test_compressed_9.jpg"),1)},

        {QLatin1String("sharpImage"), PairImageQuality(QLatin1String("blur_sky_1.jpg"),3)},
        {QLatin1String("sharpImage"), PairImageQuality(QLatin1String("blur_rock_1.jpg"),3)},
        {QLatin1String("sharpImage"), PairImageQuality(QLatin1String("blur_caffe_1.jpg"),3)},
        {QLatin1String("sharpImage"), PairImageQuality(QLatin1String("blur_street_1.jpg"),3)},

        {QLatin1String("motionBlurImage"), PairImageQuality(QLatin1String("blur_sky_1.jpg"),1)},
        {QLatin1String("motionBlurImage"), PairImageQuality(QLatin1String("blur_rock_2.jpg"),1)},
        {QLatin1String("motionBlurImage"), PairImageQuality(QLatin1String("blur_caffe_2.jpg"),1)},
        {QLatin1String("motionBlurImage"), PairImageQuality(QLatin1String("blur_street_2.jpg"),1)},

        {QLatin1String("defocusImage"), PairImageQuality(QLatin1String("blur_sky_3.jpg"),1)},
        {QLatin1String("defocusImage"), PairImageQuality(QLatin1String("blur_rock_3.jpg"),1)},
        {QLatin1String("defocusImage"), PairImageQuality(QLatin1String("blur_caffe_3.jpg"),1)},
        {QLatin1String("defocusImage"), PairImageQuality(QLatin1String("blur_street_3.jpg"),1)},

        {QLatin1String("blurBackGroundImage"), PairImageQuality(QLatin1String("blur_blurbackground_1.jpg"),2)},
        {QLatin1String("blurBackGroundImage"), PairImageQuality(QLatin1String("blur_blurbackground_2.jpg"),3)},
        {QLatin1String("blurBackGroundImage"), PairImageQuality(QLatin1String("blur_blurbackground_3.jpg"),3)},

        {QLatin1String("highISO"), PairImageQuality(QLatin1String("noise_book_1.jpg"),3)},
        {QLatin1String("highISO"), PairImageQuality(QLatin1String("noise_book_2.jpg"),1)},
        {QLatin1String("highISO"), PairImageQuality(QLatin1String("noise_graffi_1.jpg"),3)},
        {QLatin1String("highISO"), PairImageQuality(QLatin1String("noise_graffi_2.jpg"),1)},

        {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_nor.png"),3)},
        {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_gaussian.png"),1)},
        {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_rayleigh.png"),1)},
        {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_speckle.png"),1)},
        // {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_salt_pepper.png"),1)}, False case
        // {QLatin1String("variousTypesNoise"), PairImageQuality(QLatin1String("noise_bird_bandpass.png"),1)}, Faslse case
    };

#endif // DIGIKAM_IMGQSORT_TEST_H
