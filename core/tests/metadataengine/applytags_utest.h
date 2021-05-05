/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-10-30
 * Description : An unit-test to read metadata and apply tag paths to item with DMetadata.
 *
 * Copyright (C) 2019-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_APPLY_TAGS_UTEST_H
#define DIGIKAM_APPLY_TAGS_UTEST_H

// Local includes

#include "abstractunittest.h"
#include "metaenginesettingscontainer.h"
#include "dmetadatasettingscontainer.h"

using namespace Digikam;

class ApplyTagsTest : public AbstractUnitTest
{
    Q_OBJECT

public:

    explicit ApplyTagsTest(QObject* const parent = nullptr);

private:

    void applyTags(const QString& file,
                   const QStringList& tags,
                   const MetaEngineSettingsContainer& settings,
                   bool  expectedRead,
                   bool  expectedWrite);

private Q_SLOTS:

    void testApplyTagsToMetadata();
};

#endif // DIGIKAM_APPLY_TAGS_UTEST_H
