/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Image zoom settings
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagezoomsettings.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QList>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ImageZoomSettings::ImageZoomSettings(const QSize& imageSize, const QSize& originalSize)
{
    setImageSize(imageSize, originalSize);
}

void ImageZoomSettings::setImageSize(const QSize& size, const QSize& originalSize)
{
    m_size = size;

    if (!originalSize.isNull() && originalSize.isValid())
    {
        m_zoomConst = m_size.width() / double(originalSize.width());
    }
    else
    {
        m_zoomConst = 1.0;
    }
}

void ImageZoomSettings::setDisplayWidget(QWidget* const widget)
{
    m_displayWidget = widget;
}

double ImageZoomSettings::zoomFactor() const
{
    return m_zoom;
}

double ImageZoomSettings::realZoomFactor() const
{
    return (m_zoom / displayRatio());
}

QSizeF ImageZoomSettings::imageSize() const
{
    return m_size;
}

QSizeF ImageZoomSettings::originalImageSize() const
{
    return (m_size / m_zoomConst);
}

QSizeF ImageZoomSettings::zoomedSize() const
{
    return (m_size / (m_zoomConst * displayRatio()) * m_zoom);
}

QRectF ImageZoomSettings::sourceRect(const QRectF& imageRect) const
{
    return mapZoomToImage(imageRect);
}

bool ImageZoomSettings::isFitToSize(const QSizeF& frameSize) const
{
    return (zoomFactor() == fitToSizeZoomFactor(frameSize));
}

void ImageZoomSettings::setZoomFactor(double zoom)
{
    m_zoom = zoom;
}

void ImageZoomSettings::fitToSize(const QSizeF& frameSize, FitToSizeMode mode)
{
    setZoomFactor(fitToSizeZoomFactor(frameSize, mode));
}

double ImageZoomSettings::fitToSizeZoomFactor(const QSizeF& frameSize, FitToSizeMode mode) const
{
    if (!frameSize.isValid() || !m_size.isValid())
    {
        return 1.0;
    }

    double zoom;

    if ((frameSize.width() / frameSize.height()) < (m_size.width() / m_size.height()))
    {
        zoom = m_zoomConst * displayRatio() * frameSize.width() / m_size.width();
    }
    else
    {
        zoom = m_zoomConst * displayRatio() * frameSize.height() / m_size.height();
    }

    // Zoom rounding down and scroll bars are never activated.

    zoom = floor(zoom * 100000 - 0.1) / 100000.0;

    if (mode == OnlyScaleDown)
    {
        // OnlyScaleDown: accept that an image is smaller than available space, don't scale up

        if ((frameSize.width() > originalImageSize().width()) && (frameSize.height() > originalImageSize().height()))
        {
            zoom = 1.0;
        }
    }

    return zoom;
}

QRectF ImageZoomSettings::mapZoomToImage(const QRectF& zoomedRect) const
{
    return QRectF(zoomedRect.topLeft() / (m_zoom / (m_zoomConst * displayRatio())),
                  zoomedRect.size()    / (m_zoom / (m_zoomConst * displayRatio())));
}

QRectF ImageZoomSettings::mapImageToZoom(const QRectF& imageRect) const
{
    return QRectF(imageRect.topLeft() * (m_zoom / (m_zoomConst * displayRatio())),
                  imageRect.size()    * (m_zoom / (m_zoomConst * displayRatio())));
}

QPointF ImageZoomSettings::mapZoomToImage(const QPointF& zoomedPoint) const
{
    return (zoomedPoint / (m_zoom / (m_zoomConst * displayRatio())));
}

QPointF ImageZoomSettings::mapImageToZoom(const QPointF& imagePoint) const
{
    return (imagePoint * (m_zoom / (m_zoomConst * displayRatio())));
}

inline static bool lessThanLimitedPrecision(double a, double b)
{
    return (lround(a * 100000) < lround(b * 100000));
}

double ImageZoomSettings::snappedZoomStep(double nextZoom, const QSizeF& frameSize) const
{
    // If the zoom value gets changed from d->zoom to zoom
    // across 50%, 100% or fit-to-window, then return the
    // the corresponding special value. Otherwise zoom is returned unchanged.

    QList<double> snapValues;
    snapValues << 0.5;
    snapValues << 1.0;

    if (frameSize.isValid())
    {
        snapValues << fitToSizeZoomFactor(frameSize);
    }

    double currentZoom = zoomFactor();

    if (currentZoom < nextZoom)
    {
        for (double z : std::as_const(snapValues))
        {
            if (lessThanLimitedPrecision(currentZoom, z) && lessThanLimitedPrecision(z, nextZoom))
            {    // cppcheck-suppress useStlAlgorithm
                return z;
            }
        }
    }
    else
    {
        for (double z : std::as_const(snapValues))
        {
            if (lessThanLimitedPrecision(z, currentZoom) && lessThanLimitedPrecision(nextZoom, z))
            {   // cppcheck-suppress useStlAlgorithm
                return z;
            }
        }
    }

    return nextZoom;
}

double ImageZoomSettings::snappedZoomFactor(double zoom, const QSizeF& frameSize) const
{
    QList<double> snapValues;
    snapValues << 0.5;
    snapValues << 1.0;

    if (frameSize.isValid())
    {
        snapValues << fitToSizeZoomFactor(frameSize);
    }

    for (double z : std::as_const(snapValues))
    {
        if (fabs(zoom - z) < 0.05)
        {   // cppcheck-suppress useStlAlgorithm
            return z;
        }
    }

    return zoom;
}

double ImageZoomSettings::displayRatio() const
{
    if (m_displayWidget)
    {
        return m_displayWidget->devicePixelRatio();
    }

    qCWarning(DIGIKAM_GENERAL_LOG) << "ImageZoomSettings: display widget not set";

    return 1.0;
}

} // namespace Digikam
