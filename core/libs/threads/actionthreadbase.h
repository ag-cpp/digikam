/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-12-28
 * Description : Low level threads management for batch processing on multi-core
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * Copyright (C) 2011-2012 by Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef DIGIKAM_ACTION_THREAD_BASE_H
#define DIGIKAM_ACTION_THREAD_BASE_H

// Qt includes

#include <QHash>
#include <QObject>
#include <QThread>
#include <QRunnable>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ActionJob : public QObject,
                                 public QRunnable
{
    Q_OBJECT

public:

    /** Constructor which delegate deletion of QRunnable instance to ActionThreadBase, not QThreadPool.
     */
    explicit ActionJob(QObject* const parent = nullptr);

    /** Re-implement destructor in you implementation. Don't forget to cancel job.
     */
    ~ActionJob() override;

Q_SIGNALS:

    /** Use this signal in your implementation to inform ActionThreadBase manager that job is started
     */
    void signalStarted();

    /** Use this signal in your implementation to inform ActionThreadBase manager the job progress
     */
    void signalProgress(int);

    /** Use this signal in your implementation to inform ActionThreadBase manager the job is done.
     */
    void signalDone();

public Q_SLOTS:

    /** Call this method to cancel job.
     */
    void cancel();

protected:

    /** You can use this boolean in your implementation to know if job must be canceled.
     */
    bool m_cancel;
};

/** Define a QHash of job/priority to process by ActionThreadBase manager.
 *  Priority value can be used to control the run queue's order of execution.
 *  Zero priority want mean to process job with higher priority.
 */
typedef QHash<ActionJob*, int> ActionJobCollection;

// -------------------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT ActionThreadBase : public QThread
{
    Q_OBJECT

public:

    explicit ActionThreadBase(QObject* const parent = nullptr);
    ~ActionThreadBase() override;

    /** Adjust maximum number of threads used to parallelize collection of job processing.
     */
    void setMaximumNumberOfThreads(int n);

    /** Return the maximum number of threads used to parallelize collection of job processing.
     */
    int  maximumNumberOfThreads()   const;

    /** Reset maximum number of threads used to parallelize collection of job processing to max core detected on computer.
     *  This method is called in contructor.
     */
    void setDefaultMaximumNumberOfThreads();

    /** Cancel processing of current jobs under progress.
     */
    void cancel();

protected:

    /** Main thread loop used to process jobs in todo list.
     */
    void run()                            override;

    /** Append a collection of jobs to process into QThreadPool.
     *  Jobs are add to pending lists and will be deleted by ActionThreadBase, not QThreadPool.
     */
    void appendJobs(const ActionJobCollection& jobs);

    /** Return true if list of pending jobs to process is empty.
     */
    bool isEmpty()                  const;

    /** Return the number of pending jobs to process.
     */
    int pendingCount()              const;

protected Q_SLOTS:

    void slotJobFinished();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ACTION_THREAD_BASE_H
