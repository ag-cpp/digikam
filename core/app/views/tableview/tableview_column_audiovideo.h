/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-05-13
 * Description : Table view column helpers: Audio/video properties
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_TABLE_VIEW_COLUMN_AUDIOVIDEO_H
#define DIGIKAM_TABLE_VIEW_COLUMN_AUDIOVIDEO_H

// Qt includes

#include <QStringList>

// Local includes

#include "tableview_columnfactory.h"

namespace Digikam
{

namespace TableViewColumns
{

class ColumnAudioVideoProperties : public TableViewColumn
{
    Q_OBJECT

public:

    enum SubColumn
    {
        SubColumnAudioBitRate     = 0,
        SubColumnAudioChannelType = 1,
        SubColumnAudioCodec       = 2,
        SubColumnDuration         = 3,
        SubColumnFrameRate        = 4,
        SubColumnVideoCodec       = 5
    };

public:

    explicit ColumnAudioVideoProperties(TableViewShared* const tableViewShared,
                                        const TableViewColumnConfiguration& pConfiguration,
                                        const SubColumn pSubColumn,
                                        QObject* const parent = nullptr);
    ~ColumnAudioVideoProperties() override;

    QString getTitle()                                                                                const override;
    ColumnFlags getColumnFlags()                                                                      const override;
    QVariant data(TableViewModel::Item* const item, const int role)                                   const override;
    ColumnCompareResult compare(TableViewModel::Item* const itemA, TableViewModel::Item* const itemB) const override;
    void setConfiguration(const TableViewColumnConfiguration& newConfiguration) override;

    static TableViewColumnDescription getDescription();
    static QStringList getSubColumns();

private:

    SubColumn subColumn;
};

} // namespace TableViewColumns

} // namespace Digikam

#endif // DIGIKAM_TABLE_VIEW_COLUMN_AUDIOVIDEO_H
