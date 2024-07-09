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

#include "LayerInterface.h"

// Local includes

#include "RenderState.h"

namespace Marble
{

LayerInterface::~LayerInterface()
{
    // nothing to do
}

qreal LayerInterface::zValue() const
{
    return 0.0;
}

RenderState LayerInterface::renderState() const
{
    return RenderState();
}

QString LayerInterface::runtimeTrace() const
{
    return QString();
}

} // namespace Marble
