/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-09
 * Description : a test for the freerotation tool
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_DIMG_FREE_ROTATION_TEST_H
#define DIGIKAM_DIMG_FREE_ROTATION_TEST_H

// Qt includes

#include <QObject>

class DImgFreeRotationTest : public QObject
{
    Q_OBJECT

public:

    explicit DImgFreeRotationTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void testCalculateAngle();
    void testCalculateAngle_data();

private:

    double myRound(double val, int accuracy);
};

#endif // DIGIKAM_DIMG_FREE_ROTATION_TEST_H
