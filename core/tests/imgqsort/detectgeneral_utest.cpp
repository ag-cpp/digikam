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

#include "detectgeneral_utest.h"

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

using namespace Digikam;

QTEST_MAIN(ImgQSortTestGeneral)

ImgQSortTestGeneral::ImgQSortTestGeneral(QObject* const)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralBadImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("badImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("badImage"), DETECTIONGENERAL);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteBadImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("quiteBadImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("quiteBadImage"), DETECTIONGENERAL);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralNormalImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("normalImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("normalImage"), DETECTIONGENERAL);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteGoodImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("quiteGoodImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("quiteGoodImage"), DETECTIONGENERAL);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralGoodImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("goodImage"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("goodImage"), DETECTIONGENERAL);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::testParseTestImagesForSunImage()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("badImage"));

    CustomDetection* customSettings = new CustomDetection{true,true,false,true};

    QHash<QString, int> results = testParseTestImages(QLatin1String("badImage"), customSettings);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }

    delete customSettings;
}
