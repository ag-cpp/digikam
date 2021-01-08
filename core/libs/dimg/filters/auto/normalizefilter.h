/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : normalize image filter.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_NORMALIZE_FILTER_H
#define DIGIKAM_NORMALIZE_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT NormalizeFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit NormalizeFilter(QObject* const parent = nullptr);
    NormalizeFilter(DImg* const orgImage, const DImg* const refImage, QObject* const parent=nullptr);
    ~NormalizeFilter() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:NormalizeFilter");
    }

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    static QString DisplayableName();

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;
    void normalizeImage();

private:

    struct Q_DECL_HIDDEN NormalizeParam
    {
        unsigned short* lut;
        double          min;
        double          max;
    };

    DImg m_refImage;
};

} // namespace Digikam

#endif // DIGIKAM_NORMALIZE_FILTER_H
