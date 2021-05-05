/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-23
 * Description : black and white settings view.
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

#ifndef DIGIKAM_BW_SEPIA_SETTINGS_H
#define DIGIKAM_BW_SEPIA_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "bwsepiafilter.h"
#include "dimg.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT BWSepiaSettings : public QWidget
{
    Q_OBJECT

public:

    explicit BWSepiaSettings(QWidget* const parent, DImg* const img);
    ~BWSepiaSettings() override;

    BWSepiaContainer defaultSettings() const;
    void resetToDefault();

    BWSepiaContainer settings()        const;
    void setSettings(const BWSepiaContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void loadSettings();
    void saveAsSettings();

    void setScaleType(HistogramScale scale);

    void startPreviewFilters();

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotFilterSelected();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BW_SEPIA_SETTINGS_H
