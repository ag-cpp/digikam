/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-17-07
 * Description : A Gaussian Blur threaded image filter.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_BLUR_FILTER_H
#define DIGIKAM_BLUR_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DIGIKAM_EXPORT BlurFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit BlurFilter(QObject* const parent = nullptr);
    explicit BlurFilter(DImg* const orgImage,
                        QObject* const parent = nullptr,
                        int radius = 3);

    /**
     * Constructor for slave mode: execute immediately in current thread with specified master filter
     */
    explicit BlurFilter(DImgThreadedFilter* const parentFilter,
                        const DImg& orgImage,
                        const DImg& destImage,
                        int progressBegin = 0,
                        int progressEnd = 100,
                        int radius = 3);

    ~BlurFilter()                                                             override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:BlurFilter");
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
    void blurMultithreaded(uint start, uint stop);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BLUR_FILTER_H
