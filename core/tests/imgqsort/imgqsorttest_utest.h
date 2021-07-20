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

#ifndef DIGIKAM_IMGQSORT_TEST_GENERAL_H
#define DIGIKAM_IMGQSORT_TEST_GENERAL_H

// Qt includes

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QString>

// Local includes

#include "imgqsorttest_shared.h"
#include "digikam_debug.h"

class ImgQSortTestGeneral : public QObject
{
    Q_OBJECT

public:

    explicit ImgQSortTestGeneral(QObject* const parent = nullptr);

private:

    QDir imageDir() const;
    void testParseTestImages(const QString& testcase_name, DetectionType mode);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void testParseTestImagesForGeneralBadImage();
    void testParseTestImagesForGeneralQuiteBadImage();
    void testParseTestImagesForGeneralNormalImage();
    void testParseTestImagesForGeneralQuiteGoodImage();
    void testParseTestImagesForGeneralGoodImage();

};

// pair name image - quality expected
using PairImageQuality = QPair<QString, int>;
using DataTestCases = QMultiMap<QString, PairImageQuality> ;

DataTestCases const dataTestCases = 
    {   
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_1.jpg"),3)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_2.jpg"),3)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_3.jpg"),2)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_4.jpg"),1)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_5.jpg"),1)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_6.jpg"),1)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_7.jpg"),1)},
        {QLatin1String("badImage"), PairImageQuality(QLatin1String("general_bad_image_8.jpg"),1)},

        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_1.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_2.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_3.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_4.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_5.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_6.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_7.jpg"),2)},
        {QLatin1String("quiteBadImage"), PairImageQuality(QLatin1String("general_quite_bad_image_8.jpg"),2)},

        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_1.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_2.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_3.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_4.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_5.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_6.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_7.jpg"),2)},
        {QLatin1String("normalImage"), PairImageQuality(QLatin1String("general_normal_image_8.jpg"),2)},

        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},
        {QLatin1String("quiteGoodImage"), PairImageQuality(QLatin1String("general_quite_good_image_1.jpg"),2)},

        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
        {QLatin1String("goodImage"), PairImageQuality(QLatin1String("general_good_image_1.jpg"),3)},
    };

#endif // DIGIKAM_IMGQSORT_TEST_GENERAL_H
