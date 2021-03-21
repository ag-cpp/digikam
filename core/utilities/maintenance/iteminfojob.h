/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-22-01
 * Description : interface to get item info from database.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_INFO_JOB_H
#define DIGIKAM_ITEM_INFO_JOB_H

// Qt includes

#include <QObject>
#include <QByteArray>

// Local includes

#include "iteminfo.h"

namespace Digikam
{

class Album;

class ItemInfoJob : public QObject
{
    Q_OBJECT

public:

    explicit ItemInfoJob();
    ~ItemInfoJob() override;

    void allItemsFromAlbum(Album* const album);
    void stop();
    bool isRunning() const;

Q_SIGNALS:

    void signalItemsInfo(const ItemInfoList& items);
    void signalCompleted();

private Q_SLOTS:

    void slotResult();
    void slotData(const QList<ItemListerRecord>& data);

private:

    // Disabme
    ItemInfoJob(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_INFO_JOB_H
