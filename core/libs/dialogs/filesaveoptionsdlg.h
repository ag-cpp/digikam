/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-14
 * Description : a dialog to display image file save options.
 *
 * Copyright (C)      2009 by David Eriksson <meldavid at acc umu se>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FILE_SAVE_OPTIONS_DLG_H
#define DIGIKAM_FILE_SAVE_OPTIONS_DLG_H

// Qt includes

#include <QDialog>

// Local includes

#include "digikam_export.h"
#include "filesaveoptionsbox.h"

class QWidget;

namespace Digikam
{

class DIGIKAM_EXPORT FileSaveOptionsDlg : public QDialog
{
    Q_OBJECT

public:

    FileSaveOptionsDlg(QWidget* const parent, FileSaveOptionsBox* const options);
    ~FileSaveOptionsDlg();
};

} // namespace Digikam

#endif // DIGIKAM_FILE_SAVE_OPTIONS_DLG_H
