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

// Qt includes

#include <QAtomicInt>

// Local includes

#include "GeoDataCoordinates.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataLookAtPrivate
{
public:

    GeoDataLookAtPrivate()
        : m_coordinates(),
          m_range(0.0),
          ref(1)
    {
    }

    GeoDataCoordinates m_coordinates;
    qreal              m_range;

    QAtomicInt         ref;
};

} // namespace Marble
