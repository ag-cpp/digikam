/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QColor>

// Local includes

#include "digikam_export.h"

namespace Marble
{

class MarbleClock;
class SunLocatorPrivate;
class Planet;

class DIGIKAM_EXPORT SunLocator : public QObject
{
    Q_OBJECT

public:

    SunLocator(const MarbleClock* clock, const Planet* planet);
    ~SunLocator() override;

    qreal shading(qreal lon, qreal a, qreal c) const;
    static void shadePixel(QRgb& pixcol, qreal shade);
    static void shadePixelComposite(QRgb& pixcol, const QRgb& dpixcol, qreal shade);

    void  setPlanet(const Planet* planet);

    qreal getLon() const;
    qreal getLat() const;

public Q_SLOTS:

    void update();

Q_SIGNALS:

    void positionChanged(qreal lon, qreal lat);

private:

    Q_DISABLE_COPY(SunLocator)

private:

    SunLocatorPrivate* const d = nullptr;
};

} // Namespace Marble
