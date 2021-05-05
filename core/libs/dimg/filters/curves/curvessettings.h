/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-01
 * Description : Curves settings view.
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

#ifndef DIGIKAM_CURVES_SETTINGS_H
#define DIGIKAM_CURVES_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "curvesfilter.h"
#include "curveswidget.h"
#include "curvesbox.h"
#include "dimg.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT CurvesSettings : public QWidget
{
    Q_OBJECT

public:

    explicit CurvesSettings(QWidget* const parent, DImg* const img);
    ~CurvesSettings() override;

    CurvesContainer defaultSettings() const;
    void resetToDefault();

    CurvesContainer settings()        const;
    void setSettings(const CurvesContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

    void setScale(HistogramScale type);
    void setCurrentChannel(ChannelType channel);

    int curvesLeftOffset() const;

Q_SIGNALS:

    void signalSettingsChanged();
    void signalSpotColorChanged();
    void signalChannelReset(int);
    void signalPickerChanged(int);

public Q_SLOTS:

    void slotSpotColorChanged(const Digikam::DColor& color);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_CURVES_SETTINGS_H
