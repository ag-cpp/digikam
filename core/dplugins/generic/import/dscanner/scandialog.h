/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-09-09
 * Description : scanner dialog
 *
 * Copyright (C) 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SCAN_DIALOG_H
#define DIGIKAM_SCAN_DIALOG_H

// Qt includes

#include <QCloseEvent>
#include <QWidget>
#include <QImage>

// KDE include

#include <ksanewidget.h>

#if (QT_VERSION < QT_VERSION_CHECK(5, 99, 0))
#   include <ksane_version.h>
#endif

// Local includes

#include "dplugindialog.h"

using namespace Digikam;
using namespace KSaneIface;

namespace DigikamGenericDScannerPlugin
{

class ScanDialog : public DPluginDialog
{
    Q_OBJECT

public:

    explicit ScanDialog(KSaneWidget* const saneWdg, QWidget* const parent = nullptr);
    ~ScanDialog()                   override;

    void setTargetDir(const QString& targetDir);

protected:

    void closeEvent(QCloseEvent*)   override;

Q_SIGNALS:

    void signalImportedImage(const QUrl&);

private Q_SLOTS:

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

    void slotSaveImage(const QImage&);

#elif KSANE_VERSION < QT_VERSION_CHECK(21,8,0)

    void slotSaveImage(QByteArray&, int, int, int, int);

#else

    void slotSaveImage(const QImage&);

#endif

    void slotThreadProgress(const QUrl&, int);
    void slotThreadDone(const QUrl&, bool);
    void slotDialogFinished();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericDScannerPlugin

#endif // DIGIKAM_SCAN_DIALOG_H
