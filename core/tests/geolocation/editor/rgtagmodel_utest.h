/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-21
 * Description : Test for RG tag model.
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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

#ifndef DIGIKAM_RGTAGMODEL_UTEST_H
#define DIGIKAM_RGTAGMODEL_UTEST_H

// Qt includes

#include <QTest>

class TestRGTagModel : public QObject
{
    Q_OBJECT

public:

    explicit TestRGTagModel(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void testNoOp();
    void testModelEmpty();

    void testModel1();
    void testModel2();
    void testModel3();
    void testModelSpacerTags();
};

#endif // DIGIKAM_RGTAGMODEL_UTEST_H
