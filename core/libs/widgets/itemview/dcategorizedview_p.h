/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-16
 * Description : Item view for listing items in a categorized fashion optionally
 *
 * SPDX-FileCopyrightText: 2007      by Rafael Fernández López <ereslibre at kde dot org>
 * SPDX-FileCopyrightText: 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "dcategorizedview.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QVector>
#include <QPainter>
#include <QScrollBar>
#include <QPaintEvent>

// Local includes

#include "dcategorizedsortfilterproxymodel.h"
#include "dcategorydrawer.h"

/**
 * NOTE: By defining DOLPHIN_DRAGANDDROP the custom drag and drop implementation of
 * DCategorizedView is bypassed to have a consistent drag and drop look for all
 * views. Hopefully transparent pixmaps for drag objects will be supported in
 * Qt 4.4, so that this workaround can be skipped.
 */
#define DOLPHIN_DRAGANDDROP

class DCategoryDrawer;

namespace Digikam
{

class DCategorizedSortFilterProxyModel;

class Q_DECL_HIDDEN SparseModelIndexVector : public QVector<QModelIndex>
{
public:

    explicit SparseModelIndexVector(int rowCount, QAbstractItemModel* const model_, int column_)
        : QVector<QModelIndex>(rowCount),
          model               (model_),
          column              (column_)
    {
    }

    inline QModelIndex& operator[](int i)
    {
        QModelIndex& index = QVector<QModelIndex>::operator[](i);

        if (!index.isValid())
        {
            index = model->index(i, column);
        }

        return index;
    }

private:

    // not to be used

    const QModelIndex& operator[](int i) const
    {
        return QVector<QModelIndex>::operator[](i);
    }

private:

    QAbstractItemModel* model   = nullptr;
    int                 column  = 0;
};

// ------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DCategorizedView::Private
{
public:

    explicit Private(DCategorizedView* const listView);
    ~Private() = default;

    // Methods

    /**
     * Returns the list of items that intersects with @p rect
     */
    const QModelIndexList& intersectionSet(const QRect& rect);

    /**
     * Gets the item rect in the viewport for @p index
     */
    QRect visualRectInViewport(const QModelIndex& index) const;

    /**
     * Returns the category rect in the viewport for @p category
     */
    QRect visualCategoryRectInViewport(const QString& category) const;

    /**
     * Caches and returns the rect that corresponds to @p index
     */
    const QRect& cacheIndex(const QModelIndex& index);

    /**
     * Caches and returns the rect that corresponds to @p category
     */
    const QRect& cacheCategory(const QString& category);

    /**
     * Returns the rect that corresponds to @p index
     * @note If the rect is not cached, it becomes cached
     */
    const QRect& cachedRectIndex(const QModelIndex& index);

    /**
     * Returns the rect that corresponds to @p category
     * @note If the rect is not cached, it becomes cached
     */
    const QRect& cachedRectCategory(const QString& category);

    /**
     * Returns the visual rect (taking in count x and y offsets) for @p index
     * @note If the rect is not cached, it becomes cached
     */
    QRect visualRect(const QModelIndex& index);

    /**
     * Returns the visual rect (taking in count x and y offsets) for @p category
     * @note If the rect is not cached, it becomes cached
     */
    QRect categoryVisualRect(const QString& category);

    /**
     * Returns the contents size of this view (topmost category to bottommost index + spacing)
     */
    QSize contentsSize();

    /**
     * This method will draw a new category represented by index
     * @p index on the rect specified by @p option.rect, with
     * painter @p painter
     */
    void drawNewCategory(const QModelIndex& index,
                         int sortRole,
                         const QStyleOption& option,
                         QPainter* painter);

    /**
     * This method will update scrollbars ranges. Called when our model changes
     * or when the view is resized
     */
    void updateScrollbars();

    /**
     * This method will draw dragged items in the painting operation
     */
    void drawDraggedItems(QPainter* painter);

    /**
     * This method will determine which rect needs to be updated because of a
     * dragging operation
     */
    void drawDraggedItems();

    /**
     * This method will, starting from the index at begin in the given (sorted) modelIndex List,
     * find the last index having the same category as the index to begin with.
     */
    int categoryUpperBound(SparseModelIndexVector& modelIndexList, int begin, int averageSize = 0);

    /**
     * Returns a QItemSelection for all items intersection rect.
     */
    QItemSelection selectionForRect(const QRect& rect);

public:

    /// Attributes

    class Q_DECL_HIDDEN ElementInfo
    {
    public:

        ElementInfo() = default;

        QString category;
        int     relativeOffsetToCategory = 0;
    };

public:

    /// Basic data

    DCategorizedView*                 listView                  = nullptr;
    DCategoryDrawer*                  categoryDrawer            = nullptr;
    QSize                             biggestItemSize           = QSize(0, 0);

    /// Behavior data
    bool                              mouseButtonPressed        = false;
    bool                              rightMouseButtonPressed   = false;
    bool                              dragLeftViewport          = false;
    bool                              drawItemsWhileDragging    = true;
    QModelIndex                       hovered;
    QString                           hoveredCategory;
    QPoint                            initialPressPosition;
    QPoint                            mousePosition;
    int                               forcedSelectionPosition   = 0;

    /**
     * Cache data
     * We cannot merge some of them into structs because it would affect
     * performance
     */

    QVector<ElementInfo>              elementsInfo;
    QHash<int, QRect>                 elementsPosition;
    QHash<QString, QVector<int> >     categoriesIndexes;
    QHash<QString, QRect>             categoriesPosition;
    QStringList                       categories;
    QModelIndexList                   intersectedIndexes;
    QRect                             lastDraggedItemsRect;
    QItemSelection                    lastCategorySelection;
    QItemSelection                    lastSelection;

    /// Attributes for speed reasons

    DCategorizedSortFilterProxyModel* proxyModel                = nullptr;
};

} // namespace Digikam
