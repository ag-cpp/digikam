/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-31
 * Description : Figure out camera clock delta from a clock picture.
 *
 * Copyright (C) 2009      by Pieter Edelman <p dot edelman at gmx dot net>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CLOCK_PHOTO_DIALOG_H
#define DIGIKAM_CLOCK_PHOTO_DIALOG_H

// Qt includes

#include <QUrl>
#include <QDialog>

namespace Digikam
{

class DeltaTime;

class ClockPhotoDialog : public QDialog
{
    Q_OBJECT

public:

    explicit ClockPhotoDialog(QWidget* const parent, const QUrl& defaultUrl);
    ~ClockPhotoDialog() override;

    /**
     * Try to load the photo specified by the QUrl, and set the datetime widget
     * to the photo time. Return true on success, or false if either the photo
     * can't be read or the datetime information can't be read.
     */
    bool setImage(const QUrl&);

    DeltaTime deltaValues() const;

private Q_SLOTS:

    void slotLoadPhoto();
    void slotOk();

    /**
     * If the cancel button is clicked, reject the dialog.
     */
    void slotCancel();

private:

    void loadSettings();
    void saveSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_CLOCK_PHOTO_DIALOG_H
