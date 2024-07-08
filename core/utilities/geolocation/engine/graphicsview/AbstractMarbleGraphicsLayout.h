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

#include "digikam_export.h"

namespace Marble
{

class MarbleGraphicsItem;

class DIGIKAM_EXPORT AbstractMarbleGraphicsLayout
{
public:

    AbstractMarbleGraphicsLayout();
    virtual ~AbstractMarbleGraphicsLayout();

    /**
     * This updates the positions of all items in the layout.
     * Calling this will be done on every repainting, so you won't have to do it yourself.
     */
    virtual void updatePositions(MarbleGraphicsItem* parent) = 0;
};

} // Namespace Marble
