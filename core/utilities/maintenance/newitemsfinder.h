/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : new items finder.
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_NEW_ITEMS_FINDER_H
#define DIGIKAM_NEW_ITEMS_FINDER_H

// Qt includes

#include <QObject>
#include <QString>

// Local includes

#include "maintenancetool.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT NewItemsFinder : public MaintenanceTool
{
    Q_OBJECT

public:

    enum FinderMode
    {
        CompleteCollectionScan,   ///< Scan whole collection immediately.
        ScanDeferredFiles,        ///< Defer whole collection scan.
        ScheduleCollectionScan    ///< Scan immediately folders list passed in constructor.
    };

public:

    explicit NewItemsFinder(const FinderMode mode = CompleteCollectionScan,
                            const QStringList& foldersToScan = QStringList(),
                            ProgressItem* const parent = nullptr);
    ~NewItemsFinder() override;

private Q_SLOTS:

    void slotStart() override;
    void slotScanStarted(const QString&);
    void slotPartialScanDone(const QString&);
    void slotTotalFilesToScan(int);
    void slotFilesScanned(int);
    void slotCancel() override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_NEW_ITEMS_FINDER_H
