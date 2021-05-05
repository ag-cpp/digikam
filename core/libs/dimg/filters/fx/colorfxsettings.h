/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-11-08
 * Description : Color effects settings view.
 *
 * Copyright (C) 2012 by Alexander Dymo <adymo at kdevelop dot org>
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

#ifndef DIGIKAM_COLOR_FX_SETTINGS_H
#define DIGIKAM_COLOR_FX_SETTINGS_H

// Local includes

#include <QWidget>

// Local includes

#include "digikam_export.h"
#include "colorfxfilter.h"

class  KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT ColorFXSettings : public QWidget
{
    Q_OBJECT

public:

    explicit ColorFXSettings(QWidget* const parent, bool useGenericImg = true);
    ~ColorFXSettings() override;

    ColorFXContainer defaultSettings()          const;
    void resetToDefault();

    ColorFXContainer settings()                 const;
    void setSettings(const ColorFXContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

    void startPreviewFilters();

Q_SIGNALS:

    void signalSettingsChanged();

private:

    void findLuts();
    QString translateLuts(const QString& name)  const;

private Q_SLOTS:

    void slotEffectTypeChanged(int type);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_COLOR_FX_SETTINGS_H
