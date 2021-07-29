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

#include "detectexposure_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QDebug>

// Local includes

#include "imgqsorttest.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetecteExposure)

ImgQSortTestDetecteExposure::ImgQSortTestDetecteExposure(QObject* const parent)
  : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("exposureDetection"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("exposureDetection"), DETECTEXPOSURE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection_backlight()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("exposureBacklight"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("exposureBacklight"), DETECTEXPOSURE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
} 

void ImgQSortTestDetecteExposure::testParseTestImagesForExposureDetection_sun()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("exposureSun"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("exposureSun"), DETECTEXPOSURE);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
} 
