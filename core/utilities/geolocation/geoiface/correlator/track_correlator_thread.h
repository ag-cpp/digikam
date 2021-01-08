/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-19
 * Description : Thread for correlator for tracks and images
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_TRACK_CORRELATOR_THREAD_H
#define DIGIKAM_TRACK_CORRELATOR_THREAD_H

// Qt includes

#include <QThread>

// Local includes

#include "track_correlator.h"

namespace Digikam
{

class TrackCorrelatorThread : public QThread
{
    Q_OBJECT

public:

    explicit TrackCorrelatorThread(QObject* const parent = nullptr);
    ~TrackCorrelatorThread() override;

public:

    TrackCorrelator::Correlation::List  itemsToCorrelate;
    TrackCorrelator::CorrelationOptions options;
    TrackManager::Track::List           fileList;
    bool                                doCancel;
    bool                                canceled;

protected:

    void run() override;

Q_SIGNALS:

    void signalItemsCorrelated(const Digikam::TrackCorrelator::Correlation::List& correlatedItems);
};

} // namespace Digikam

#endif // DIGIKAM_TRACK_CORRELATOR_THREAD_H
