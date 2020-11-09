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

#include "detbyclockphotobutton.h"

// Qt includes

#include <QMimeData>

// local includes

#include "digikam_debug.h"

namespace Digikam
{

DetByClockPhotoButton::DetByClockPhotoButton(const QString& text)
    : QPushButton(text)
{
    setAcceptDrops(true);
}

void DetByClockPhotoButton::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void DetByClockPhotoButton::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void DetByClockPhotoButton::dropEvent(QDropEvent* event)
{
    const QMimeData* const mimeData = event->mimeData();

    if (mimeData && mimeData->hasUrls())
    {
        // if user dropped more than one image, take the first one

        QUrl url = mimeData->urls().constFirst();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Dropped clock photo:" << url;
        emit signalClockPhotoDropped(url);
    }
}

} // namespace Digikam
