/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-01-29
 * Description : Database cleaner.
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

#ifndef DIGIKAM_DB_CLEANER_H
#define DIGIKAM_DB_CLEANER_H

// Qt includes

#include <QDialog>
#include <QString>
#include <QWidget>

// Local includes

#include "dlayoutbox.h"
#include "dworkingpixmap.h"
#include "maintenancetool.h"

namespace Digikam
{
    class Identity;
}

namespace Digikam
{

class DbCleaner : public MaintenanceTool
{
    Q_OBJECT

public:

    explicit DbCleaner(bool cleanThumbsDb = false,
                       bool cleanFacesDb = false,
                       bool cleanSimilarityDb = false,
                       bool shrinkDatabases = false,
                       ProgressItem* const parent = nullptr);
    ~DbCleaner()                    override;

    void setUseMultiCoreCPU(bool b) override;

private Q_SLOTS:

    void slotStart()                override;
    void slotCancel()               override;
    void slotAdvance();
    void slotShrinkNextDBInfo(bool done, bool passed);

    void slotFetchedData(const QList<qlonglong>& staleImageIds,
                         const QList<int>& staleThumbIds,
                         const QList<Identity>& staleIdentities,
                         const QList<qlonglong>& staleImageSimilarities);

    void slotAddItemsToProcess(int count);

    void slotCleanItems();
    void slotCleanedItems();
    void slotCleanedThumbnails();
    void slotCleanedFaces();
    void slotCleanedSimilarity();
    void slotShrinkDatabases();

    void slotDone()                 override;

private:

    class Private;
    Private* const d;
};

// -----------------------------------------------------

class DbShrinkDialog : public QDialog
{
    Q_OBJECT

public:

    explicit DbShrinkDialog(QWidget* const parent);
    ~DbShrinkDialog() override;

    void setActive(const int pos);
    void setIcon(const int pos, const QIcon& icon);

public Q_SLOTS:

    int exec()        override;

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DB_CLEANER_H
