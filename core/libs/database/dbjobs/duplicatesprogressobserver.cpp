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

#include "duplicatesprogressobserver.h"

// Local includes

#include "dbjob.h"

namespace Digikam
{

DuplicatesProgressObserver::DuplicatesProgressObserver(SearchesJob* const thread)
    : HaarProgressObserver(),
      m_job(thread)
{
}

DuplicatesProgressObserver::~DuplicatesProgressObserver()
{
    m_job = nullptr;
}

void DuplicatesProgressObserver::totalNumberToScan(int number)
{
    emit m_job->totalSize(number);
}

void DuplicatesProgressObserver::processedNumber(int number)
{
    emit m_job->processedSize(number);
}

bool DuplicatesProgressObserver::isCanceled()
{
    return m_job->isCanceled();
}

} // namespace Digikam
