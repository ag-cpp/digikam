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

#include "detectblur_utest.h"

// Qt includes

#include <QTest>
#include <QStringList>
#include <QFileInfoList>
#include <QDebug>
#include <QDir>

// Local includes

#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "dpluginloader.h"
#include "digikam_debug.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetectBlur)

ImgQSortTestDetectBlur::ImgQSortTestDetectBlur(QObject* const parent)
  : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("blurDetection"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("blurDetection"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_SharpImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("sharpImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("sharpImage"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_MotionBlurImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("motionBlurImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("motionBlurImage"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_DefocusImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("defocusImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("defocusImage"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_BlurBackGroundImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("blurBackGroundImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("blurBackGroundImage"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectBlur::testParseTestImagesForBlurDetection_FailCase()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("blurDetectionFailTest"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("blurDetectionFailTest"), DETECTBLUR);

    for (const auto& image_refQuality : dataTest)
    {
        QEXPECT_FAIL("", "Will fix in the next release", Continue);

        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}
