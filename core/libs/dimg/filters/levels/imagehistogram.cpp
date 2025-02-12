/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-07-21
 * Description : image histogram manipulation methods.
 *
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagehistogram.h"

// C++ includes

#include <cstdio>
#include <cmath>
#include <cstring>

// Qt includes


// Local includes

#include "dimg.h"
#include "digikam_debug.h"
#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageHistogram::Private
{

public:

    // NOTE : Using a structure instead a class is more faster
    // with bytes manipulation.

    struct Q_DECL_HIDDEN double_packet
    {
        double value = 0.0;
        double red   = 0.0;
        double green = 0.0;
        double blue  = 0.0;
        double alpha = 0.0;
    };

public:

    Private() = default;

public:

    /**
     * The histogram data.
     */
    struct double_packet* histogram     = nullptr;
    bool                  valid         = false;

    /**
     * Image information.
     */
    DImg                  img;

    /**
     * Numbers of histogram segments depending of image bytes depth
     */
    int                   histoSegments = 0;
};

ImageHistogram::ImageHistogram(const DImg& img, QObject* const parent)
    : DynamicThread(parent),
      d            (new Private)
{
    // A simple copy of reference must be enough instead a deep copy. See this bug comment for details:
    // https://bugs.kde.org/show_bug.cgi?id=274555#c40
/*
    d->img           = img.copy();
*/
    d->img           = img;
    d->histoSegments = d->img.sixteenBit() ? NUM_SEGMENTS_16BIT : NUM_SEGMENTS_8BIT;
}

ImageHistogram::~ImageHistogram()
{
    stopCalculation();

    if (d->histogram)
    {
        delete [] d->histogram;
    }

    delete d;
}

bool ImageHistogram::isSixteenBit() const
{
    return d->img.sixteenBit();
}

int ImageHistogram::getHistogramSegments() const
{
    return d->histoSegments;
}

int ImageHistogram::getMaxSegmentIndex() const
{
    return (d->histoSegments - 1);
}

void ImageHistogram::calculateInThread()
{
    // this is done in an extra method and not in the constructor
    // to allow to connect to the signals, which is only possible after construction

    if (!d->img.isNull())
    {
        Q_EMIT calculationAboutToStart();
        start();
    }
    else
    {
        Q_EMIT calculationFinished(false);
    }
}

void ImageHistogram::stopCalculation()
{
    stop();
    wait();
}

bool ImageHistogram::isValid() const
{
    return d->valid;
}

bool ImageHistogram::isCalculating() const
{
    return isRunning();
}

// List of threaded operations.

void ImageHistogram::run()
{
    calculate();
}

void ImageHistogram::calculate()
{
    // TODO this gets even called with null img

    if (d->img.isNull())
    {
        Q_EMIT calculationFinished(false);

        return;
    }

    // check if the calculation has been done before

    if (d->histogram && d->valid)
    {
        Q_EMIT calculationFinished(true);

        return;
    }

    uint i;
    int  max;

    Q_EMIT calculationStarted();

    if (!d->histogram)
    {
        d->histogram = new Private::double_packet[d->histoSegments];
    }

    if (!d->histogram)
    {
        qCWarning(DIGIKAM_DIMG_LOG) << ("HistogramWidget::calcHistogramValues: Unable to allocate memory!");

        Q_EMIT calculationFinished(false);

        return;
    }

    if (isSixteenBit())         // 16 bits image.
    {
        unsigned short blue, green, red, alpha;
        unsigned short* const data = reinterpret_cast<unsigned short*>(d->img.bits());

        // count here instead of inside the loop, because d is not optimized because it's not defined in the header

        const uint count = d->img.width() * d->img.height() * 4;

        for (i = 0 ; runningFlag() && (i < count) ; i += 4)
        {
            blue  = data[i    ];
            green = data[i + 1];
            red   = data[i + 2];
            alpha = data[i + 3];

            d->histogram[blue].blue++;
            d->histogram[green].green++;
            d->histogram[red].red++;
            d->histogram[alpha].alpha++;

            max = (blue > green) ? blue : green;

            if (red > max)
            {
                d->histogram[red].value++;
            }
            else
            {
                d->histogram[max].value++;
            }
        }
    }
    else                                  // 8 bits images.
    {
        uchar blue, green, red, alpha;
        const uchar* const data = d->img.bits();

        // count here instead of inside the loop, because d is not optimized because it's not defined in the header

        const uint count = d->img.width() * d->img.height() * 4;

        for (i = 0 ; runningFlag() && (i < count) ; i += 4)
        {
            blue  = data[i    ];
            green = data[i + 1];
            red   = data[i + 2];
            alpha = data[i + 3];

            d->histogram[blue].blue++;
            d->histogram[green].green++;
            d->histogram[red].red++;
            d->histogram[alpha].alpha++;

            max = (blue > green) ? blue : green;

            if (red > max)
            {
                d->histogram[red].value++;
            }
            else
            {
                d->histogram[max].value++;
            }
        }
    }

    if (runningFlag())
    {
        d->valid = true;

        Q_EMIT calculationFinished(true);
    }
}

double ImageHistogram::getCount(int channel, int start, int end) const
{
    int    i;
    double count = 0.0;

    if (!d->histogram || (start < 0) ||
        (end > d->histoSegments - 1) || (start > end))
    {
        return 0.0;
    }

    switch (channel)
    {
        case LuminosityChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                count += d->histogram[i].value;
            }

            break;
        }

        case RedChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                count += d->histogram[i].red;
            }

            break;
        }

        case GreenChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                count += d->histogram[i].green;
            }

            break;
        }

        case BlueChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                count += d->histogram[i].blue;
            }

            break;
        }

        case AlphaChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                count += d->histogram[i].alpha;
            }

            break;
        }
    }

    return count;
}

