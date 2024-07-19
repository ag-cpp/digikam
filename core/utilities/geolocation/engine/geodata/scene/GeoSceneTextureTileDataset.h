/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team <marble dot kde dot org>
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "GeoSceneTileDataset.h"

namespace Marble
{

class DIGIKAM_EXPORT GeoSceneTextureTileDataset : public GeoSceneTileDataset
{
public:

    explicit GeoSceneTextureTileDataset(const QString& name);

    const char* nodeType() const override;
};

} // namespace Marble
