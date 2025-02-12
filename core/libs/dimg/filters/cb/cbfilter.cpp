/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-18
 * Description : color balance filter
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "cbfilter.h"

// C++ includes

#include <cstdio>
#include <cmath>

// Local includes

#include "dimg.h"
#include "digikam_globals_p.h"      // For KF6::Ki18n deprecated

namespace Digikam
{

class Q_DECL_HIDDEN CBFilter::Private
{
public:

    Private() = default;

    int         redMap[256]         = { 0 };
    int         greenMap[256]       = { 0 };
    int         blueMap[256]        = { 0 };
    int         alphaMap[256]       = { 0 };

    int         redMap16[65536]     = { 0 };
    int         greenMap16[65536]   = { 0 };
    int         blueMap16[65536]    = { 0 };
    int         alphaMap16[65536]   = { 0 };

    CBContainer settings;
};

CBFilter::CBFilter(QObject* const parent)
    : DImgThreadedFilter(parent),
      d                 (new Private)
{
    reset();
    initFilter();
}

CBFilter::CBFilter(DImg* const orgImage,
                   QObject* const parent,
                   const CBContainer& settings)
    : DImgThreadedFilter(orgImage, parent, QLatin1String("CBFilter")),
      d                 (new Private)
{
    d->settings = settings;
    reset();
    initFilter();
}

CBFilter::CBFilter(const CBContainer& settings,
                   DImgThreadedFilter* const master,
                   const DImg& orgImage,
                   DImg& destImage,
                   int progressBegin,
                   int progressEnd)
    : DImgThreadedFilter(master, orgImage, destImage, progressBegin, progressEnd, QLatin1String("CBFilter")),
      d                 (new Private)
{
    d->settings = settings;
    reset();
    initFilter();
    destImage   = m_destImage;
}

CBFilter::~CBFilter()
{
    cancelFilter();
    delete d;
}

QString CBFilter::DisplayableName()
{
    return QString::fromUtf8(I18N_NOOP("Color Balance Tool"));
}

void CBFilter::filterImage()
{
    setGamma(d->settings.gamma);
    applyCBFilter(m_orgImage, d->settings.red, d->settings.green, d->settings.blue, d->settings.alpha);
    m_destImage = m_orgImage;
}

void CBFilter::reset()
{
    // initialize to linear mapping

    for (int i = 0 ; i < 65536 ; ++i)
    {
        d->redMap16[i]   = i;
        d->greenMap16[i] = i;
        d->blueMap16[i]  = i;
        d->alphaMap16[i] = i;
    }

    for (int i = 0 ; i < 256 ; ++i)
    {
        d->redMap[i]   = i;
        d->greenMap[i] = i;
        d->blueMap[i]  = i;
        d->alphaMap[i] = i;
    }
}

void CBFilter::setTables(int* const redMap,
                         int* const greenMap,
                         int* const blueMap,
                         int* const alphaMap,
                         bool sixteenBit)
{
    if (!sixteenBit)
    {
        for (int i = 0 ; i < 256 ; ++i)
        {
            if (redMap)
            {
                d->redMap[i]   = redMap[i];
            }

            if (greenMap)
            {
                d->greenMap[i] = greenMap[i];
            }

            if (blueMap)
            {
                d->blueMap[i]  = blueMap[i];
            }

            if (alphaMap)
            {
                d->alphaMap[i] = alphaMap[i];
            }
        }
    }
    else
    {
        for (int i = 0 ; i < 65536 ; ++i)
        {
            if (redMap)
            {
                d->redMap16[i]   = redMap[i];
            }

            if (greenMap)
            {
                d->greenMap16[i] = greenMap[i];
            }

            if (blueMap)
            {
                d->blueMap16[i]  = blueMap[i];
            }

            if (alphaMap)
            {
                d->alphaMap16[i] = alphaMap[i];
            }
        }
    }
}

void CBFilter::getTables(int* const redMap,
                         int* const greenMap,
                         int* const blueMap,
                         int* const alphaMap,
                         bool sixteenBit)
{
    if (!sixteenBit)
    {
        if (redMap)
        {
            memcpy(redMap, d->redMap, (256 * sizeof(int)));
        }

        if (greenMap)
        {
            memcpy(greenMap, d->greenMap, (256 * sizeof(int)));
        }

        if (blueMap)
        {
            memcpy(blueMap, d->blueMap, (256 * sizeof(int)));
        }

        if (alphaMap)
        {
            memcpy(alphaMap, d->alphaMap, (256 * sizeof(int)));
        }
    }
    else
    {
        if (redMap)
        {
            memcpy(redMap, d->redMap16, (65536 * sizeof(int)));
        }

        if (greenMap)
        {
            memcpy(greenMap, d->greenMap16, (65536 * sizeof(int)));
        }

        if (blueMap)
        {
            memcpy(blueMap, d->blueMap16, (65536 * sizeof(int)));
        }

        if (alphaMap)
        {
            memcpy(alphaMap, d->alphaMap16, (65536 * sizeof(int)));
        }
    }
}

void CBFilter::applyCBFilter(DImg& image, double r, double g, double b, double a)
{
    if (image.isNull())
    {
        return;
    }

    uint size = image.width() * image.height();
    int  progress;

    adjustRGB(r, g, b, a, image.sixteenBit());

    if (!image.sixteenBit())                    // 8 bits image.
    {
        uchar* data = (uchar*) image.bits();

        for (uint i = 0 ; runningFlag() && (i < size) ; ++i)
        {
            data[0]  = d->blueMap[data[0]];
            data[1]  = d->greenMap[data[1]];
            data[2]  = d->redMap[data[2]];
            data[3]  = d->alphaMap[data[3]];

            data    += 4;

            progress = (int)(((double)i * 100.0) / size);

            if ((progress % 5) == 0)
            {
                postProgress(progress);
            }
        }
    }
    else                                        // 16 bits image.
    {
        ushort* data = reinterpret_cast<ushort*>(image.bits());

        for (uint i = 0 ; runningFlag() && (i < size) ; ++i)
        {
            data[0]  = d->blueMap16[data[0]];
            data[1]  = d->greenMap16[data[1]];
            data[2]  = d->redMap16[data[2]];
            data[3]  = d->alphaMap16[data[3]];

            data    += 4;

            progress = (int)(((double)i * 100.0) / size);

            if ((progress % 5) == 0)
            {
                postProgress(progress);
            }
        }
    }
}

void CBFilter::setGamma(double val)
{
    val = (val < 0.01) ? 0.01 : val;
    int val2;

    for (int i = 0 ; i < 65536 ; ++i)
    {
        val2             = (int)(pow(((double)d->redMap16[i] / 65535), (1 / val)) * 65535);
        d->redMap16[i]   = CLAMP065535(val2);

        val2             = (int)(pow(((double)d->greenMap16[i] / 65535), (1 / val)) * 65535);
        d->greenMap16[i] = CLAMP065535(val2);

        val2             = (int)(pow(((double)d->blueMap16[i] / 65535), (1 / val)) * 65535);
        d->blueMap16[i]  = CLAMP065535(val2);

        val2             = (int)(pow(((double)d->alphaMap16[i] / 65535), (1 / val)) * 65535);
        d->alphaMap16[i] = CLAMP065535(val2);
    }

    for (int i = 0 ; i < 256 ; ++i)
    {
        val2           = (int)(pow(((double)d->redMap[i] / 255), (1 / val)) * 255);
        d->redMap[i]   = CLAMP0255(val2);

        val2           = (int)(pow(((double)d->greenMap[i] / 255), (1 / val)) * 255);
        d->greenMap[i] = CLAMP0255(val2);

        val2           = (int)(pow(((double)d->blueMap[i] / 255), (1 / val)) * 255);
        d->blueMap[i]  = CLAMP0255(val2);

        val2           = (int)(pow(((double)d->alphaMap[i] / 255), (1 / val)) * 255);
        d->alphaMap[i] = CLAMP0255(val2);
    }
}

void CBFilter::adjustRGB(double r, double g, double b, double a, bool sixteenBit)
{
    if ((r == 1.0) && (g == 1.0) && (b == 1.0) && (a == 1.0))
    {
        // Nothing to do.
        return ;
    }

    int* r_table     = nullptr;
    int* g_table     = nullptr;
    int* b_table     = nullptr;
    int* a_table     = nullptr;
    int* dummy_table = nullptr;

    if (sixteenBit)
    {
        r_table     = new int[65536];
        g_table     = new int[65536];
        b_table     = new int[65536];
        a_table     = new int[65536];
        dummy_table = new int[65536];
    }
    else
    {
        r_table     = new int[256];
        g_table     = new int[256];
        b_table     = new int[256];
        a_table     = new int[256];
        dummy_table = new int[256];
    }

    if ((r == g) && (r == b) && (r == a))
    {
        setGamma(r);
    }
    else
    {
        getTables(r_table, g_table, b_table, a_table, sixteenBit);

        if (r != 1.0)
        {
            setGamma(r);
            getTables(r_table, dummy_table, dummy_table, dummy_table, sixteenBit);
            reset();
        }

        if (g != 1.0)
        {
            setGamma(g);
            getTables(dummy_table, g_table, dummy_table, dummy_table, sixteenBit);
            reset();
        }

        if (b != 1.0)
        {
            setGamma(b);
            getTables(dummy_table, dummy_table, b_table, dummy_table, sixteenBit);
            reset();
        }

        if (a != 1.0)
        {
            setGamma(a);
            getTables(dummy_table, dummy_table, dummy_table, a_table, sixteenBit);
            reset();
        }

        setTables(r_table, g_table, b_table, a_table, sixteenBit);
    }

    delete [] r_table;
    delete [] g_table;
    delete [] b_table;
    delete [] a_table;
    delete [] dummy_table;
}

FilterAction CBFilter::filterAction()
{
    FilterAction action(FilterIdentifier(), CurrentVersion());
    action.setDisplayableName(DisplayableName());

    action.addParameter(QLatin1String("alpha"), d->settings.alpha);
    action.addParameter(QLatin1String("blue"),  d->settings.blue);
    action.addParameter(QLatin1String("gamma"), d->settings.gamma);
    action.addParameter(QLatin1String("green"), d->settings.green);
    action.addParameter(QLatin1String("red"),   d->settings.red);

    return action;
}

void CBFilter::readParameters(const Digikam::FilterAction& action)
{
    d->settings.alpha = action.parameter(QLatin1String("alpha")).toDouble();
    d->settings.blue  = action.parameter(QLatin1String("blue")).toDouble();
    d->settings.gamma = action.parameter(QLatin1String("gamma")).toDouble();
    d->settings.green = action.parameter(QLatin1String("green")).toDouble();
    d->settings.red   = action.parameter(QLatin1String("red")).toDouble();
}

} // namespace Digikam

#include "moc_cbfilter.cpp"
