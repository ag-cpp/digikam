/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-11
 * Description : An unit-test to print metadata tags from file using DMetadata.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PRINT_METADATA_UTEST_H
#define DIGIKAM_PRINT_METADATA_UTEST_H

// Local includes

#include "abstractunittest.h"
#include "dmetadata.h"

using namespace Digikam;

class PrintMetadataTest : public AbstractUnitTest
{
    Q_OBJECT

public:

    explicit PrintMetadataTest(QObject* const parent = nullptr);

private:

    void printMetadataMap(const DMetadata::MetaDataMap& map);
    void printMetadata(const QString& filePath, bool exif, bool iptc, bool xmp, bool expectedRead);

    /// NOTE: 'expected' paramareters want mean that we expect a metadata tags map non empty
    void loadExif(const DMetadata& meta, bool expected);
    void loadIptc(const DMetadata& meta, bool expected);
    void loadXmp(const DMetadata& meta, bool expected);

private Q_SLOTS:

    void testPrintMetadata();
};

#endif // DIGIKAM_PRINT_METADATA_UTEST_H
