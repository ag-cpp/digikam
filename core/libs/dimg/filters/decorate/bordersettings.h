/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-17
 * Description : Border settings view.
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

#ifndef DIGIKAM_BORDER_SETTINGS_H
#define DIGIKAM_BORDER_SETTINGS_H

// Local includes

#include <QWidget>
#include <QColor>

// Local includes

#include "digikam_export.h"
#include "borderfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT BorderSettings : public QWidget
{
    Q_OBJECT

public:

    explicit BorderSettings(QWidget* const parent);
    ~BorderSettings() override;

    BorderContainer defaultSettings() const;
    void resetToDefault();

    BorderContainer settings()        const;
    void setSettings(const BorderContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotBorderTypeChanged(int borderType);
    void slotPreserveAspectRatioToggled(bool);
    void slotColorForegroundChanged(const QColor& color);
    void slotColorBackgroundChanged(const QColor& color);

private:

    void toggleBorderSlider(bool b);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BORDER_SETTINGS_H
