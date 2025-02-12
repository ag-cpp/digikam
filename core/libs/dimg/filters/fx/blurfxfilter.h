/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : Blur FX threaded image filter.
 *
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * Original Blur algorithms copyrighted 2004 by
 * Pieter Z. Voloshyn <pieter dot voloshyn at gmail dot com>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "digikam_globals.h"

namespace Digikam
{

class RandomNumberGenerator;

class DIGIKAM_EXPORT BlurFXFilter : public DImgThreadedFilter
{
    Q_OBJECT

public:

    enum BlurFXFilterTypes
    {
        ZoomBlur = 0,
        RadialBlur,
        FarBlur,
        MotionBlur,
        SoftenerBlur,
        ShakeBlur,
        FocusBlur,
        SmartBlur,
        FrostGlass,
        Mosaic
    };

private:

    struct Q_DECL_HIDDEN Args
    {
        explicit Args()
          : start        (0),
            stop         (0),
            h            (0),
            w            (0),
            orgImage     (nullptr),
            destImage    (nullptr),
            X            (0),
            Y            (0),
            Distance     (0),
            nCount       (0),
            lpXArray     (nullptr),
            lpYArray     (nullptr),
            BlendRadius  (0),
            bInversed    (0),
            layer1       (nullptr),
            layer2       (nullptr),
            layer3       (nullptr),
            layer4       (nullptr),
            SizeW        (0),
            SizeH        (0),
            StrengthRange(0),
            Radius       (0),
            Kernel       (nullptr),
            arrMult      (nullptr),
            pBlur        (nullptr)
        {
        }

        uint   start;
        uint   stop;
        uint   h;
        uint   w;
        DImg*  orgImage;
        DImg*  destImage;
        int    X;
        int    Y;
        int    Distance;

        int    nCount;
        int*   lpXArray;
        int*   lpYArray;

        int    BlendRadius;
        bool   bInversed;

        uchar* layer1;
        uchar* layer2;
        uchar* layer3;
        uchar* layer4;

        int    SizeW;
        int    SizeH;

        int    StrengthRange;
        int    Radius;
        int*   Kernel;
        int**  arrMult;
        uchar* pBlur;
    };

public:

    explicit BlurFXFilter(QObject* const parent = nullptr);
    explicit BlurFXFilter(DImg* const orgImage,
                          QObject* const parent = nullptr,
                          int blurFXType = ZoomBlur,
                          int distance = 100,
                          int level = 45);
    ~BlurFXFilter() override;

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:BlurFXFilter");
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

    // Backported from ImageProcessing version 1
    void softenerBlur(DImg* const orgImage, DImg* const destImage);
    void softenerBlurMultithreaded(const Args& prm);

    void shakeBlur(DImg* const orgImage, DImg* const destImage, int Distance);
    void shakeBlurStage1Multithreaded(const Args& prm);
    void shakeBlurStage2Multithreaded(const Args& prm);

    void frostGlass(DImg* const orgImage, DImg* const destImage, int Frost);

    // Backported from ImageProcessing version 2
    void zoomBlur(DImg* const orgImage, DImg* const destImage, int X, int Y, int Distance, const QRect& pArea=QRect());
    void zoomBlurMultithreaded(const Args& prm);

    void radialBlur(DImg* const orgImage, DImg* const destImage, int X, int Y, int Distance, const QRect& pArea=QRect());
    void radialBlurMultithreaded(const Args& prm);

    void focusBlur(DImg* const orgImage, DImg* const destImage, int X, int Y, int BlurRadius, int BlendRadius,
                   bool bInversed=false, const QRect& pArea=QRect());
    void focusBlurMultithreaded(const Args& prm);

    void farBlur(DImg* const orgImage, DImg* const destImage, int Distance);

    void motionBlur(DImg* const orgImage, DImg* const destImage, int Distance, double Angle=0.0);
    void motionBlurMultithreaded(const Args& prm);

    void smartBlur(DImg* const orgImage, DImg* const destImage, int Radius, int Strength);
    void smartBlurStage1Multithreaded(const Args& prm);
    void smartBlurStage2Multithreaded(const Args& prm);

    void mosaic(DImg* const orgImage, DImg* const destImage, int SizeW, int SizeH);
    void mosaicMultithreaded(const Args& prm);

private:

    void MakeConvolution(DImg* const orgImage, DImg* const destImage, int Radius, int Kernel[]);
    void MakeConvolutionStage1Multithreaded(const Args& prm);
    void MakeConvolutionStage2Multithreaded(const Args& prm);

    DColor RandomColor(uchar* const Bits, int Width, int Height, bool sixteenBit, int bytesDepth,
                       int X, int Y, int Radius,
                       int alpha, RandomNumberGenerator& generator, int range, uchar* const IntensityCount,
                       uint* const AverageColorR, uint* const AverageColorG, uint* const AverageColorB);

    // Return the limit defined the max and min values.
    inline int Lim_Max(int Now, int Up, int Max)
    {
        --Max;

        while (Now > Max - Up)
        {
            --Up;
        }

        return (Up);
    };

    // Return the luminance (Y) component of YIQ color model.
    inline int GetIntensity (int R, int G, int B)
    {
        return (int)(R * 0.3 + G * 0.59 + B * 0.11);
    };

    // function to allocate a 2d array
    inline int** Alloc2DArray (int Columns, int Rows)
    {
        // We declare our future 2d array to be returned
        // and we alloc the main pointer with Columns
        int** lpcArray = new int*[Columns];

        for (int i = 0 ; i < Columns ; ++i)
        {
            lpcArray[i] = new int[Rows];
        }

        return (lpcArray);
    }

    // Function to deallocates the 2d array previously created
    inline void Free2DArray (int** lpcArray, int Columns)
    {
        // loop to deallocate the columns
        for (int i = 0 ; i < Columns ; ++i)
        {
            delete [] lpcArray[i];
        }

        // now, we delete the main pointer
        delete [] lpcArray;
    }

    inline bool IsInside (int Width, int Height, int X, int Y)
    {
        bool bIsWOk = ((X < 0) ? false : (X >= Width ) ? false : true);
        bool bIsHOk = ((Y < 0) ? false : (Y >= Height) ? false : true);

        return (bIsWOk && bIsHOk);
    };

    inline int GetOffset(int Width, int X, int Y, int bytesDepth)
    {
        return (Y * Width * bytesDepth) + (X * bytesDepth);
    };

    inline int GetOffsetAdjusted(int Width, int Height, int X, int Y, int bytesDepth)
    {
        X = (X < 0) ?  0  :  ((X >= Width ) ? (Width  - 1) : X);
        Y = (Y < 0) ?  0  :  ((Y >= Height) ? (Height - 1) : Y);

        return GetOffset(Width, X, Y, bytesDepth);
    };

    inline bool IsColorInsideTheRange (int cR, int cG, int cB,
                                       int nR, int nG, int nB,
                                       int Range)
    {
        if ((nR >= (cR - Range)) && (nR <= (cR + Range)))
        {
            if ((nG >= (cG - Range)) && (nG <= (cG + Range)))
            {
                if ((nB >= (cB - Range)) && (nB <= (cB + Range)))
                {
                    return true;
                }
            }
        }

        return false;
    };

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
