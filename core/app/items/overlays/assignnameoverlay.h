/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-14
 * Description : overlay for assigning names to faces
 *
 * Copyright (C) 2010      by Aditya Bhatt <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_ASSIGN_NAME_OVERLAY_H
#define DIGIKAM_ASSIGN_NAME_OVERLAY_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "itemdelegateoverlay.h"
#include "itemviewdelegate.h"

namespace Digikam
{

class AssignNameWidget;
class FaceTagsIface;
class ItemInfo;
class TaggingAction;

class AssignNameOverlay : public PersistentWidgetDelegateOverlay
{
    Q_OBJECT
    REQUIRE_DELEGATE(ItemViewDelegate)

public:

    explicit AssignNameOverlay(QObject* const parent);
    ~AssignNameOverlay()                                              override;

    AssignNameWidget* assignNameWidget() const;

Q_SIGNALS:

    void confirmFaces(const QList<QModelIndex>& indexes, int tagId);
    void removeFaces(const QList<QModelIndex>& indexes);

protected Q_SLOTS:

    void slotAssigned(const TaggingAction& action, const ItemInfo&, const QVariant& faceIdentifier);
    void slotRejected(const ItemInfo&, const QVariant& faceIdentifier);

protected:

    QWidget* createWidget()                                           override;
    void     setActive(bool)                                          override;
    void     visualChange()                                           override;
    void     showOnIndex(const QModelIndex& index)                    override;
    void     hide()                                                   override;
    bool     checkIndex(const QModelIndex& index)               const override;
    void     viewportLeaveEvent(QObject* obj, QEvent* event)          override;
    void     widgetEnterEvent()                                       override;
    void     widgetLeaveEvent()                                       override;
    void     setFocusOnWidget()                                       override;
    bool     eventFilter(QObject* o, QEvent* e)                       override;

    void updatePosition();
    void updateFace();

private:

    AssignNameOverlay(); // Disable default constructor.

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ASSIGN_NAME_OVERLAY_H
