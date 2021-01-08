/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-28-04
 * Description : first run assistant dialog
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FIRST_RUN_DLG_H
#define DIGIKAM_FIRST_RUN_DLG_H

// Qt includes

#include <QString>
#include <QWidget>
#include <QWizard>

// Local includes

#include "digikam_export.h"
#include "dbengineparameters.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FirstRunDlg : public QWizard
{
    Q_OBJECT

public:

    explicit FirstRunDlg(QWidget* const parent = nullptr);
    ~FirstRunDlg() override;

    QString firstAlbumPath() const;
    DbEngineParameters getDbEngineParameters() const;

private:

    bool validateCurrentPage() override;

private Q_SLOTS:

    void slotFinishPressed();
    void slotHelp();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FIRST_RUN_DLG_H
