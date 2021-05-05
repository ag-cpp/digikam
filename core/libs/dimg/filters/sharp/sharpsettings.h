/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-10
 * Description : sharp settings view.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SHARP_SETTINGS_H
#define DIGIKAM_SHARP_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT SharpContainer
{

public:

    enum SharpingMethods
    {
        SimpleSharp = 0,
        UnsharpMask,
        Refocus
    };

public:

    explicit SharpContainer()
      : method(SimpleSharp),
        ssRadius(0),
        umRadius(1.0),
        umAmount(1.0),
        umThreshold(0.05),
        umLumaOnly(false),
        rfRadius(1.0),
        rfCorrelation(0.5),
        rfNoise(0.03),
        rfGauss(0.0),
        rfMatrix(5)
    {
    };

    ~SharpContainer()
    {
    };

public:

    int    method;      ///< Store SharpingMethods value

    /// Simple sharp
    int    ssRadius;

    /// Unsharp mask
    double umRadius;
    double umAmount;
    double umThreshold;
    bool   umLumaOnly;

    /// Refocus
    double rfRadius;
    double rfCorrelation;
    double rfNoise;
    double rfGauss;
    int    rfMatrix;
};

// -----------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT SharpSettings : public QWidget
{
    Q_OBJECT

public:

    explicit SharpSettings(QWidget* const parent);
    ~SharpSettings() override;

    SharpContainer defaultSettings() const;
    void resetToDefault();

    SharpContainer settings()        const;
    void setSettings(const SharpContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotSharpMethodChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SHARP_SETTINGS_H
