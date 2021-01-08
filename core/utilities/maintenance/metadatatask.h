/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-09
 * Description : Thread actions task for metadata synchronizer.
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_METADATA_TASK_H
#define DIGIKAM_METADATA_TASK_H

// Qt includes

#include <QThread>

// Local includes

#include "actionthreadbase.h"
#include "iteminfo.h"
#include "metadatasynchronizer.h"

namespace Digikam
{

class MaintenanceData;

class MetadataTask : public ActionJob
{
    Q_OBJECT

public:

    explicit MetadataTask();
    ~MetadataTask()         override;

    void setTagsOnly(bool value);
    void setDirection(MetadataSynchronizer::SyncDirection dir);
    void setMaintenanceData(MaintenanceData* const data = nullptr);

Q_SIGNALS:

    void signalFinished(const QImage&);

protected:

    void run()              override;

private:

    // Disable
    MetadataTask(QObject*);

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_TASK_H
