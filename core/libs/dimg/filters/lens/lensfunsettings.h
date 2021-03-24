/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * Copyright (C) 2008      by Adrian Schroeter <adrian at suse dot de>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LENS_FUN_SETTINGS_H
#define DIGIKAM_LENS_FUN_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "lensfunfilter.h"
#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT LensFunSettings : public QWidget
{
    Q_OBJECT

public:

    explicit LensFunSettings(QWidget* const parent = nullptr);
    ~LensFunSettings() override;

    void setEnabledCCA(bool b);
    void setEnabledVig(bool b);
    void setEnabledDist(bool b);
    void setEnabledGeom(bool b);

    LensFunContainer defaultSettings() const;
    LensFunContainer settings()        const;
    void resetToDefault();

    void assignFilterSettings(LensFunContainer& prm);
    void setFilterSettings(const LensFunContainer& settings);

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LENS_FUN_SETTINGS_H
