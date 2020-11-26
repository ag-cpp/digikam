/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-02
 * Description : Test the functions for dealing with DatabaseFields
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_IO_JOBS_TEST_H
#define DIGIKAM_IO_JOBS_TEST_H

// Qt includes

#include <QObject>

class IOJobsTest : public QObject
{
    Q_OBJECT

public:

    explicit IOJobsTest(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void init();
    void cleanup();

    void copyAndMove();
    void copyAndMove_data();

    void permanentDel();
    void permanentDel_data();
//    void rename();
};

#endif // DIGIKAM_IO_JOBS_TEST_H
