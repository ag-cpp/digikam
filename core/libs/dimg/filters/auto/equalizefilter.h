/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-24-01
 * Description : equalize image filter.
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

#ifndef DIGIKAM_EQUALIZE_FILTER_H
#define DIGIKAM_EQUALIZE_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class DImg;

class DIGIKAM_EXPORT EqualizeFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit EqualizeFilter(QObject* const parent = nullptr);
    EqualizeFilter(DImg* const orgImage, const DImg* const refImage, QObject* const parent = nullptr);
    ~EqualizeFilter()                                                         override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:EqualizeFilter");
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
    void equalizeImage();

private:

    struct Q_DECL_HIDDEN double_packet
    {
        double_packet()
            : red  (0.0),
              green(0.0),
              blue (0.0),
              alpha(0.0)
        {
        }

        double red;
        double green;
        double blue;
        double alpha;
    };

    struct Q_DECL_HIDDEN int_packet
    {
       int_packet()
            : red  (0),
              green(0),
              blue (0),
              alpha(0)
        {
        }

        unsigned int red;
        unsigned int green;
        unsigned int blue;
        unsigned int alpha;
    };

    DImg m_refImage;
};

} // namespace Digikam

#endif // DIGIKAM_EQUALIZE_FILTER_H
