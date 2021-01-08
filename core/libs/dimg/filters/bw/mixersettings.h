/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-18
 * Description : Channel mixer settings view.
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

#ifndef DIGIKAM_MIXER_SETTINGS_H
#define DIGIKAM_MIXER_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "mixerfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT MixerSettings : public QWidget
{
    Q_OBJECT

public:

    explicit MixerSettings(QWidget* const parent);
    ~MixerSettings() override;

    MixerContainer defaultSettings() const;
    void resetToDefault();

    MixerContainer settings()        const;
    void setSettings(const MixerContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

    int  currentChannel() const;

    void setMonochromeTipsVisible(bool b);

Q_SIGNALS:

    void signalSettingsChanged();
    void signalMonochromeActived(bool);
    void signalOutChannelChanged();

private:

    void updateSettingsWidgets();
    void updateTotalPercents();

private Q_SLOTS:

    void slotResetCurrentChannel();
    void slotGainsChanged();
    void slotMonochromeActived(bool);
    void slotLuminosityChanged(bool);
    void slotOutChannelChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_MIXER_SETTINGS_H
