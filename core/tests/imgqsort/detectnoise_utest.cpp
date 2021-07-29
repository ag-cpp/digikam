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

#include "detectnoise_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QFileInfoList>
#include <QDebug>
#include <QDir>

// Local includes

#include "digikam_debug.h"
#include "imgqsorttest.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetectNoise)

ImgQSortTestDetectNoise::ImgQSortTestDetectNoise(QObject* const parent)
  : ImgQSortTest(parent)
{
}

void ImgQSortTestDetectNoise::testParseTestImagesForNoiseDetection()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("noiseDetection"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("noiseDetection"), DETECTNOISE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForImageHighSO()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("highISO"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("highISO"), DETECTNOISE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoise()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("variousTypesNoise"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("variousTypesNoise"), DETECTNOISE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoiseFailCase()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("variousTypesNoiseFailCase"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("variousTypesNoiseFailCase"), DETECTNOISE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}
