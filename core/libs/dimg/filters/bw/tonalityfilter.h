/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : Change tonality image filter
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_TONALITY_FILTER_H
#define DIGIKAM_TONALITY_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT TonalityContainer
{

public:

    explicit TonalityContainer()
      : redMask  (0),
        greenMask(0),
        blueMask (0)
    {
    };

    ~TonalityContainer()
    {
    };

public:

    int redMask;
    int greenMask;
    int blueMask;
};

// -----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT TonalityFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit TonalityFilter(QObject* const parent = nullptr);
    explicit TonalityFilter(DImg* const orgImage,
                            QObject* const parent = nullptr,
                            const TonalityContainer& settings = TonalityContainer());
    ~TonalityFilter()                                       override;

    static QString FilterIdentifier()
    {
        return QLatin1String("digikam:TonalityFilter");
    }

    static QString DisplayableName();

    static QList<int> SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int CurrentVersion()
    {
        return 1;
    }

    QString filterIdentifier()                        const override
    {
        return FilterIdentifier();
    }

    FilterAction filterAction()                             override;
    void readParameters(const FilterAction& action)         override;

private:

    void filterImage()                                      override;

private:

    TonalityContainer m_settings;
};

} // namespace Digikam

#endif // DIGIKAM_TONALITY_FILTER_H
