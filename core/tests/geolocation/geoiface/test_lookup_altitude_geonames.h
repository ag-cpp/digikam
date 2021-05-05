/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-07
 * Description : Test for the geonames based altitude lookup class
 *
 * Copyright (C) 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TEST_LOOKUP_ALTITUDE_GEONAMES_H
#define DIGIKAM_TEST_LOOKUP_ALTITUDE_GEONAMES_H

// Qt includes

#include <QTest>

class TestLookupAltitudeGeonames : public QObject
{
    Q_OBJECT

public:

    explicit TestLookupAltitudeGeonames(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testSimpleLookup();
};

#endif // DIGIKAM_TEST_LOOKUP_ALTITUDE_GEONAMES_H
