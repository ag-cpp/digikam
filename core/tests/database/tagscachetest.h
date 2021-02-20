/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-20
 * Description : Unit tests for TagsCache class
 *
 * Copyright (C) 2021 by David Haslam, <dch dot code at gmail dot com>
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

#ifndef TAGSCACHETEST_H
#define TAGSCACHETEST_H

#include <QObject>
#include <QDebug>
#include <QtTest>

#include "tagscache.h"

/**
 * Unit tests for TagsCache class in core/libs/database/tags/tagscache.h
 *
 * TagsCache provides cached access to the hierarchical tag data in the core database.
 * These tests use the public interface to create sample tag hierarchies and then
 * check the access and relationship to the nodes.
 *
 * Uses a temporary in-memory sqlite database, and does not require a GUI.
 */

class TagsCacheTest : public QObject
{
    Q_OBJECT

public:

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testSimpleHierarchy();
    void testComplexHierarchy();
    void testRepeatedNames();
    void testDuplicateTop();

private:

    Digikam::TagsCache* tags_cache;

    // utilities
    int countTags();

    // debug tests
    void dumpTables();
    void dumpTags();

};

#endif /* TAGSCACHETEST_H */
