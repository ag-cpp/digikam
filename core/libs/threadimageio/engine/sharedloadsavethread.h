/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-16
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_SHARED_LOAD_SAVE_THREAD_H
#define DIGIKAM_SHARED_LOAD_SAVE_THREAD_H

// Local includes

#include "managedloadsavethread.h"

namespace Digikam
{

class DIGIKAM_EXPORT SharedLoadSaveThread : public ManagedLoadSaveThread
{
    Q_OBJECT

public:

    explicit SharedLoadSaveThread(QObject* const parent = nullptr);
    ~SharedLoadSaveThread() override;

    void load(const LoadingDescription& description,
              AccessMode mode,
              LoadingPolicy policy = LoadingPolicyAppend);

private:

    // Disable
    SharedLoadSaveThread(const SharedLoadSaveThread&)            = delete;
    SharedLoadSaveThread& operator=(const SharedLoadSaveThread&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_SHARED_LOAD_SAVE_THREAD_H
