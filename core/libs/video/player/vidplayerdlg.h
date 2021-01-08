/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_VID_PLAYER_DLG_H
#define DIGIKAM_VID_PLAYER_DLG_H

// Qt include

#include <QDialog>
#include <QWidget>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT VidPlayerDlg : public QDialog
{
    Q_OBJECT

public:

    explicit VidPlayerDlg(const QString& file, QWidget* const parent = nullptr);
    ~VidPlayerDlg() override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_VID_PLAYER_DLG_H
