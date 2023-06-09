/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-10-11
 * Description : item sort based on QCollator
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemsortcollator.h"

// Qt includes

#include <QCollator>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemSortCollator::Private
{
public:

    explicit Private()
    {
        itemCollator.setNumericMode(true);
        albumCollator.setNumericMode(true);
    }

    QCollator itemCollator;
    QCollator albumCollator;
};

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN ItemSortCollatorCreator
{
public:

    ItemSortCollator object;
};

Q_GLOBAL_STATIC(ItemSortCollatorCreator, itemSortCollatorCreator)

// -----------------------------------------------------------------------------------------------

ItemSortCollator::ItemSortCollator()
    : d(new Private)
{
}

ItemSortCollator::~ItemSortCollator()
{
    delete d;
}

ItemSortCollator* ItemSortCollator::instance()
{
    return &itemSortCollatorCreator->object;
}

int ItemSortCollator::itemCompare(const QString& a, const QString& b,
                                  Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if (natural)
    {
        d->itemCollator.setCaseSensitivity(caseSensitive);
        d->itemCollator.setIgnorePunctuation(a.contains(QLatin1String("_v"),
                                                        Qt::CaseInsensitive));

        return d->itemCollator.compare(a, b);
    }

    return QString::compare(a, b, caseSensitive);
}

int ItemSortCollator::albumCompare(const QString& a, const QString& b,
                                   Qt::CaseSensitivity caseSensitive, bool natural) const
{
    if (natural)
    {
        d->albumCollator.setCaseSensitivity(caseSensitive);
        d->albumCollator.setIgnorePunctuation(false);

        return d->albumCollator.compare(a, b);
    }

    return QString::compare(a, b, caseSensitive);
}

} // namespace Digikam
