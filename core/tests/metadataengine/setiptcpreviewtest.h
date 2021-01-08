/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to set IPTC Preview
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

#ifndef DIGIKAM_SET_IPTC_PREVIEW_TEST_H
#define DIGIKAM_SET_IPTC_PREVIEW_TEST_H

// Local includes

#include "abstractunittest.h"

class SetIptcPreviewTest : public AbstractUnitTest
{
    Q_OBJECT

public:

    explicit SetIptcPreviewTest(QObject* const parent = nullptr);

private:

    void setIptcPreview(const QString& file);

private Q_SLOTS:

    void testSetIptcPreview();
};

#endif // DIGIKAM_SET_IPTC_PREVIEW_TEST_H
