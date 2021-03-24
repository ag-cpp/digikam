/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : filter to add Film Grain to image.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_FILM_GRAIN_FILTER_H
#define DIGIKAM_FILM_GRAIN_FILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"
#include "dcolor.h"

namespace Digikam
{

class DIGIKAM_EXPORT FilmGrainContainer
{

public:

    FilmGrainContainer()
      : grainSize(1),
        photoDistribution(false),
        addLuminanceNoise(true),
        lumaIntensity(25),
        lumaShadows(-100),
        lumaMidtones(0),
        lumaHighlights(-100),
        addChrominanceBlueNoise(false),
        chromaBlueIntensity(25),
        chromaBlueShadows(-100),
        chromaBlueMidtones(0),
        chromaBlueHighlights(-100),
        addChrominanceRedNoise(false),
        chromaRedIntensity(25),
        chromaRedShadows(-100),
        chromaRedMidtones(0),
        chromaRedHighlights(-100)
    {
    };

    ~FilmGrainContainer()
    {
    };

    bool isDirty() const
    {
        return (addLuminanceNoise || addChrominanceBlueNoise || addChrominanceRedNoise);
    };

public:

    int  grainSize;
    bool photoDistribution;

    bool addLuminanceNoise;
    int  lumaIntensity;
    int  lumaShadows;
    int  lumaMidtones;
    int  lumaHighlights;

    bool addChrominanceBlueNoise;
    int  chromaBlueIntensity;
    int  chromaBlueShadows;
    int  chromaBlueMidtones;
    int  chromaBlueHighlights;

    bool addChrominanceRedNoise;
    int  chromaRedIntensity;
    int  chromaRedShadows;
    int  chromaRedMidtones;
    int  chromaRedHighlights;
};

// -----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT FilmGrainFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    explicit FilmGrainFilter(QObject* const parent = nullptr);
    explicit FilmGrainFilter(DImg* const orgImage, QObject* const parent = nullptr, const FilmGrainContainer& settings=FilmGrainContainer());
    // Constructor for slave mode: execute immediately in current thread with specified master filter
    explicit FilmGrainFilter(DImgThreadedFilter* const parentFilter, const DImg& orgImage, const DImg& destImage,
                             int progressBegin=0, int progressEnd=100,
                             const FilmGrainContainer& settings=FilmGrainContainer());
    ~FilmGrainFilter() override;


    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:FilmGrainFilter");
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

    void                    readParameters(const FilterAction& action)        override;

    QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    FilterAction    filterAction()                                    override;

private:

    void filterImage()                                                        override;

    void filmgrainMultithreaded(uint start, uint stop);

    inline void computeNoiseSettings(const DColor& col,
                                     double& luRange, double& luNoise,
                                     double& cbRange, double& cbNoise,
                                     double& crRange, double& crNoise);
    inline double interpolate(int shadows, int midtones, int highlights, const DColor& col);
    inline double randomizeUniform(double range);
    inline double randomizeGauss(double sigma);
    inline double randomizePoisson(double lambda);
    inline void   adjustYCbCr(DColor& col, double range, double nRand, int channel);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FILM_GRAIN_FILTER_H
