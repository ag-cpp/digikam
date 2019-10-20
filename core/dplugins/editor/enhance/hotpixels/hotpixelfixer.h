/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : Threaded image filter to fix hot pixels
 *
 * Copyright (C) 2005-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef DIGIKAM_EDITOR_HOT_PIXEL_FIXER_H
#define DIGIKAM_EDITOR_HOT_PIXEL_FIXER_H

// Qt includes

#include <QList>
#include <QImage>
#include <QObject>
#include <QRect>
#include <QString>

// Local includes

#include "dimgthreadedfilter.h"
#include "hotpixel.h"
#include "hotpixelsweights.h"

using namespace Digikam;

namespace DigikamEditorHotPixelsToolPlugin
{

class HotPixelFixer : public DImgThreadedFilter
{

public:

    enum InterpolationMethod
    {
        AVERAGE_INTERPOLATION   = 0,
        LINEAR_INTERPOLATION    = 1,
        QUADRATIC_INTERPOLATION = 2,
        CUBIC_INTERPOLATION     = 3
    };

    enum Direction
    {
        TWODIM_DIRECTION     = 0,
        VERTICAL_DIRECTION   = 1,
        HORIZONTAL_DIRECTION = 2
    };

public:

    explicit HotPixelFixer(QObject* const parent = nullptr);
    explicit HotPixelFixer(DImg* const orgImage, QObject* const parent,
                           const QList<HotPixel>& hpList, int interpolationMethod);
    ~HotPixelFixer();

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:HotPixelFilter");
    }

    static QString          DisplayableName()
    {
        return QString::fromUtf8(I18N_NOOP("Hot Pixels Tool"));
    }

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    void readParameters(const FilterAction& action) override;

    virtual QString         filterIdentifier() const override
    {
        return FilterIdentifier();
    }

    virtual Digikam::FilterAction filterAction() override;

private:

    virtual void filterImage() override;

    void interpolate(DImg& img,HotPixel& hp, int method);
    void weightPixels(DImg& img, HotPixel& px, int method,
                      Direction dir, int maxComponent);

    inline bool validPoint(DImg& img, const QPoint& p)
    {
        return (p.x() >= 0 && p.y() >= 0 && p.x() < (long) img.width() && p.y() < (long) img.height());
    };

    QList <HotPixelsWeights> mWeightList;

private:

    int             m_interpolationMethod;

    QList<HotPixel> m_hpList;
};

} // namespace DigikamEditorHotPixelsToolPlugin

#endif // DIGIKAM_EDITOR_HOT_PIXEL_FIXER_H
