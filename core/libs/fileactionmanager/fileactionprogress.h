/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-23
 * Description : file action progress indicator
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

#ifndef DIGIKAM_FILE_ACTION_PROGRESS_H
#define DIGIKAM_FILE_ACTION_PROGRESS_H

// Qt includes

#include <QString>

// Local includes

#include "progressmanager.h"

namespace Digikam
{

class FileActionProgress : public ProgressItem
{
    Q_OBJECT

public:

    explicit FileActionProgress(const QString& name);
    ~FileActionProgress() override;

Q_SIGNALS:

    void signalComplete();

private Q_SLOTS:

    void slotProgressValue(float);
    void slotProgressStatus(const QString&);
    void slotCompleted();
    void slotCancel();

private:

    // Disable
    FileActionProgress(QObject*);
};

} // namespace Digikam

#endif // DIGIKAM_FILE_ACTION_PROGRESS_H
