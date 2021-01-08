/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-05-03
 * Description : DNG convert settings for camera interface.
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2016 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_DNG_CONVERT_SETTINGS_H
#define DIGIKAM_DNG_CONVERT_SETTINGS_H

// Qt includes

#include <QWidget>

// Local settings

#include "downloadsettings.h"

class KConfigGroup;

namespace Digikam
{

class DNGConvertSettings : public QWidget
{
    Q_OBJECT

public:

    explicit DNGConvertSettings(QWidget* const parent = nullptr);
    ~DNGConvertSettings() override;

    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

    void settings(DownloadSettings* const settings);

Q_SIGNALS:

    void signalDownloadNameChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DNG_CONVERT_SETTINGS_H
