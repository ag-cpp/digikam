/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : Online version dialog.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ONLINE_VERSION_DLG_H
#define DIGIKAM_ONLINE_VERSION_DLG_H

// Qt includes

#include <QDialog>

// Local includes

#include "digikam_export.h"
#include "digikam_version.h"

namespace Digikam
{

class DIGIKAM_EXPORT OnlineVersionDlg : public QDialog
{
    Q_OBJECT

public:

    explicit OnlineVersionDlg(QWidget* const parent = nullptr,
                              const QString& version = QLatin1String(digikam_version_short));
    ~OnlineVersionDlg() override;

private Q_SLOTS:

    void slotNewVersionAvailable(const QString& version);
    void slotNewVersionCheckError(const QString& error);

    void slotDownloadInstaller();
    void slotDownloadError(const QString& error);
    void slotDownloadProgress(qint64, qint64);

    void slotRunInstaller();
    void slotOpenInFileManager();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ONLINE_VERSION_DLG_H
