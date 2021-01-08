/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-01-12
 * Description : exposure indicator settings container.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EXPOSURE_CONTAINER_H
#define DIGIKAM_EXPOSURE_CONTAINER_H

// Qt includes

#include <QColor>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExposureSettingsContainer
{

public:

    explicit ExposureSettingsContainer()
        : underExposureIndicator(false),
          overExposureIndicator(false),
          exposureIndicatorMode(true),
          underExposurePercent(1.0),
          overExposurePercent(1.0),
          underExposureColor(Qt::white),
          overExposureColor(Qt::black)
    {
    };

    virtual ~ExposureSettingsContainer()
    {
    };

public:

    bool   underExposureIndicator;
    bool   overExposureIndicator;

    /**
     * If this option is true, over and under exposure indicators will be displayed
     * only when pure white and pure black color matches, as all color components match
     * the condition in the same time.
     * Else indicators are turn on when one of color components match the condition.
     */
    bool   exposureIndicatorMode;

    float  underExposurePercent;
    float  overExposurePercent;

    QColor underExposureColor;
    QColor overExposureColor;

private:

    Q_DISABLE_COPY(ExposureSettingsContainer)
};

} // namespace Digikam

#endif // DIGIKAM_EXPOSURE_CONTAINER_H
