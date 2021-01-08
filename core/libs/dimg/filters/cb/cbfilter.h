/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-18
 * Description : color balance filter
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CB_FILTER_H
#define DIGIKAM_CB_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT CBContainer
{

public:

    explicit CBContainer()
      : red  (1.0),
        green(1.0),
        blue (1.0),
        alpha(1.0),
        gamma(1.0)
    {
    };

    ~CBContainer()
    {
    };

public:

    double red;
    double green;
    double blue;
    double alpha;
    double gamma;
};

// -----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT CBFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit CBFilter(QObject* const parent = nullptr);
    explicit CBFilter(DImg* const orgImage,
                      QObject* const parent = nullptr,
                      const CBContainer& settings = CBContainer());
    explicit CBFilter(const CBContainer& settings,
                      DImgThreadedFilter* const master,
                      const DImg& orgImage,
                      DImg& destImage,
                      int progressBegin = 0,
                      int progressEnd = 100);
    ~CBFilter()                                                       override;

    static QString FilterIdentifier()
    {
        return QLatin1String("digikam:ColorBalanceFilter");
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

    void  readParameters(const FilterAction& action)                  override;

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;

private:

    void filterImage()                                                override;

    void reset();
    void setGamma(double val);
    void setTables(int* const redMap, int* const greenMap, int* const blueMap, int* const alphaMap, bool sixteenBit);
    void getTables(int* const redMap, int* const greenMap, int* const blueMap, int* const alphaMap, bool sixteenBit);
    void adjustRGB(double r, double g, double b, double a, bool sixteenBit);
    void applyCBFilter(DImg& image, double r, double g, double b, double a);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_CB_FILTER_H