double ImageHistogram::getPixels() const
{
    if (!d->histogram)
    {
        return 0.0;
    }

    return (double)(d->img.numPixels());
}

double ImageHistogram::getMean(int channel, int start, int end) const
{
    int    i;
    double mean = 0.0;
    double count;

    if (!d->histogram || (start < 0) ||
        (end > d->histoSegments - 1) || (start > end))
    {
        return 0.0;
    }

    switch (channel)
    {
        case LuminosityChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                mean += i * d->histogram[i].value;
            }

            break;
        }

        case RedChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                mean += i * d->histogram[i].red;
            }

            break;
        }

        case GreenChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                mean += i * d->histogram[i].green;
            }

            break;
        }

        case BlueChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                mean += i * d->histogram[i].blue;
            }

            break;
        }

        case AlphaChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                mean += i * d->histogram[i].alpha;
            }

            break;
        }

        default:
        {
            return 0.0;
        }
    }

    count = getCount(channel, start, end);

    if (count > 0.0)
    {
        return (mean / count);
    }

    return mean;
}

int ImageHistogram::getMedian(int channel, int start, int end) const
{
    int    i;
    double sum = 0.0;
    double count;

    if (!d->histogram || (start < 0) ||
        (end > d->histoSegments - 1) || (start > end))
    {
        return 0;
    }

    count = getCount(channel, start, end);

    switch (channel)
    {
        case LuminosityChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                sum += d->histogram[i].value;

                if (sum * 2 > count)
                {
                    return i;
                }
            }

            break;
        }

        case RedChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                sum += d->histogram[i].red;

                if (sum * 2 > count)
                {
                    return i;
                }
            }

            break;
        }

        case GreenChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                sum += d->histogram[i].green;

                if (sum * 2 > count)
                {
                    return i;
                }
            }

            break;
        }

        case BlueChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                sum += d->histogram[i].blue;

                if (sum * 2 > count)
                {
                    return i;
                }
            }

            break;
        }

        case AlphaChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                sum += d->histogram[i].alpha;

                if (sum * 2 > count)
                {
                    return i;
                }
            }

            break;
        }

        default:
        {
            return 0;
        }
    }

    return 0;
}

double ImageHistogram::getStdDev(int channel, int start, int end) const
{
    int    i;
    double dev = 0.0;
    double count;
    double mean;

    if (!d->histogram || (start < 0) ||
        (end > d->histoSegments - 1) || (start > end))
    {
        return 0.0;
    }

    mean  = getMean(channel, start, end);
    count = getCount(channel, start, end);

    if (count == 0.0)
    {
        count = 1.0;
    }

    switch (channel)
    {
        case LuminosityChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                dev += (i - mean) * (i - mean) * d->histogram[i].value;
            }

            break;
        }

        case RedChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                dev += (i - mean) * (i - mean) * d->histogram[i].red;
            }

            break;
        }

        case GreenChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                dev += (i - mean) * (i - mean) * d->histogram[i].green;
            }

            break;
        }

        case BlueChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                dev += (i - mean) * (i - mean) * d->histogram[i].blue;
            }

            break;
        }

        case AlphaChannel:
        {
            for (i = start ; i <= end ; ++i)
            {
                dev += (i - mean) * (i - mean) * d->histogram[i].alpha;
            }

            break;
        }

        default:
        {
            return 0.0;
        }
    }

    return sqrt(dev / count);
}

double ImageHistogram::getValue(int channel, int bin) const
{
    double value;

    if (!d->histogram || (bin < 0) || (bin > d->histoSegments - 1))
    {
        return 0.0;
    }

    switch (channel)
    {
        case LuminosityChannel:
        {
            value = d->histogram[bin].value;
            break;
        }

        case RedChannel:
        {
            value = d->histogram[bin].red;
            break;
        }

        case GreenChannel:
        {
            value = d->histogram[bin].green;
            break;
        }

        case BlueChannel:
        {
            value = d->histogram[bin].blue;
            break;
        }

        case AlphaChannel:
        {
            value = d->histogram[bin].alpha;
            break;
        }

        default:
        {
            return 0.0;
        }
    }

    return value;
}

double ImageHistogram::getMaximum(int channel, int start, int end) const
{
    double max = 0.0;
    int    x;

    if (!d->histogram || (start < 0) ||
        (end > d->histoSegments - 1) || (start > end))
    {
        return 0.0;
    }

    switch (channel)
    {
        case LuminosityChannel:
        {
            for (x = start ; x <= end ; ++x)
            {
                if (d->histogram[x].value > max)
                {
                    max = d->histogram[x].value;
                }
            }

            break;
        }

        case RedChannel:
        {
            for (x = start ; x <= end ; ++x)
            {
                if (d->histogram[x].red > max)
                {
                    max = d->histogram[x].red;
                }
            }

            break;
        }

        case GreenChannel:
        {
            for (x = start ; x <= end ; ++x)
            {
                if (d->histogram[x].green > max)
                {
                    max = d->histogram[x].green;
                }
            }

            break;
        }

        case BlueChannel:
        {
            for (x = start ; x <= end ; ++x)
            {
                if (d->histogram[x].blue > max)
                {
                    max = d->histogram[x].blue;
                }
            }

            break;
        }

        case AlphaChannel:
        {
            for (x = start ; x <= end ; ++x)
            {
                if (d->histogram[x].alpha > max)
                {
                    max = d->histogram[x].alpha;
                }
            }

            break;
        }

        default:
        {
            return 0.0;
        }
    }

    return max;
}

} // namespace Digikam

#include "moc_imagehistogram.cpp"
