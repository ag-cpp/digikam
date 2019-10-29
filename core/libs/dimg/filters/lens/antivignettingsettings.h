/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-15
 * Description : Anti vignetting settings view.
 *
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ANTI_VIGNETTING_SETTINGS_H
#define DIGIKAM_ANTI_VIGNETTING_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "antivignettingfilter.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT AntiVignettingSettings : public QWidget
{
    Q_OBJECT

public:

    explicit AntiVignettingSettings(QWidget* parent);
    ~AntiVignettingSettings();

    AntiVignettingContainer defaultSettings() const;
    void resetToDefault();

    AntiVignettingContainer settings() const;
    void setSettings(const AntiVignettingContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void setMaskPreviewPixmap(const QPixmap& pix);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ANTI_VIGNETTING_SETTINGS_H
