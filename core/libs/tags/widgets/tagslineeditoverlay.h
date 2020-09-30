/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-30
 * Description : line edit for addition of tags on mouse hover
 *
 * Copyright (C) 2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef DIGIKAM_TAGS_LINE_EDIT_OVERLAY_H
#define DIGIKAM_TAGS_LINE_EDIT_OVERLAY_H

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "itemdelegateoverlay.h"
#include "itemviewdelegate.h"

namespace Digikam
{

class AddTagsLineEdit;

class TagsLineEditOverlay : public AbstractWidgetDelegateOverlay
{
    Q_OBJECT
    REQUIRE_DELEGATE(ItemViewDelegate)

public:

    explicit TagsLineEditOverlay(QObject* const parent);

    AddTagsLineEdit* addTagsLineEdit() const;

Q_SIGNALS:

    void tagEdited(const QModelIndex& index, int rating);
    void tagEdited(const QModelIndex& index, const QString&);

protected Q_SLOTS:

    void slotTagChanged(int);
    void slotTagChanged(const QString&);
    void slotDataChanged(const QModelIndex&, const QModelIndex&);

protected:

    QWidget* createWidget()                    override;
    void setActive(bool)                       override;
    void visualChange()                        override;
    void slotEntered(const QModelIndex& index) override;
    void hide()                                override;

    void updatePosition();
    void updateTag();

protected:

    QPersistentModelIndex m_index;
};

} // namespace Digikam

#endif // DIGIKAM_TAGS_LINE_EDIT_OVERLAY_H
