/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-07
 * Description : test for the simple datatypes and helper functions
 *
 * Copyright (C) 2010-2013 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_PRIMITIVES_UTEST_H
#define DIGIKAM_PRIMITIVES_UTEST_H

// Qt includes

#include <QTest>

class TestPrimitives : public QObject
{
    Q_OBJECT

public:

    explicit TestPrimitives(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testParseLatLonString();
    void testParseXYStringToPoint();
    void testParseBoundsString();
    void testNormalizeBounds_data();
    void testNormalizeBounds();
    void testGroupStateComputer();
};

#endif // DIGIKAM_PRIMITIVES_UTEST_H
