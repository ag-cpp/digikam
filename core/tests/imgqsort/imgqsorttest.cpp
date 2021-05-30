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

#include "imgqsorttest.h"

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

QTEST_MAIN(ImgQSortTest)


ImgQSortTest::ImgQSortTest(QObject* const)
{
}

void ImgQSortTest::testParseTestImages(const QString& testcase_name, DetectionType mode)
{
    QStringList imageNames;
    QList<PairImageQuality> dataTest = dataTestCases.values(testcase_name);
    
    for (auto i = dataTest.begin(); i != dataTest.end(); i++ )
    {
        qInfo()<<"image name "<<(*i).first;
        imageNames << (*i).first;
    }

    QFileInfoList list = imageDir().entryInfoList(imageNames,QDir::Files, QDir::Name);

    QMultiMap<QString, int> results = ImgQSortTest_ParseTestImages(mode, list);

    for (auto i = dataTest.begin(); i != dataTest.end(); i++ )
    {
        QVERIFY(results.values((*i).first).first() == (*i).second);
    }
}

void ImgQSortTest::initTestCase()
{
    QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void ImgQSortTest::cleanupTestCase()
{
}

QDir ImgQSortTest::imageDir() const
{
    QDir dir(QFINDTESTDATA("data/"));
    qDebug() << "Images Directory:" << dir;
    return dir;
}

void ImgQSortTest::testParseTestImagesForExposureDetection()
{
    testParseTestImages(QLatin1String("exposureDetection"), DetectExposure);
}

void ImgQSortTest::testParseTestImagesForNoiseDetection()
{
    testParseTestImages(QLatin1String("noiseDetection"), DetectNoise);
}

void ImgQSortTest::testParseTestImagesForBlurDetection()
{
    testParseTestImages(QLatin1String("blurDetection"), DetectBlur);
}

void ImgQSortTest::testParseTestImagesForCompressionDetection()
{
    testParseTestImages(QLatin1String("compressionDetection"), DetectCompression);
}

void ImgQSortTest::testParseTestImagesForBlurDetection_SharpImage()
{
    testParseTestImages(QLatin1String("sharpImage"), DetectBlur);
}

void ImgQSortTest::testParseTestImagesForBlurDetection_MotionBlurImage()
{
    testParseTestImages(QLatin1String("motionBlurImage"), DetectBlur);
}

void ImgQSortTest::testParseTestImagesForBlurDetection_DefocusImage()
{
    testParseTestImages(QLatin1String("defocusImage"), DetectBlur);
}

void ImgQSortTest::testParseTestImagesForBlurDetection_BlurBackGroundImage()
{
    testParseTestImages(QLatin1String("blurBackGroundImage"), DetectBlur);
}