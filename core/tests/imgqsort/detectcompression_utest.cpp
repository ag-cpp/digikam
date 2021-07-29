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

#include "detectcompression_utest.h"

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

QTEST_MAIN(ImgQSortTestDetectCompression)

ImgQSortTestDetectCompression::ImgQSortTestDetectCompression(QObject* const parent)
  : ImgQSortTest(parent)
{
    m_dataTestCases = dataTestCases;
}

void ImgQSortTestDetectCompression::testParseTestImagesForCompressionDetection()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("compressionDetection"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("compressionDetection"), DETECTCOMPRESSION);

    for (const auto& image_refQuality : dataTest)
    {
        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}

void ImgQSortTestDetectCompression::testParseTestImagesForCompressionDetection_failCase()
{
    QList<PairImageQuality> dataTest = getDataTestCases().values(QLatin1String("compressionDetection_failCase"));

    QHash<QString, int> results = testParseTestImages(QLatin1String("compressionDetection_failCase"), DETECTCOMPRESSION);

    for (const auto& image_refQuality : dataTest)
    {
        QEXPECT_FAIL("", "Will fix in the next release", Continue);

        QVERIFY(results.value(image_refQuality.first) == image_refQuality.second);
    }
}