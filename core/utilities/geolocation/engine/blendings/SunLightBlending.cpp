/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "SunLightBlending.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QImage>
#include <QColor>

// Local includes

#include "SunLocator.h"
#include "TextureTile.h"
#include "TileLoaderHelper.h"
#include "MarbleGlobal.h"
#include "digikam_debug.h"

namespace Marble
{

SunLightBlending::SunLightBlending(const SunLocator* sunLocator)
    : Blending(),
      m_sunLocator(sunLocator),
      m_levelZeroColumns(0),
      m_levelZeroRows(0)
{
}

SunLightBlending::~SunLightBlending()
{
}

void SunLightBlending::blend(QImage* const tileImage, TextureTile const* const top) const
{
    if (tileImage->depth() != 32)
    {
        return;
    }

    // TODO add support for 8-bit maps?
    // add sun shading
    const TileId id = top->id();
    const qreal  global_width  = tileImage->width()
                                 * TileLoaderHelper::levelToColumn(m_levelZeroColumns, id.zoomLevel());
    const qreal  global_height = tileImage->height()
                                 * TileLoaderHelper::levelToRow(m_levelZeroRows, id.zoomLevel());
    const qreal lon_scale = 2 * M_PI / global_width;
    const qreal lat_scale = -M_PI / global_height;
    const int tileHeight = tileImage->height();
    const int tileWidth = tileImage->width();

    // First we determine the supporting point interval for the interpolation.
    const int n = maxDivisor(30, tileWidth);
    const int ipRight = n * (int)(tileWidth / n);

    const QImage* nighttile = top->image();

    for (int cur_y = 0; cur_y < tileHeight; ++cur_y)
    {
        const qreal lat = lat_scale * (id.y() * tileHeight + cur_y) - 0.5 * M_PI;
        const qreal a = sin((lat + DEG2RAD * m_sunLocator->getLat()) / 2.0);
        const qreal c = cos(lat) * cos(-DEG2RAD * m_sunLocator->getLat());

        QRgb* scanline        = reinterpret_cast<QRgb*>(tileImage->scanLine(cur_y));
        const QRgb* nscanline = reinterpret_cast<const QRgb*>(nighttile->scanLine(cur_y));

        qreal lastShade = -10.0;

        int cur_x = 0;

        while (cur_x < tileWidth)
        {

            const bool interpolate = (cur_x != 0 && cur_x < ipRight && cur_x + n < tileWidth);

            qreal shade = 0;

            if (interpolate)
            {
                const int check = cur_x + n;
                const qreal checklon   = lon_scale * (id.x() * tileWidth + check);
                shade = m_sunLocator->shading(checklon, a, c);

                // if the shading didn't change across the interpolation
                // interval move on and don't change anything.
                if (shade == lastShade && shade == 1.0)
                {
                    scanline += n;
                    nscanline += n;
                    cur_x += n;
                    continue;
                }

                if (shade == lastShade && shade == 0.0)
                {
                    for (int t = 0; t < n; ++t)
                    {
                        SunLocator::shadePixelComposite(*scanline, *nscanline, shade);
                        ++scanline;
                        ++nscanline;
                    }

                    cur_x += n;
                    continue;
                }

                qreal lon = lon_scale * (id.x() * tileWidth + cur_x);

                for (int t = 0; t < n ; ++t)
                {
                    shade = m_sunLocator->shading(lon, a, c);
                    SunLocator::shadePixelComposite(*scanline, *nscanline, shade);
                    ++scanline;
                    ++nscanline;
                    lon += lon_scale;
                }

                cur_x += n;
            }

            else
            {
                // Make sure we don't exceed the image memory
                if (cur_x < tileWidth)
                {
                    qreal lon   = lon_scale * (id.x() * tileWidth + cur_x);
                    shade = m_sunLocator->shading(lon, a, c);
                    SunLocator::shadePixelComposite(*scanline, *nscanline, shade);
                    ++scanline;
                    ++nscanline;
                    ++cur_x;
                }
            }

            lastShade = shade;
        }
    }
}

void SunLightBlending::setLevelZeroLayout(int levelZeroColumns, int levelZeroRows)
{
    m_levelZeroColumns = levelZeroColumns;
    m_levelZeroRows = levelZeroRows;
}

// TODO: This should likely go into a math class Q_DECL_HIDDEN in the future ...
int SunLightBlending::maxDivisor(int maximum, int fullLength)
{
    // Find the optimal interpolation interval n for the
    // current image canvas width
    int best = 2;

    int nEvalMin = fullLength;

    for (int it = 1; it <= maximum; ++it)
    {
        // The optimum is the interval which results in the least amount
        // supporting points taking into account the rest which can't
        // get used for interpolation.
        int nEval = fullLength / it + fullLength % it;

        if (nEval < nEvalMin)
        {
            nEvalMin = nEval;
            best = it;
        }
    }

    return best;
}

} // namespace Marble
