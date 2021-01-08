/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : a full screen settings widget
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FULL_SCREEN_SETTINGS_H
#define DIGIKAM_FULL_SCREEN_SETTINGS_H

// Qt includes

#include <QGroupBox>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT FullScreenSettings : public QGroupBox
{
    Q_OBJECT

public:

    explicit FullScreenSettings(int options, QWidget* const parent);
    ~FullScreenSettings() override;

    void readSettings(const KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FULL_SCREEN_SETTINGS_H
