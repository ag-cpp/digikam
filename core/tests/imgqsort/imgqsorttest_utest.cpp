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

#include "imgqsorttest_utest.h"

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
}

void ImgQSortTestGeneral::testParseTestImages(const QString& testcase_name, DetectionType mode)
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
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestGeneral::initTestCase()
{
    QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void ImgQSortTestGeneral::cleanupTestCase()
{
}

QDir ImgQSortTestGeneral::imageDir() const
{
    QDir dir(QFINDTESTDATA("data/"));
    qDebug(DIGIKAM_TESTS_LOG) << "Images Directory:" << dir;
    return dir;
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralBadImage()
{
    testParseTestImages(QLatin1String("badImage"), DETECTIONGENERAL);
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteBadImage()
{
    testParseTestImages(QLatin1String("quiteBadImage"), DETECTIONGENERAL);
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralNormalImage()
{
    testParseTestImages(QLatin1String("normalImage"), DETECTIONGENERAL);
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralQuiteGoodImage()
{
    testParseTestImages(QLatin1String("quiteGoodImage"), DETECTIONGENERAL);
}

void ImgQSortTestGeneral::testParseTestImagesForGeneralGoodImage()
{
    testParseTestImages(QLatin1String("goodImage"), DETECTIONGENERAL);
}
