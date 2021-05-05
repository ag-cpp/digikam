/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-26
 * Description : White Balance settings view.
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

#ifndef DIGIKAM_WB_SETTINGS_H
#define DIGIKAM_WB_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "wbfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT WBSettings : public QWidget
{
    Q_OBJECT

public:

    explicit WBSettings(QWidget* const parent);
    ~WBSettings() override;

    WBContainer defaultSettings() const;
    void resetToDefault();

    WBContainer settings()        const;
    void setSettings(const WBContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

    bool pickTemperatureIsChecked();
    void setCheckedPickTemperature(bool b);

    void showAdvancedButtons(bool b);

Q_SIGNALS:

    void signalSettingsChanged();
    void signalPickerColorButtonActived();
    void signalAutoAdjustExposure();

private Q_SLOTS:

    void slotTemperatureChanged(double temperature);
    void slotTemperaturePresetChanged(int tempPreset);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_WB_SETTINGS_H
