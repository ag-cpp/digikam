/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border threaded image filter.
 *
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "borderfilter.h"

// C++ includes

#include <cmath>
#include <cstdlib>

// Qt includes

#include <QPoint>
#include <QPolygon>
#include <QRegion>
#include <QRegularExpression>

// Local includes

#include "dimg.h"
#include "digikam_debug.h"
#include "digikam_globals_p.h"      // For KF6::Ki18n deprecated

namespace Digikam
{

class Q_DECL_HIDDEN BorderFilter::Private
{
public:

    Private() = default;

    int             borderMainWidth = 0;
    int             border2ndWidth  = 0;

    float           orgRatio        = 0.0F;

    DColor          solidColor;
    DColor          niepceBorderColor;
    DColor          niepceLineColor;
    DColor          bevelUpperLeftColor;
    DColor          bevelLowerRightColor;
    DColor          decorativeFirstColor;
    DColor          decorativeSecondColor;

    BorderContainer settings;

public:

    void setup(const DImg& m_orgImage);
};

void BorderFilter::Private::setup(const DImg& m_orgImage)
{
    solidColor            = DColor(settings.solidColor,            m_orgImage.sixteenBit());
    niepceBorderColor     = DColor(settings.niepceBorderColor,     m_orgImage.sixteenBit());
    niepceLineColor       = DColor(settings.niepceLineColor,       m_orgImage.sixteenBit());
    bevelUpperLeftColor   = DColor(settings.bevelUpperLeftColor,   m_orgImage.sixteenBit());
    bevelLowerRightColor  = DColor(settings.bevelLowerRightColor,  m_orgImage.sixteenBit());
    decorativeFirstColor  = DColor(settings.decorativeFirstColor,  m_orgImage.sixteenBit());
    decorativeSecondColor = DColor(settings.decorativeSecondColor, m_orgImage.sixteenBit());

    if (settings.preserveAspectRatio)
    {
        orgRatio        = (float)settings.orgWidth / (float)settings.orgHeight;
        int size        = qMin(m_orgImage.height(), m_orgImage.width());
        borderMainWidth = (int)(size * settings.borderPercent);
        border2ndWidth  = (int)(size * 0.005);

        // Clamp internal border with to 1 pixel to be visible with small image.

        if (border2ndWidth < 1)
        {
            border2ndWidth = 1;
        }
    }
}

BorderFilter::BorderFilter(QObject* const parent)
    : DImgThreadedFilter(parent),
      d                 (new Private)
{
    initFilter();
}

BorderFilter::BorderFilter(DImg* image, QObject* const parent, const BorderContainer& settings)
    : DImgThreadedFilter(image, parent, QLatin1String("Border")),
      d                 (new Private)
{
    d->settings = settings;
    initFilter();
}

BorderFilter::~BorderFilter()
{
    cancelFilter();

    delete d;
}

QString BorderFilter::DisplayableName()
{
    return QString::fromUtf8(I18N_NOOP("Border Tool"));
}

void BorderFilter::filterImage()
{
    d->setup(m_orgImage);

    switch (d->settings.borderType)
    {
        case BorderContainer::SolidBorder:
        {
            if (d->settings.preserveAspectRatio)
            {
                solid(m_orgImage, m_destImage, d->solidColor, d->borderMainWidth);
            }
            else
            {
                solid2(m_orgImage, m_destImage, d->solidColor, d->settings.borderWidth1);
            }

            break;
        }

        case BorderContainer::NiepceBorder:
        {
            if (d->settings.preserveAspectRatio)
            {
                niepce(m_orgImage, m_destImage, d->niepceBorderColor, d->borderMainWidth,
                       d->niepceLineColor, d->border2ndWidth);
            }
            else
            {
                niepce2(m_orgImage, m_destImage, d->niepceBorderColor, d->settings.borderWidth1,
                        d->niepceLineColor, d->settings.borderWidth4);
            }

            break;
        }

        case BorderContainer::BeveledBorder:
        {
            if (d->settings.preserveAspectRatio)
            {
                bevel(m_orgImage, m_destImage, d->bevelUpperLeftColor,
                      d->bevelLowerRightColor, d->borderMainWidth);
            }
            else
            {
                bevel2(m_orgImage, m_destImage, d->bevelUpperLeftColor,
                       d->bevelLowerRightColor, d->settings.borderWidth1);
            }

            break;
        }

        case BorderContainer::PineBorder:
        case BorderContainer::WoodBorder:
        case BorderContainer::PaperBorder:
        case BorderContainer::ParqueBorder:
        case BorderContainer::IceBorder:
        case BorderContainer::LeafBorder:
        case BorderContainer::MarbleBorder:
        case BorderContainer::RainBorder:
        case BorderContainer::CratersBorder:
        case BorderContainer::DriedBorder:
        case BorderContainer::PinkBorder:
        case BorderContainer::StoneBorder:
        case BorderContainer::ChalkBorder:
        case BorderContainer::GraniteBorder:
        case BorderContainer::RockBorder:
        case BorderContainer::WallBorder:
        {
            if (d->settings.preserveAspectRatio)
            {
                pattern(m_orgImage, m_destImage, d->borderMainWidth,
                        d->decorativeFirstColor, d->decorativeSecondColor,
                        d->border2ndWidth, d->border2ndWidth);
            }
            else
            {
                pattern2(m_orgImage, m_destImage, d->settings.borderWidth1,
                         d->decorativeFirstColor, d->decorativeSecondColor,
                         d->settings.borderWidth2, d->settings.borderWidth2);
            }

            break;
        }
    }
}

// -- Methods to preserve aspect ratio of image ------------------------------------------

void BorderFilter::solid(const DImg& src, DImg& dest, const DColor& fg, int borderWidth)
{
    if (d->settings.orgWidth > d->settings.orgHeight)
    {
        int height = src.height() + borderWidth * 2;
        dest       = DImg((int)(height * d->orgRatio), height, src.sixteenBit(), src.hasAlpha());
        dest.fill(fg);
        dest.bitBltImage(&src, (dest.width() - src.width()) / 2, borderWidth);
    }
    else
    {
        int width = src.width() + borderWidth * 2;
        dest      = DImg(width, (int)(width / d->orgRatio), src.sixteenBit(), src.hasAlpha());
        dest.fill(fg);
        dest.bitBltImage(&src, borderWidth, (dest.height() - src.height()) / 2);
    }
}

void BorderFilter::niepce(const DImg& src, DImg& dest, const DColor& fg,
                          int borderWidth, const DColor& bg, int lineWidth)
{
    DImg tmp;
    solid(src, tmp, bg, lineWidth);
    solid(tmp, dest, fg, borderWidth);
}

void BorderFilter::bevel(const DImg& src, DImg& dest, const DColor& topColor,
                         const DColor& btmColor, int borderWidth)
{
    int width, height;

    if (d->settings.orgWidth > d->settings.orgHeight)
    {
        height = src.height() + borderWidth * 2;
        width  = (int)(height * d->orgRatio);
    }
    else
    {
        width  = src.width() + borderWidth * 2;
        height = (int)(width / d->orgRatio);
    }

    dest = DImg(width, height, src.sixteenBit(), src.hasAlpha());
    dest.fill(topColor);

    QPolygon btTriangle(3);
    btTriangle.setPoint(0, width, 0);
    btTriangle.setPoint(1, 0,     height);
    btTriangle.setPoint(2, width, height);
    QRegion btRegion(btTriangle);

    // paint upper right corner

    QPoint upperRightCorner((width - ((width - src.width()) / 2) - 2),
                            ((0 + (height - src.height())) / 2 + 2));

    for (int x = upperRightCorner.x() ; x < width ; ++x)
    {
        for (int y = 0 ; y < upperRightCorner.y() ; ++y)
        {
            if (btRegion.contains(QPoint(x, y)))
            {
                dest.setPixelColor(x, y, btmColor);
            }
        }
    }

    // paint right border

    for (int x = upperRightCorner.x() ; x < width ; ++x)
    {
        for (int y = upperRightCorner.y() ; y < height ; ++y)
        {
            dest.setPixelColor(x, y, btmColor);
        }
    }

    // paint lower left corner

    QPoint lowerLeftCorner((0 + ((width - src.width()) / 2) + 2),
                           (height - ((height - src.height()) / 2) - 2));

    for (int x = 0 ; x < lowerLeftCorner.x() ; ++x)
    {
        for (int y = lowerLeftCorner.y() ; y < height ; ++y)
        {
            if (btRegion.contains(QPoint(x, y)))
            {
                dest.setPixelColor(x, y, btmColor);
            }
        }
    }

    // paint bottom border

    for (int x = lowerLeftCorner.x() ; x < width ; ++x)
    {
        for (int y = lowerLeftCorner.y() ; y < height ; ++y)
        {
            dest.setPixelColor(x, y, btmColor);
        }
    }

    if (d->settings.orgWidth > d->settings.orgHeight)
    {
        dest.bitBltImage(&src, (dest.width() - src.width()) / 2, borderWidth);
    }
    else
    {
        dest.bitBltImage(&src, borderWidth, (dest.height() - src.height()) / 2);
    }
}

void BorderFilter::pattern(const DImg& src, DImg& dest, int borderWidth,
                           const DColor& firstColor, const DColor& secondColor,
                           int firstWidth, int secondWidth)
{
    // Original image with the second solid border around.

    DImg tmp;
    solid(src, tmp, secondColor, secondWidth);

    // Border tiled image using pattern with second solid border around.

    int width, height, destW, destH;

    if (d->settings.orgWidth > d->settings.orgHeight)
    {
        height = d->settings.orgHeight + borderWidth * 2;
        width  = (int)(height * d->orgRatio);

        destH  = src.height() + borderWidth * 2;
        destW  = (int)(destH * d->orgRatio);
    }
    else
    {
        width  = d->settings.orgWidth + borderWidth * 2;
        height = (int)(width / d->orgRatio);

        destW  = src.width() + borderWidth * 2;
        destH  = (int)(destW / d->orgRatio);
    }

    DImg tmp2(width, height, tmp.sixteenBit(), tmp.hasAlpha());
    qCDebug(DIGIKAM_DIMG_LOG) << "Border File:" << d->settings.borderPath;
    DImg border(d->settings.borderPath);

    if (border.isNull())
    {
        return;
    }

    border.convertToDepthOfImage(&tmp2);

    for (int x = 0 ; x < width ; x += border.width())
    {
        for (int y = 0 ; y < height ; y += border.height())
        {
            tmp2.bitBltImage(&border, x, y);
        }
    }

    tmp2 = tmp2.smoothScale(destW, destH);

    solid(tmp2, dest, firstColor, firstWidth);

    // Merge both images to one.

    if (d->settings.orgWidth > d->settings.orgHeight)
    {
        dest.bitBltImage(&tmp, (dest.width() - tmp.width()) / 2, borderWidth);
    }
    else
    {
        dest.bitBltImage(&tmp, borderWidth, (dest.height() - tmp.height()) / 2);
    }
}

// -- Methods to not-preserve aspect ratio of image ------------------------------------------

void BorderFilter::solid2(const DImg& src, DImg& dest, const DColor& fg, int borderWidth)
{
    dest = DImg(src.width() + borderWidth * 2, src.height() + borderWidth * 2,
                src.sixteenBit(), src.hasAlpha());
    dest.fill(fg);
    dest.bitBltImage(&src, borderWidth, borderWidth);
}

void BorderFilter::niepce2(const DImg& src, DImg& dest, const DColor& fg, int borderWidth,
                           const DColor& bg, int lineWidth)
{
    DImg tmp;
    solid2(src, tmp, bg, lineWidth);
    solid2(tmp, dest, fg, borderWidth);
}

void BorderFilter::bevel2(const DImg& src, DImg& dest, const DColor& topColor,
                          const DColor& btmColor, int borderWidth)
{
    int x, y;
    int wc;

    dest = DImg(src.width()  + borderWidth * 2,
                src.height() + borderWidth * 2,
                src.sixteenBit(), src.hasAlpha());

    // top

    for (y = 0, wc = (int)dest.width() - 1 ; y < borderWidth ; ++y, --wc)
    {
        for (x = 0; x < wc; ++x)
        {
            dest.setPixelColor(x, y, topColor);
        }

        for ( ; x < (int)dest.width() ; ++x)
        {
            dest.setPixelColor(x, y, btmColor);
        }
    }

    // left and right

    for ( ; y < (int)dest.height() - borderWidth ; ++y)
    {
        for (x = 0 ; x < borderWidth ; ++x)
        {
            dest.setPixelColor(x, y, topColor);
        }

        for (x = (int)dest.width() - 1 ; x > (int)dest.width() - borderWidth - 1 ; --x)
        {
            dest.setPixelColor(x, y, btmColor);
        }
    }

    // bottom

    for (wc = borderWidth ; y < (int)dest.height() ; ++y, --wc)
    {
        for (x = 0 ; x < wc ; ++x)
        {
            dest.setPixelColor(x, y, topColor);
        }

        for ( ; x < (int)dest.width() ; ++x)
        {
            dest.setPixelColor(x, y, btmColor);
        }
    }

    dest.bitBltImage(&src, borderWidth, borderWidth);
}

void BorderFilter::pattern2(const DImg& src, DImg& dest, int borderWidth,
                            const DColor& firstColor, const DColor& secondColor,
                            int firstWidth, int secondWidth)
{
    // Border tile.

    int w = d->settings.orgWidth + borderWidth * 2;
    int h = d->settings.orgHeight + borderWidth * 2;

    qCDebug(DIGIKAM_DIMG_LOG) << "Border File:" << d->settings.borderPath;
    DImg border(d->settings.borderPath);

    if (border.isNull())
    {
        return;
    }

    DImg borderImg(w, h, src.sixteenBit(), src.hasAlpha());
    border.convertToDepthOfImage(&borderImg);

    for (int x = 0 ; x < w ; x += border.width())
    {
        for (int y = 0 ; y < h ; y += border.height())
        {
            borderImg.bitBltImage(&border, x, y);
        }
    }

    // First line around the pattern tile.

    DImg tmp = borderImg.smoothScale(src.width()  + borderWidth * 2,
                                     src.height() + borderWidth * 2);

    solid2(tmp, dest, firstColor, firstWidth);

    // Second line around original image.

    tmp.reset();
    solid2(src, tmp, secondColor, secondWidth);

    // Copy original image.

    dest.bitBltImage(&tmp, borderWidth, borderWidth);
}

static QString colorToString(const QColor& c)
{
    if (c.alpha() != 255)
    {
        return QString::fromLatin1("rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
    }
    else
    {
        return QString::fromLatin1("rgba(%1,%2,%3,%4)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    }
}

static QColor stringToColor(const QString& s)
{
    QRegularExpression regexp(QRegularExpression::anchoredPattern(QLatin1String("(rgb|rgba)\\s*\\((.+)\\)\\s*")));
    QRegularExpressionMatch match = regexp.match(s);

    if (match.hasMatch())
    {
        QStringList colors = match.captured(1).split(QLatin1Char(','), Qt::SkipEmptyParts);

        if (colors.size() >= 3)
        {
            QColor c(colors.at(0).toInt(), colors.at(1).toInt(), colors.at(2).toInt());

            if (match.captured(0) == QLatin1String("rgba") && colors.size() == 4)
            {
                c.setAlpha(colors.at(3).toInt());
            }

            return c;
        }
    }

    return QColor();
}

FilterAction BorderFilter::filterAction()
{
    FilterAction action(FilterIdentifier(), CurrentVersion());
    action.setDisplayableName(DisplayableName());

    action.addParameter(QLatin1String("borderPath"),            d->settings.borderPath);
    action.addParameter(QLatin1String("borderPercent"),         d->settings.borderPercent);
    action.addParameter(QLatin1String("borderType"),            d->settings.borderType);
    action.addParameter(QLatin1String("borderWidth1"),          d->settings.borderWidth1);
    action.addParameter(QLatin1String("borderWidth2"),          d->settings.borderWidth2);
    action.addParameter(QLatin1String("borderWidth3"),          d->settings.borderWidth3);
    action.addParameter(QLatin1String("borderWidth4"),          d->settings.borderWidth4);
    action.addParameter(QLatin1String("preserveAspectRatio"),   d->settings.preserveAspectRatio);
    action.addParameter(QLatin1String("orgHeight"),             d->settings.orgHeight);
    action.addParameter(QLatin1String("orgWidth"),              d->settings.orgWidth);

    action.addParameter(QLatin1String("solidColor"),            colorToString(d->settings.solidColor));
    action.addParameter(QLatin1String("niepceBorderColor"),     colorToString(d->settings.niepceBorderColor));
    action.addParameter(QLatin1String("niepceLineColor"),       colorToString(d->settings.niepceLineColor));
    action.addParameter(QLatin1String("bevelUpperLeftColor"),   colorToString(d->settings.bevelUpperLeftColor));
    action.addParameter(QLatin1String("bevelLowerRightColor"),  colorToString(d->settings.bevelLowerRightColor));
    action.addParameter(QLatin1String("decorativeFirstColor"),  colorToString(d->settings.decorativeFirstColor));
    action.addParameter(QLatin1String("decorativeSecondColor"), colorToString(d->settings.decorativeSecondColor));

    return action;
}

void BorderFilter::readParameters(const FilterAction& action)
{
    d->settings.borderPath            = action.parameter(QLatin1String("borderPath")).toString();
    d->settings.borderPercent         = action.parameter(QLatin1String("borderPercent")).toDouble();
    d->settings.borderType            = action.parameter(QLatin1String("borderType")).toInt();
    d->settings.borderWidth1          = action.parameter(QLatin1String("borderWidth1")).toInt();
    d->settings.borderWidth2          = action.parameter(QLatin1String("borderWidth2")).toInt();
    d->settings.borderWidth3          = action.parameter(QLatin1String("borderWidth3")).toInt();
    d->settings.borderWidth4          = action.parameter(QLatin1String("borderWidth4")).toInt();
    d->settings.preserveAspectRatio   = action.parameter(QLatin1String("preserveAspectRatio")).toBool();
    d->settings.orgHeight             = action.parameter(QLatin1String("orgHeight")).toInt();
    d->settings.orgWidth              = action.parameter(QLatin1String("orgWidth")).toInt();

    d->settings.solidColor            = stringToColor(action.parameter(QLatin1String("solidColor")).toString());
    d->settings.niepceBorderColor     = stringToColor(action.parameter(QLatin1String("niepceBorderColor")).toString());
    d->settings.niepceLineColor       = stringToColor(action.parameter(QLatin1String("niepceLineColor")).toString());
    d->settings.bevelUpperLeftColor   = stringToColor(action.parameter(QLatin1String("bevelUpperLeftColor")).toString());
    d->settings.bevelLowerRightColor  = stringToColor(action.parameter(QLatin1String("bevelLowerRightColor")).toString());
    d->settings.decorativeFirstColor  = stringToColor(action.parameter(QLatin1String("decorativeFirstColor")).toString());
    d->settings.decorativeSecondColor = stringToColor(action.parameter(QLatin1String("decorativeSecondColor")).toString());
}

} // namespace Digikam

#include "moc_borderfilter.cpp"
