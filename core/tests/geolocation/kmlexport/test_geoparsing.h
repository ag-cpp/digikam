/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-17
 * Description : test parsing gpx data
 *
 * Copyright (C) 2010      by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TEST_GEOPARSING_H
#define DIGIKAM_TEST_GEOPARSING_H

// Qt includes

#include <QTest>

class TestGPXParsing : public QObject
{
    Q_OBJECT

public:

    explicit TestGPXParsing(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testQDateTimeParsing();
    void testCustomParsing();
};

#endif // DIGIKAM_TEST_GEOPARSING_H
