/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-30
 * Description : rating icon view item at mouse hover
 *
 * Copyright (C) 2009      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_RATING_OVERLAY_H
#define DIGIKAM_ITEM_RATING_OVERLAY_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "itemdelegateoverlay.h"
#include "itemviewdelegate.h"

namespace Digikam
{

class RatingWidget;

class ItemRatingOverlay : public AbstractWidgetDelegateOverlay
{
    Q_OBJECT
    REQUIRE_DELEGATE(ItemViewDelegate)

public:

    explicit ItemRatingOverlay(QObject* const parent);

    RatingWidget* ratingWidget() const;

Q_SIGNALS:

    void ratingEdited(const QList<QModelIndex>& indexes, int rating);

protected Q_SLOTS:

    void slotRatingChanged(int);
    void slotDataChanged(const QModelIndex&, const QModelIndex&);

protected:

    void updatePosition();
    void updateRating();

protected:

    QWidget* createWidget()                    override;
    void setActive(bool)                       override;
    void visualChange()                        override;
    void hide()                                override;
    void slotEntered(const QModelIndex& index) override;
    void widgetEnterEvent()                    override;
    void widgetLeaveEvent()                    override;

protected:

    QPersistentModelIndex m_index;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_RATING_OVERLAY_H
