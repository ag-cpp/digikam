/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-21
 * Description : setup tab for slideshow options.
 *
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_SETUP_SLIDESHOW_DIALOG_H
#define DIGIKAM_SETUP_SLIDESHOW_DIALOG_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace DigikamGenericSlideShowPlugin
{

class DIGIKAM_EXPORT SetupSlideShowDialog : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupSlideShowDialog(QWidget* const parent = nullptr);
    ~SetupSlideShowDialog();

    void applySettings();

public Q_SLOTS:

    void slotSetUnchecked(int);

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SETUP_SLIDESHOW_DIALOG_H
