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

#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "dpluginloader.h"

using namespace Digikam;

QTEST_MAIN(ImgQSortTestDetectNoise)

ImgQSortTestDetectNoise::ImgQSortTestDetectNoise(QObject* const)
{
}

void ImgQSortTestDetectNoise::testParseTestImages(const QString& testcase_name, DetectionType mode, bool expectedFail)
{
    QStringList imageNames;
    QList<PairImageQuality> dataTest = dataTestCases.values(testcase_name);
    
    for (const auto& image_refQuality : dataTest)
    {
        imageNames << image_refQuality.first;
    }

    QFileInfoList list = imageDir().entryInfoList(imageNames,QDir::Files, QDir::Name);

    QHash<QString, int> results = ImgQSortTest_ParseTestImages(mode, list);

    for (const auto& image_refQuality : dataTest)
    {
        if (expectedFail)
        {
            QEXPECT_FAIL("", "Will fix in the next release", Continue);
        }
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectNoise::initTestCase()
{
    QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void ImgQSortTestDetectNoise::cleanupTestCase()
{
}

QDir ImgQSortTestDetectNoise::imageDir() const
{
    QDir dir(QFINDTESTDATA("data/"));
    qDebug() << "Images Directory:" << dir;
    return dir;
}

void ImgQSortTestDetectNoise::testParseTestImagesForNoiseDetection()
{
    testParseTestImages(QLatin1String("noiseDetection"), DETECTNOISE);
}

void ImgQSortTestDetectNoise::testParseTestImagesForImageHighSO()
{
    testParseTestImages(QLatin1String("highISO"), DETECTNOISE);
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoise()
{
    testParseTestImages(QLatin1String("variousTypesNoise"), DETECTNOISE);
}

void ImgQSortTestDetectNoise::testParseTestImagesForVariousTypeNoiseFailCase()
{
    testParseTestImages(QLatin1String("variousTypesNoiseFailCase"), DETECTNOISE, true);
}
