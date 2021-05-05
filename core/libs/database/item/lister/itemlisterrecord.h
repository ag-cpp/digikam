/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Data set for item lister
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_LISTER_RECORD_H
#define DIGIKAM_ITEM_LISTER_RECORD_H

// Qt includes

#include <QString>
#include <QDataStream>
#include <QDateTime>
#include <QSize>
#include <QList>
#include <QVariant>

// Local includes

#include "digikam_export.h"
#include "coredbalbuminfo.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemListerRecord
{

public:

    explicit ItemListerRecord();

    bool operator==(const ItemListerRecord& record) const;

public:

    int                    albumID;
    int                    albumRootID;
    int                    rating;

    qlonglong              fileSize;
    qlonglong              imageID;
    qlonglong              currentFuzzySearchReferenceImage;

    double                 currentSimilarity;

    QString                format;
    QString                name;

    QDateTime              creationDate;
    QDateTime              modificationDate;

    QSize                  imageSize;

    DatabaseItem::Category category;

    QList<QVariant>        extraValues;
};

DIGIKAM_DATABASE_EXPORT QDataStream& operator<<(QDataStream& os, const ItemListerRecord& record);
DIGIKAM_DATABASE_EXPORT QDataStream& operator>>(QDataStream& ds, ItemListerRecord& record);

} // namespace Digikam

#endif // DIGIKAM_ITEM_LISTER_RECORD_H
