/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-06
 * Description : test for the filesaveoptionsbox
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_FILE_SAVE_OPTIONS_BOX_UTEST_H
#define DIGIKAM_FILE_SAVE_OPTIONS_BOX_UTEST_H

// Qt includes

#include <QObject>

class FileSaveOptionsBoxTest : public QObject
{
    Q_OBJECT

public:

    explicit FileSaveOptionsBoxTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void testDiscoverFormat_data();
    void testDiscoverFormat();

    void testDiscoverFormatDefault();
};

#endif // DIGIKAM_FILE_SAVE_OPTIONS_BOX_UTEST_H
