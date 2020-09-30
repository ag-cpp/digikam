/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-01
 * Description : a tool to export items to Google web services
 *
 * Copyright (C) 2010      by Jens Mueller <tschenser at gmx dot de>
 * Copyright (C) 2013-2018 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_GS_NEW_ALBUM_DLG_H
#define DIGIKAM_GS_NEW_ALBUM_DLG_H

// Qt includes

#include <QRadioButton>
#include <QDateTimeEdit>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCloseEvent>

// Local includes

#include "wsnewalbumdialog.h"

using namespace Digikam;

namespace DigikamGenericGoogleServicesPlugin
{

class GSFolder;

class GSNewAlbumDlg : public WSNewAlbumDialog
{
    Q_OBJECT

public:

    explicit GSNewAlbumDlg(QWidget* const parent,
                           const QString& serviceName,
                           const QString& toolName);
    ~GSNewAlbumDlg() override;

    void getAlbumProperties(GSFolder& album);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericGoogleServicesPlugin

#endif // DIGIKAM_GS_NEW_ALBUM_DLG_H
