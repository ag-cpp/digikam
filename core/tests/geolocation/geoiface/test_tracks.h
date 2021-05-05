/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-17
 * Description : Test parsing gpx data.
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TEST_TRACKS_H
#define DIGIKAM_TEST_TRACKS_H

// Qt includes

#include <QTest>

// local includes

class TestTracks : public QObject
{
    Q_OBJECT

public:

    explicit TestTracks(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testQDateTimeParsing();
    void testCustomDateTimeParsing();
    void testSaxLoader();
    void testSaxLoaderError();
    void testFileLoading();
};

#endif // DIGIKAM_TEST_TRACKS_H
