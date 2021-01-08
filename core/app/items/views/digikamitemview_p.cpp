/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-10-03
 * Description : Private Qt model-view for items
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Johannes Wienke <languitar at semipol do de>
 *
 * This program is free software you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "digikamitemview_p.h"

// Local includes

#include "contextmenuhelper.h"

namespace Digikam
{

DigikamItemView::Private::Private(DigikamItemView* const qq)
    : utilities          (nullptr),
      normalDelegate     (nullptr),
      faceDelegate       (nullptr),
      overlaysActive     (false),
      fullscreenActive   (false),
      rotateLeftOverlay  (nullptr),
      rotateRightOverlay (nullptr),
      fullscreenOverlay  (nullptr),
      faceMode           (false),
      q_ptr              (qq)
{
}

DigikamItemView::Private::~Private()
{
}

void DigikamItemView::Private::updateOverlays()
{
    Q_Q(DigikamItemView);

    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (overlaysActive)
    {
        if (!settings->getIconShowOverlays())
        {
            disconnect(rotateLeftOverlay, SIGNAL(signalRotate(QList<QModelIndex>)),
                       q, SLOT(slotRotateLeft(QList<QModelIndex>)));

            disconnect(rotateRightOverlay, SIGNAL(signalRotate(QList<QModelIndex>)),
                       q, SLOT(slotRotateRight(QList<QModelIndex>)));

            q->removeOverlay(rotateLeftOverlay);
            q->removeOverlay(rotateRightOverlay);

            overlaysActive = false;
        }
    }
    else
    {
        if (settings->getIconShowOverlays())
        {
            q->addOverlay(rotateLeftOverlay, normalDelegate);
            q->addOverlay(rotateRightOverlay, normalDelegate);

            connect(rotateLeftOverlay, SIGNAL(signalRotate(QList<QModelIndex>)),
                    q, SLOT(slotRotateLeft(QList<QModelIndex>)));

            connect(rotateRightOverlay, SIGNAL(signalRotate(QList<QModelIndex>)),
                    q, SLOT(slotRotateRight(QList<QModelIndex>)));

            overlaysActive = true;
        }
    }

    if (fullscreenActive)
    {
        if (!settings->getIconShowFullscreen())
        {
            disconnect(fullscreenOverlay, SIGNAL(signalRotate(QList<QModelIndex>)),
                       q, SLOT(slotFullscreen(QList<QModelIndex>)));

            q->removeOverlay(fullscreenOverlay);

            fullscreenActive = false;
        }
    }
    else
    {
        if (settings->getIconShowFullscreen())
        {
            fullscreenActive = true;

            q->addOverlay(fullscreenOverlay, normalDelegate);

            connect(fullscreenOverlay, SIGNAL(signalFullscreen(QList<QModelIndex>)),
                    q, SLOT(slotFullscreen(QList<QModelIndex>)));
        }
    }
}

} // namespace Digikam
