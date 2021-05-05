/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-15-02
 * Description : auto exposure image filter.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_AUTO_EXPO_FILTER_H
#define DIGIKAM_AUTO_EXPO_FILTER_H

// Local includes

#include "digikam_export.h"
#include "wbfilter.h"
#include "digikam_globals.h"
#include "filteraction.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT AutoExpoFilter : public WBFilter
{
    Q_OBJECT

public:

    explicit AutoExpoFilter(QObject* const parent = nullptr);
    AutoExpoFilter(DImg* const orgImage, const DImg* const refImage, QObject* const parent = nullptr);
    ~AutoExpoFilter()                                                         override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:AutoExpoFilter");
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

    QString         filterIdentifier()                                  const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                            override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

private:

    DImg m_refImage;
};

} // namespace Digikam

#endif // DIGIKAM_AUTO_EXPO_FILTER_H
