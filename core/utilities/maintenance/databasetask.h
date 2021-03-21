/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-01-29
 * Description : Thread actions task for database cleanup.
 *
 * Copyright (C) 2017-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
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

#ifndef DIGIKAM_DATABASE_TASK_H
#define DIGIKAM_DATABASE_TASK_H

// Local includes

#include "actionthreadbase.h"

namespace Digikam
{
    class Identity;
}

namespace Digikam
{

class LoadingDescription;
class MaintenanceData;

class DatabaseTask : public ActionJob
{
    Q_OBJECT

public:

    enum Mode
    {
        Unknown,
        ComputeDatabaseJunk,
        CleanCoreDb,
        CleanThumbsDb,
        CleanRecognitionDb,
        CleanSimilarityDb,
        ShrinkDatabases
    };

public:

    explicit DatabaseTask();
    ~DatabaseTask()     override;

    void setMode(Mode mode);
    void setMaintenanceData(MaintenanceData* const data = nullptr);

    void computeDatabaseJunk(bool thumbsDb = false,
                             bool facesDb = false,
                             bool similarityDb = false);

Q_SIGNALS:

    void signalFinished();
    void signalFinished(bool done, bool errorFree);

    void signalData(const QList<qlonglong>& staleImageIds,
                    const QList<int>& staleThumbIds,
                    const QList<Identity>& staleIdentities,
                    const QList<qlonglong>& staleSimilarityImageIds);

    /**
     * Signal to emit the count of additional items to process.
     */
    void signalAddItemsToProcess(int count);

protected:

    void run()          override;

private:

    // Disable
    DatabaseTask(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DATABASE_TASK_H
