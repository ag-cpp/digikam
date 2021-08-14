#ifndef DIGIKAM_FOCUSPOINT_ITEM_H
#define DIGIKAM_FOCUSPOINT_ITEM_H

// Qt includes

#include <QObject>
#include <QGraphicsWidget>

// Local includes
/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Face tag region item
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

#include "regionframeitem.h"
#include "focuspoint.h"

#include <QLabel>

namespace Digikam
{

class HidingStateChanger;

class Q_DECL_HIDDEN FocusPointItem : public RegionFrameItem       // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:

    explicit FocusPointItem(QGraphicsItem* const parent);
    ~FocusPointItem() override;

    void setPoint(const FocusPoint& point);
    FocusPoint point()                                const;
    void setEditable(bool allowEdit);

protected:

    FocusPoint          m_point;
    QColor              m_color;
    QLabel*             m_widgetName;
    float               m_width;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

private:

    // Disable
    FocusPointItem()                                 = delete;
    FocusPointItem(const FocusPointItem&)            = delete;
    FocusPointItem& operator=(const FocusPointItem&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINT_ITEM_H
