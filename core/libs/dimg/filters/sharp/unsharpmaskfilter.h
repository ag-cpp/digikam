/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-17-07
 * Description : A Unsharp Mask threaded image filter.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Matthias Welwarsky <matze at welwarsky dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_UNSHARP_MASK_FILTER_H
#define DIGIKAM_UNSHARP_MASK_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DIGIKAM_EXPORT UnsharpMaskFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit UnsharpMaskFilter(QObject* const parent = nullptr);
    explicit UnsharpMaskFilter(DImg* const orgImage,
                               QObject* const parent = nullptr,
                               double radius = 1.0,
                               double amount = 1.0,
                               double threshold = 0.05,
                               bool luma=false);

    ~UnsharpMaskFilter()                                                      override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:UnsharpMaskFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    QString         filterIdentifier()                                  const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                            override;
    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;
    void unsharpMaskMultithreaded(uint start, uint stop, uint y);

private:

    double m_radius;
    double m_amount;
    double m_threshold;
    bool   m_luma;
};

} // namespace Digikam

#endif // DIGIKAM_UNSHARP_MASK_FILTER_H
