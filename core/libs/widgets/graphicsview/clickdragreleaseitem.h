/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-04
 * Description : A simple item to click, drag and release
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_CLICK_DRAG_RELEASE_ITEM_H
#define DIGIKAM_CLICK_DRAG_RELEASE_ITEM_H

// Qt includes

#include <QGraphicsObject>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ClickDragReleaseItem : public QGraphicsObject      // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:

    explicit ClickDragReleaseItem(QGraphicsItem* const parent);
    ~ClickDragReleaseItem()                                          override;

    QRectF boundingRect() const                                      override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

Q_SIGNALS:

    /**
     * Signals are emitted at click, drag and release event.
     * Reported positions are in scene coordinates.
     * A drag is reported only if the mouse was moved a certain threshold.
     * A release is reported after every press.
     */

    void started(const QPointF& pos);
    void moving(const QRectF& rect);
    void finished(const QRectF& rect);
    void cancelled();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent*)                  override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*)                   override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*)                override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)            override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent*)                   override;
    void keyPressEvent(QKeyEvent*)                                   override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_CLICK_DRAG_RELEASE_ITEM_H
