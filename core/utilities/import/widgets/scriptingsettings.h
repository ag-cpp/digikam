/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-07-20
 * Description : scripting settings for camera interface.
 *
 * Copyright (C) 2012 by Petri Damstén <damu@iki.fi>
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

#ifndef DIGIKAM_SCRIPTING_SETTINGS_H
#define DIGIKAM_SCRIPTING_SETTINGS_H

// Qt includes

#include <QWidget>
#include <QDateTime>

// Local settings

#include "downloadsettings.h"
#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_GUI_EXPORT ScriptingSettings : public QWidget
{
    Q_OBJECT

public:

    explicit ScriptingSettings(QWidget* const parent = nullptr);
    ~ScriptingSettings() override;

    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

    void settings(DownloadSettings* const settings) const;

private Q_SLOTS:

    void slotToolTipButtonToggled(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SCRIPTING_SETTINGS_H
