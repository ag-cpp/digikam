/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-07
 * Description : Trash view
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_TRASH_VIEW_H
#define DIGIKAM_TRASH_VIEW_H

// Qt includes

#include <QWidget>
#include <QStyledItemDelegate>

namespace Digikam
{

class DTrashItemModel;
class ThumbnailSize;

class TrashView : public QWidget
{
    Q_OBJECT

public:

    explicit TrashView(QWidget* const parent = nullptr);
    ~TrashView() override;

    /**
     * @return model used for the view
     */
    DTrashItemModel* model()            const;

    /**
     * @return current thumbnail size
     */
    ThumbnailSize getThumbnailSize()    const;

    /**
     * @brief set thumbnail size to give to model
     * @param thumbSize: size to set
     */
    void setThumbnailSize(const ThumbnailSize& thumbSize);

    /**
     * @return QUrl to the last selected item in view
     */
    QUrl lastSelectedItemUrl()          const;

    /**
     * @brief Highlights the last selected item when the view gets focus
     */
    void selectLastSelected();

    /**
     * @return text for the main status bar
     */
    QString statusBarText()             const;

private Q_SLOTS:

    void slotSelectionChanged();
    void slotUndoLastDeletedItems();
    void slotRestoreSelectedItems();
    void slotDeleteSelectedItems();
    void slotRemoveItemsFromModel();
    void slotRemoveAllItemsFromModel();
    void slotDeleteAllItems();
    void slotDataChanged();
    void slotChangeLastSelectedItem(const QModelIndex& curr, const QModelIndex& prev);

Q_SIGNALS:

    void selectionChanged();

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------

class ThumbnailAligningDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit ThumbnailAligningDelegate(QObject* const parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace Digikam

#endif // DIGIKAM_TRASH_VIEW_H
