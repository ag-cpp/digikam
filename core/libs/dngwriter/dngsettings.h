/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-24
 * Description : DNG save settings widgets
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DNG_SETTINGS_H
#define DIGIKAM_DNG_SETTINGS_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DNGSettings : public QWidget
{
    Q_OBJECT

public:

    explicit DNGSettings(QWidget* const parent = nullptr);
    ~DNGSettings() override;

    void setCompressLossLess(bool b);
    bool compressLossLess()         const;

    void setPreviewMode(int mode);
    int  previewMode()              const;

    void setBackupOriginalRawFile(bool b);
    bool backupOriginalRawFile()    const;

    void setDefaultSettings();

public Q_SLOTS:

    /// To handle changes from host application Setup dialog.
    void slotSetupChanged();

Q_SIGNALS:

    void signalSettingsChanged();
    void signalSetupExifTool();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DNG_SETTINGS_H
