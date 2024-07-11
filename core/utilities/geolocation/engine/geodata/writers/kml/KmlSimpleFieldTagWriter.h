/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "GeoTagWriter.h"
#include "GeoDataSimpleField.h"

class QString;

namespace Marble
{

class KmlSimpleFieldTagWriter : public GeoTagWriter
{
public:

    bool write(const GeoNode* node, GeoWriter& writer) const override;

private:

    static QString resolveType(GeoDataSimpleField::SimpleFieldType type);
};

} // namespace Marble
