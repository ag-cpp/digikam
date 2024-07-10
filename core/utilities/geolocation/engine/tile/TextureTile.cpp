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

#include "TextureTile.h"

// C++ includes

#include <limits>

// Local includes

#include "digikam_debug.h"

namespace Marble
{

TextureTile::TextureTile(TileId const& tileId, QImage const& image, const Blending* blending)
    : Tile(tileId),
      m_image(image),
      m_blending(blending)
{
    Q_ASSERT(!image.isNull());
}

TextureTile::~TextureTile()
{
}

} // namespace Marble

