/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "ScreenGraphicsItem.h"
#include "ScreenGraphicsItem_p.h"

// Qt includes

#include <QMouseEvent>

// Local includes

#include "MarbleWidget.h"
#include "digikam_debug.h"

namespace Marble
{

ScreenGraphicsItem::ScreenGraphicsItem(MarbleGraphicsItem* parent)
    : MarbleGraphicsItem(new ScreenGraphicsItemPrivate(this, parent))
{
}

ScreenGraphicsItem::ScreenGraphicsItem(ScreenGraphicsItemPrivate* dd)
    : MarbleGraphicsItem(dd)
{
}

ScreenGraphicsItem::~ScreenGraphicsItem()
{
}

QPointF ScreenGraphicsItem::position() const
{
    Q_D(const ScreenGraphicsItem);
    return d->m_position;
}

void ScreenGraphicsItem::setPosition(const QPointF& position)
{
    Q_D(ScreenGraphicsItem);
    d->m_position = position;
}

QPointF ScreenGraphicsItem::positivePosition() const
{
    Q_D(const ScreenGraphicsItem);
    return d->positivePosition();
}

QVector<QPointF> ScreenGraphicsItem::absolutePositions() const
{
    Q_D(const ScreenGraphicsItem);
    return d->absolutePositions();
}

ScreenGraphicsItem::GraphicsItemFlags ScreenGraphicsItem::flags() const
{
    Q_D(const ScreenGraphicsItem);
    return d->m_flags;
}

void ScreenGraphicsItem::setFlags(ScreenGraphicsItem::GraphicsItemFlags flags)
{
    Q_D(ScreenGraphicsItem);
    d->m_flags = flags;
}

bool ScreenGraphicsItem::eventFilter(QObject* object, QEvent* e)
{
    MarbleWidget* widget = dynamic_cast<MarbleWidget*>(object);

    if (!widget)
    {
        return MarbleGraphicsItem::eventFilter(object, e);
    }

    Q_D(ScreenGraphicsItem);

    if (!d->m_floatItemMoving)
    {
        if (MarbleGraphicsItem::eventFilter(object, e))
        {
            return true;
        }

        if (!visible() || !d->isMovable())
        {
            return false;
        }

        if (e->type() == QEvent::MouseMove)
        {
            return false;
        }

        // Move ScreenGraphicsItem
        if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* event = static_cast<QMouseEvent*>(e);

            // Click and move above a float item triggers moving the float item
            if (contains(event->pos()))
            {
                if (event->button() == Qt::LeftButton)
                {
                    d->m_floatItemMoveStartPos = event->pos();
                    d->m_floatItemMoving = true;
                    return true;
                }
            }
        }

        return false;
    }

    else
    {
        // Move ScreenGraphicsItem
        if (e->type() == QEvent::MouseMove
            || e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* event = static_cast<QMouseEvent*>(e);
            // The rect the item was painted on before. We add one pixel as antialiasing could
            // result into painting on these pixels to.
            QRectF floatItemRect = QRectF(positivePosition() - QPoint(1, 1),
                                          size() + QSize(2, 2));

            if (e->type() == QEvent::MouseMove && event->buttons() & Qt::LeftButton)
            {
                const QPoint& point = event->pos();
                QPointF position = positivePosition();
                qreal newX = qMax<qreal>(0, position.x() + point.x() - d->m_floatItemMoveStartPos.x());
                qreal newY = qMax<qreal>(0, position.y() + point.y() - d->m_floatItemMoveStartPos.y());

                // docking behavior
                const qreal dockArea = 60.0; // Alignment area width/height
                const qreal dockJump = 30.0; // Alignment indicator jump size

                if (widget->width() - size().width() - newX < dockArea)
                {
                    newX = qMin(qreal(-1.0), size().width() + newX - widget->width());

                    if (d->m_floatItemMoveStartPos.x() < event->pos().x())
                    {
                        // Indicate change to right alignment with a short jump
                        newX = qMax(newX, -(dockArea - dockJump));
                    }
                }

                if (widget->height() - size().height() - newY < dockArea)
                {
                    newY = qMin(qreal(-1.0), size().height() + newY - widget->height());

                    if (d->m_floatItemMoveStartPos.y() < event->pos().y())
                    {
                        // Indicate change to bottom alignment with a short jump
                        newY = qMax(newY, -(dockArea - dockJump));
                    }
                }

                setPosition(QPointF(newX, newY));
                // The rect the item will be painted on now. We add one pixel as
                // antialiasing could result into painting on these pixels to.
                QRect newFloatItemRect = QRectF(positivePosition() - QPoint(1, 1),
                                                size() + QSize(2, 2)).toRect();
                d->m_floatItemMoveStartPos = event->pos();
                QRegion dirtyRegion(floatItemRect.toRect());
                dirtyRegion = dirtyRegion.united(newFloatItemRect);

                widget->setAttribute(Qt::WA_NoSystemBackground,  false);
                widget->update(dirtyRegion);
                widget->setAttribute(Qt::WA_NoSystemBackground, widget->viewport()->mapCoversViewport());
                return true;
            }

            if (e->type() == QEvent::MouseButtonRelease)
            {
                d->m_floatItemMoving = false;
            }

            // Use a special cursor as long as the item is moved
            if (d->m_floatItemMoving)
            {
                widget->setCursor(QCursor(Qt::SizeAllCursor));
                return true;
            }
        }

        return MarbleGraphicsItem::eventFilter(object, e);
    }
}

} // namespace Marble
