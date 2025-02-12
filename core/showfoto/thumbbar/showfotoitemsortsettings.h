/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 29-07-2013
 * Description : Sort settings for showfoto item infos
 *
 * SPDX-FileCopyrightText: 2013 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QCollator>

// Local includes

#include "showfotoiteminfo.h"

namespace ShowFoto
{

class ShowfotoItemSortSettings
{
public:

    enum SortOrder
    {
        AscendingOrder  = Qt::AscendingOrder,
        DescendingOrder = Qt::DescendingOrder,
        DefaultOrder                            ///< sort order depends on the chosen sort role
    };

    enum CategorizationMode
    {
        NoCategories,
        CategoryByFolder,
        CategoryByFormat
    };

    enum SortRole
    {
        SortByCreationDate,
        SortByFileName,
        SortByFileSize
    };

public:

    ShowfotoItemSortSettings()  = default;
    ~ShowfotoItemSortSettings() = default;

    bool operator==(const ShowfotoItemSortSettings& other) const;

    /**
     * Compares the categories of left and right ShowfotoItemInfos.
     * It returns -1 if the left ShowfotoItemInfo is less than right, and 0 if both fall
     * in the same category, and 1 if the left ShowfotoItemInfo is greater than right.
     * Adheres to set categorization mode and current category sort order.
     */
    int compareCategories(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const;


    /**
     * Returns true if left is less than right.
     * Adheres to current sort role and sort order.
     */
    bool lessThan(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const;

    /**
     * Returns true if left QVariant is less than right.
     * Adheres to current sort role and sort order.
     */
    bool lessThan(const QVariant& left, const QVariant& right) const;

    /**
     * Compares the showfotoItemInfos left and right.
     * Return -1 if left is less than right, 1 if left is greater than right,
     * and 0 if left equals right comparing the current sort role's value.
     * Adheres to set sort role and sort order.
     */
    int compare(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const;

    /// --- Categories ---------------

    void setCategorizationMode(CategorizationMode mode);
    void setCategorizationSortOrder(SortOrder order);

    bool isCategorized() const
    {
        return (categorizationMode >= CategoryByFolder);
    }

    /// --- Showfoto Items Sorting ---------------

    void setSortRole(SortRole role);
    void setSortOrder(SortOrder order);

    int  compare(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right, SortRole sortRole) const;

    static Qt::SortOrder defaultSortOrderForCategorizationMode(CategorizationMode mode);
    static Qt::SortOrder defaultSortOrderForSortRole(SortRole role);

    /**
     * Returns a < b if sortOrder is Ascending, or b < a if order is descending
     */
    template <typename T>
    static inline bool lessThanByOrder(const T& a, const T& b, Qt::SortOrder sortOrder)
    {
        if (sortOrder == Qt::AscendingOrder)
        {
            return (a < b);
        }
        else
        {
            return (b < a);
        }
    }

    /**
     * Returns the usual compare result of -1, 0, or 1 for lessThan, equals and greaterThan.
     */
    template <typename T>
    static inline int compareValue(const T& a, const T &b)
    {
        if (a == b)
        {
            return 0;
        }

        if (a > b)
        {
            return 1;
        }

        return (-1);
    }

    /**
     * Takes a typical result from a compare method (0 is equal, -1 is less than, 1 is greater than)
     * and applies the given sort order to it.
     */
    static inline int compareByOrder(int compareResult, Qt::SortOrder sortOrder)
    {
        if (sortOrder == Qt::AscendingOrder)
        {
            return compareResult;
        }

        return (- compareResult);
    }

    template <typename T>
    static inline int compareByOrder(const T& a, const T& b, Qt::SortOrder sortOrder)
    {
        return compareByOrder(compareValue(a, b), sortOrder);
    }

    /**
     * Compares the two string by natural comparison and adheres to given sort order
     */
    static inline int naturalCompare(const QString& a,
                                     const QString& b,
                                     Qt::SortOrder sortOrder,
                                     Qt::CaseSensitivity caseSensitive = Qt::CaseSensitive)
    {
        QCollator collator;
        collator.setNumericMode(true);
        collator.setIgnorePunctuation(false);
        collator.setCaseSensitivity(caseSensitive);

        return (compareByOrder(collator.compare(a, b), sortOrder));
    }

public:

    CategorizationMode   categorizationMode                 = NoCategories;
    SortOrder            categorizationSortOrder            = DefaultOrder;

    /**
     * Only Ascending or Descending, never be DefaultOrder
     */
    Qt::SortOrder        currentCategorizationSortOrder     = Qt::AscendingOrder;
    Qt::CaseSensitivity  categorizationCaseSensitivity      = Qt::CaseSensitive;

    SortOrder            sortOrder                          = DefaultOrder;
    SortRole             sortRole                           = SortByFileName;

    Qt::SortOrder        currentSortOrder                   = Qt::AscendingOrder;
    Qt::CaseSensitivity  sortCaseSensitivity                = Qt::CaseSensitive;
};

} // namespace ShowFoto
