/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-09
 * Description : DTrash item info model
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_DTRASH_ITEM_MODEL_H
#define DIGIKAM_DTRASH_ITEM_MODEL_H

// Qt includes

#include <QAbstractTableModel>

// Local includes

#include "dtrashiteminfo.h"
#include "thumbnailloadthread.h"

namespace Digikam
{

class DTrashItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit DTrashItemModel(QObject* const parent = nullptr);
    ~DTrashItemModel() override;

    /**
     * @brief loads a thumbnail for item in trash for showing
     * @param path: path of image in trash
     * @param pix: Pixmap to fill
     * @return true if there is an available thumbnail
     */
    bool pixmapForItem(const QString& path, QPixmap& pix)                   const;

    /**
     * @brief Clears all data from model and informs the view
     */
    void clearCurrentData();

    /**
     * @brief Runs a thread to list all items from a collection trash
     * @param colPath: path to collection to load items for
     */
    void loadItemsForCollection(const QString& colPath);

    /**
     * @brief returns DTrashItemInfo for specific index in model
     */
    DTrashItemInfo itemForIndex(const QModelIndex& index);

    /**
     * @brief returns DTrashItemInfoList for gived indexes in model
     */
    DTrashItemInfoList itemsForIndexes(const QList<QModelIndex>& indexes);

    /**
     * @brief returns the index for the DTrashItemInfo in model
     */
    QModelIndex indexForItem(const DTrashItemInfo& itemInfo)                const;

    /**
     * @brief returns a list of all items in model
     */
    DTrashItemInfoList allItems();

    /**
     * @return true if there is no data in the model
     */
    bool isEmpty();

    /**
     * @brief Changes the thumbnail size
     * @param size: size to change to
     */
    void changeThumbSize(int size);

/// QAbstractItemModel interface
public:

    int rowCount(const QModelIndex&)                                        const override;
    int columnCount(const QModelIndex&)                                     const override;

    QVariant data(const QModelIndex& index, int role)                       const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public Q_SLOTS:

    /**
     * @brief appends item to model data and informs the view
     * @param itemInfo: item to append
     */
    void append(const DTrashItemInfo& itemInfo);

    /**
     * @brief removes list of items for given indexes from model
     *        data and informs the view
     * @param indexes: indexes to remove
     */
    void removeItems(const QModelIndexList& indexes);

    /**
     * @brief refreshes the view layout
     */
    void refreshLayout();

    /**
     * @brief refreshes the thumbnails
     * @param desc: loading description from thumbnail load thread
     * @param pix: pixmap from thumbnail load thread
     */
    void refreshThumbnails(const LoadingDescription& desc, const QPixmap& pix);

Q_SIGNALS:

    void dataChange();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DTRASH_ITEM_MODEL_H
