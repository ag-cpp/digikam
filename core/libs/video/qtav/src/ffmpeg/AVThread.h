/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_AV_THREAD_H
#define QTAV_AV_THREAD_H

// Qt includes

#include <QRunnable>
#include <QScopedPointer>
#include <QThread>

// Local includes

#include "PacketBuffer.h"

// TODO: pause functions. AVOutput may be null, use AVThread's pause state

namespace QtAV
{

class AVDecoder;
class AVThreadPrivate;
class AVOutput;
class AVClock;
class Filter;
class Statistics;
class OutputSet;

class AVThread : public QThread
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AVThread)

public:

    explicit AVThread(QObject *parent = nullptr);
    virtual ~AVThread();

    // used for changing some components when running

    Q_DECL_DEPRECATED void lock();
    Q_DECL_DEPRECATED void unlock();

    void setClock(AVClock *clock);
    AVClock* clock() const;

    PacketBuffer* packetQueue() const;

    void setDecoder(AVDecoder *decoder);
    AVDecoder *decoder() const;

    void setOutput(AVOutput *out);
    AVOutput* output() const;

    void setOutputSet(OutputSet *set);
    OutputSet* outputSet() const;

    void setDemuxEnded(bool ended);

    bool isPaused() const;

    bool waitForStarted(int msec = -1);

    bool installFilter(Filter *filter, int index = 0x7FFFFFFF, bool lock = true);
    bool uninstallFilter(Filter *filter, bool lock = true);
    const QList<Filter *> &filters() const;

    // TODO: resample, resize task etc.

    void scheduleTask(QRunnable *task);
    void requestSeek();
    void scheduleFrameDrop(bool value = true);
    qreal previousHistoryPts() const;           // move to statistics?
    qreal decodeFrameRate() const;              // move to statistics?
    void setDropFrameOnSeek(bool value);

    void resetState();

public Q_SLOTS:

    virtual void stop();

    /*change pause state. the pause/continue action will do in the next loop*/

    void pause(bool p);     // processEvents when waiting?

    void nextAndPause();    // process 1 frame and pause

Q_SIGNALS:

    void frameDelivered();

    /*!
     * \brief seekFinished
     * \param timestamp the frame pts after seek
     */
    void seekFinished(qint64 timestamp);
    void eofDecoded();

private Q_SLOTS:

    void onStarted();
    void onFinished();

protected:

    AVThread(AVThreadPrivate& d, QObject *parent = nullptr);

    /*
     * If the pause state is true set by pause(true), then block the thread and wait for pause
     * state changed, i.e. pause(false)
     * and return true. Otherwise, return false imediatly.
     */

    // has timeout so that the pending tasks can be processed

    bool tryPause(unsigned long timeout = 100);
    bool processNextTask(); // in AVThread

    // pts > 0: compare pts and clock when waiting

    void waitAndCheck(ulong value, qreal pts);

    DPTR_DECLARE(AVThread)

private:

    void setStatistics(Statistics* statistics);
    friend class AVPlayer;
};

} // namespace QtAV

#endif // QTAV_AV_THREAD_H
