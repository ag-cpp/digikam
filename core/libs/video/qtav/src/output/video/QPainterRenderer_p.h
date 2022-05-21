/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_QPAINTER_RENDERER_P_H
#define QTAV_QPAINTER_RENDERER_P_H

#include "QPainterRenderer.h"

// Qt includes

#include <QImage>
#include <QPainter>

// Local includes

#include "VideoRenderer_p.h"

namespace QtAV
{

class QTAV_PRIVATE_EXPORT QPainterRendererPrivate : public VideoRendererPrivate
{
public:

    QPainterRendererPrivate()
        : painter(nullptr)
    {
    }

    virtual ~QPainterRendererPrivate()
    {
        if (painter)
        {
            delete painter;

            painter = nullptr;
        }
    }

    void setupQuality() override
    {
        switch (quality)
        {
            case QualityFastest:
            {
                painter->setRenderHint(QPainter::Antialiasing,          false);
                painter->setRenderHint(QPainter::TextAntialiasing,      false);
                painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
                painter->setRenderHint(QPainter::Antialiasing,          false);
                break;
            }

            case QualityBest:
            default:
            {
                painter->setRenderHint(QPainter::Antialiasing,          true);
                painter->setRenderHint(QPainter::TextAntialiasing,      true);
                painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
                painter->setRenderHint(QPainter::Antialiasing,          true);
                break;
            }
        }
    }

    // drawPixmap() is faster for on screen painting

public:

    QPixmap   pixmap;
    QPainter* painter = nullptr;
};

} // namespace QtAV

#endif // QTAV_QPAINTER_RENDERER_P_H
