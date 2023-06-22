/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-21-06
 * Description : Thread actions task for metadata remover.
 *
 * SPDX-FileCopyrightText: 2013-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2023      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_METADATA_REMOVE_TASK_H
#define DIGIKAM_METADATA_REMOVE_TASK_H

// Qt includes

#include <QThread>

// Local includes

#include "actionthreadbase.h"
#include "metadataremover.h"

namespace Digikam
{

class MaintenanceData;

class MetadataRemoveTask : public ActionJob
{
    Q_OBJECT

public:

    explicit MetadataRemoveTask();
    ~MetadataRemoveTask() override;

    void setRemoveAction(MetadataRemover::RemoveAction action);
    void setMaintenanceData(MaintenanceData* const data = nullptr);

Q_SIGNALS:

    void signalFinished(const QImage&);

protected:

    void run()            override;

private:

    // Disable
    MetadataRemoveTask(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_REMOVE_TASK_H
