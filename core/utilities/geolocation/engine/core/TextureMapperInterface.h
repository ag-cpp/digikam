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

class QRect;

namespace Marble
{

class GeoPainter;
class TextureColorizer;
class ViewportParams;

class TextureMapperInterface
{
public:

    TextureMapperInterface();
    virtual ~TextureMapperInterface();

    virtual void mapTexture(GeoPainter* painter,
                            const ViewportParams* viewport,
                            int tileZoomLevel,
                            const QRect& dirtyRect,
                            TextureColorizer* texColorizer) = 0;

    void setRepaintNeeded();

protected:

    bool m_repaintNeeded;
};

} // Namespace Marble
