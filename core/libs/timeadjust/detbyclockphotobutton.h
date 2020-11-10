/* ============================================================
 *
 * This file is a part of the digikam project
 * https://www.digikam.org
 *
 * Date        : 2017-01-01
 * Description : button for choosing time difference photo which accepts drag & drop
 *
 * Copyright (C) 2017 by Markus Leuthold <kusi at forum dot titlis dot org>
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

#ifndef DIGIKAM_DET_BY_CLOCK_PHOTO_BUTTON_H
#define DIGIKAM_DET_BY_CLOCK_PHOTO_BUTTON_H

// Qt includes

#include <QPushButton>
#include <QDragEnterEvent>

namespace Digikam
{

class DetByClockPhotoButton : public QPushButton
{
    Q_OBJECT

public:

    explicit DetByClockPhotoButton(const QString& text);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event)   override;
    void dropEvent(QDropEvent* event)           override;

Q_SIGNALS:

    void signalClockPhotoDropped(const QUrl&);

private:

    // Disable
    DetByClockPhotoButton(QWidget*);
};

} // namespace Digikam

#endif // DIGIKAM_DET_BY_CLOCK_PHOTO_BUTTON_H
