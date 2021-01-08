/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : Table view column helpers: Digikam properties
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TABLE_VIEW_COLUMN_DIGIKAM_H
#define DIGIKAM_TABLE_VIEW_COLUMN_DIGIKAM_H

// Qt includes

#include <QStringList>

// Local includes

#include "tableview_columnfactory.h"

namespace Digikam
{

namespace TableViewColumns
{

class ColumnDigikamProperties : public TableViewColumn
{
    Q_OBJECT

public:

    enum SubColumn
    {
        SubColumnRating     = 0,
        SubColumnPickLabel  = 1,
        SubColumnColorLabel = 2,
        SubColumnTitle      = 3,
        SubColumnCaption    = 4,
        SubColumnTags       = 5
    };

public:

    explicit ColumnDigikamProperties(
            TableViewShared* const tableViewShared,
            const TableViewColumnConfiguration& pConfiguration,
            const SubColumn pSubColumn,
            QObject* const parent = nullptr);
    ~ColumnDigikamProperties() override;

    QString getTitle()                                                                                const override;
    ColumnFlags getColumnFlags()                                                                      const override;
    QVariant data(TableViewModel::Item* const item, const int role)                                   const override;
    ColumnCompareResult compare(TableViewModel::Item* const itemA, TableViewModel::Item* const itemB) const override;
    bool columnAffectedByChangeset(const ImageChangeset& imageChangeset)                              const override;

    static TableViewColumnDescription getDescription();
    static QStringList getSubColumns();

private:

    SubColumn subColumn;
};

} // namespace TableViewColumns

} // namespace Digikam

#endif // DIGIKAM_TABLE_VIEW_COLUMN_DIGIKAM_H
