/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-23
 * Description : a test for the StateSavingObject class
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef DIGIKAM_STATE_SAVING_OBJECT_TEST_H
#define DIGIKAM_STATE_SAVING_OBJECT_TEST_H

// Qt includes

#include <QTest>

// Local includes

#include "statesavingobject.h"

class KConfigGroup;

class StateSavingObjectTest: public QObject
{
    Q_OBJECT

public:

    explicit StateSavingObjectTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void testGroupName();
    void testPrefixUsage();
    void testDirectCalling();
    void testDirectChildrenLoading();
    void testDirectChildrenSaving();
    void testRecursiveChildrenLoading();
    void testRecursiveChildrenSaving();
};

// ----------------------------------------------------------

class StubStateSaver: public QObject,
                      public Digikam::StateSavingObject
{
    Q_OBJECT

public:

    explicit StubStateSaver(QObject* const parent = nullptr);
    ~StubStateSaver() override;

    KConfigGroup getGroup();
    QString getEntryKey(const QString& base);

    void doLoadState() override;
    void doSaveState() override;

    bool loadCalled();
    bool saveCalled();

    unsigned int numLoadCalls();
    unsigned int numSaveCalls();

private:

    class Private;
    Private* const d;
};

#endif // DIGIKAM_STATE_SAVING_OBJECT_TEST_H
