/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Raindrop threaded image filter.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_RAIN_DROP_FILTER_H
#define DIGIKAM_RAIN_DROP_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"
#include "randomnumbergenerator.h"

class QRect;

namespace Digikam
{

class DIGIKAM_EXPORT RainDropFilter : public DImgThreadedFilter
{
    Q_OBJECT

private:

    struct Q_DECL_HIDDEN Args
    {
        explicit Args()
            : start(0),
              stop(0),
              orgImage(nullptr),
              destImage(nullptr),
              MinDropSize(0),
              MaxDropSize(0),
              Coeff(0),
              bLimitRange(false),
              pStatusBits(nullptr)
        {
        }

        uint   start;
        uint   stop;
        DImg*  orgImage;
        DImg*  destImage;
        int    MinDropSize;
        int    MaxDropSize;
        int    Coeff;
        bool   bLimitRange;
        uchar* pStatusBits;
    };

public:

    explicit RainDropFilter(QObject* const parent = nullptr);
    explicit RainDropFilter(DImg* const orgImage,
                            QObject* const parent = nullptr,
                            int drop=80,
                            int amount=150,
                            int coeff=30,
                            const QRect& selection=QRect(0, 0, 0, 0));
    ~RainDropFilter() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:RainDropFilter");
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

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;

    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

    void rainDropsImage(DImg* const orgImage, DImg* const destImage, int MinDropSize, int MaxDropSize,
                        int Amount, int Coeff, bool bLimitRange, int progressMin, int progressMax);

    void rainDropsImageMultithreaded(const Args& prm);

    bool CreateRainDrop(uchar* const pBits, int Width, int Height, bool sixteenBit, int bytesDepth,
                        uchar* const pResBits, uchar* const pStatusBits,
                        int X, int Y, int DropSize, double Coeff, bool bLimitRange);

    bool CanBeDropped(int Width, int Height, uchar* const pStatusBits, int X, int Y, int DropSize, bool bLimitRange);

    bool SetDropStatusBits(int Width, int Height, uchar* const pStatusBits, int X, int Y, int DropSize);

    /**
     * A color is represented in RGB value (e.g. 0xFFFFFF is white color).
     * But R, G and B values has 256 values to be used so, this function analyzes
     * the value and limits to this range.
     */
    inline int  limitValues8(int ColorValue);

    inline int  limitValues16(int ColorValue);

    inline bool isInside (int Width, int Height, int X, int Y);

    inline int  pixelOffset(int Width, int X, int Y, int bytesDepth);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_RAIN_DROP_FILTER_H
