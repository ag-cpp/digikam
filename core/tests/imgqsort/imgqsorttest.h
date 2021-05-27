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

class ImgQSortTest : public QObject
{
    Q_OBJECT

public:

    explicit ImgQSortTest(QObject* const parent = nullptr);

private:

    QDir imageDir() const;
    void testParseTestImages(const QString& testcase_name);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void testParseTestImagesForExposureDetection();
    void testParseTestImagesForNoiseDetection();
    void testParseTestImagesForBlurDetection();
    void testParseTestImagesForCompressionDetection();

    // void testParseTestImagesForBlurDetection_sharpImage();
    // void testParseTestImagesForBlurDetection_motionBlurImage();
    // void testParseTestImagesForBlurDetection_defocusImage();
};

typedef QPair<QString, int> pairImageQuality;
typedef QMultiMap<QString, pairImageQuality> DataTestCases;

static DataTestCases defineTestData() {
    DataTestCases map;
    map.insert(QLatin1String("blurDetection"), pairImageQuality(QLatin1String("test_blurred_1.jpg"),3));
    map.insert(QLatin1String("blurDetection"), pairImageQuality(QLatin1String("test_blurred_2.jpg"),3));
    map.insert(QLatin1String("blurDetection"), pairImageQuality(QLatin1String("test_blurred_5.jpg"),3));
    map.insert(QLatin1String("blurDetection"), pairImageQuality(QLatin1String("test_blurred_9.jpg"),3));

    map.insert(QLatin1String("noiseDetection"), pairImageQuality(QLatin1String("test_noised_1.jpg"),3));
    map.insert(QLatin1String("noiseDetection"), pairImageQuality(QLatin1String("test_noised_2.jpg"),3));
    map.insert(QLatin1String("noiseDetection"), pairImageQuality(QLatin1String("test_noised_5.jpg"),3));
    map.insert(QLatin1String("noiseDetection"), pairImageQuality(QLatin1String("test_noised_9.jpg"),3));

    map.insert(QLatin1String("exposureDetection"), pairImageQuality(QLatin1String("test_overexposed_1.jpg"),3));
    map.insert(QLatin1String("exposureDetection"), pairImageQuality(QLatin1String("test_overexposed_9.jpg"),3));
    map.insert(QLatin1String("exposureDetection"), pairImageQuality(QLatin1String("test_underexposed_5.jpg"),3));
    map.insert(QLatin1String("exposureDetection"), pairImageQuality(QLatin1String("test_underexposed_9.jpg"),3));

    map.insert(QLatin1String("compressionDetection"), pairImageQuality(QLatin1String("test_compressed_1.jpg"),3));
    map.insert(QLatin1String("compressionDetection"), pairImageQuality(QLatin1String("test_compressed_2.jpg"),3));
    map.insert(QLatin1String("compressionDetection"), pairImageQuality(QLatin1String("test_compressed_5.jpg"),3));
    map.insert(QLatin1String("compressionDetection"), pairImageQuality(QLatin1String("test_compressed_9.jpg"),3));
    

    // map.insert(QLatin1String("sharpImage"), pairImageQuality(QLatin1String("blur_sky_1.jpg"),3));
    // map.insert(QLatin1String("sharpImage"), pairImageQuality(QLatin1String("blur_rock_1.jpg"),3));
    // map.insert(QLatin1String("sharpImage"), pairImageQuality(QLatin1String("blur_night_1.jpg"),3));
    // map.insert(QLatin1String("sharpImage"), pairImageQuality(QLatin1String("blur_light_1.jpg"),3));
    
    return map;
}

static DataTestCases const dataTestCases = defineTestData();

#endif // DIGIKAM_IMGQSORT_TEST_H
