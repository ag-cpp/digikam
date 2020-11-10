/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-10
 * Description : Progress observer for duplicate scanning
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H
#define DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H

#include "haariface.h"
#include "dbjob.h"
#include "digikam_export.h"

namespace Digikam
{

class SearchesJob;

class DIGIKAM_DATABASE_EXPORT DuplicatesProgressObserver : public HaarProgressObserver
{

public:

    explicit DuplicatesProgressObserver(SearchesJob* const thread);
    ~DuplicatesProgressObserver()       override;

    void totalNumberToScan(int number)  override;
    void processedNumber(int number)    override;
    bool isCanceled()                   override;

private:

    SearchesJob* m_job;

private:

    Q_DISABLE_COPY(DuplicatesProgressObserver)
};

} // namespace Digikam

#endif // DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H
