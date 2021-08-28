/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-10
 * Description : Async buffer
 *
 * Copyright (C)      2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_ASYNCBUFFER_H
#define DIGIKAM_ASYNCBUFFER_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

namespace Digikam
{

template <typename T>
class Q_DECL_HIDDEN AsyncBuffer 
{
public:

    explicit AsyncBuffer(int capacity);
    ~AsyncBuffer();

public:

    T read();
    void append(const T& object);
    void cancel();

private:

    QQueue<T> m_data;
    QMutex m_mutex;
    QWaitCondition m_readWait;
    QWaitCondition m_writeWait;
    const int m_capacity;
    bool m_cancel;
};

template <typename T>
AsyncBuffer<T>::AsyncBuffer(int capacity)
    : m_capacity(capacity),
      m_cancel(false)
{
    Q_ASSERT(capacity > 0);   
}

template <typename T>
AsyncBuffer<T>::~AsyncBuffer()
{
    cancel();
}

template <typename T>
void AsyncBuffer<T>::cancel()
{
    m_cancel = true;
    m_writeWait.wakeAll();
    m_readWait.wakeAll();
}

template <typename T>
T AsyncBuffer<T>::read()
{
    T object;

    m_mutex.lock();

    while (!m_cancel && m_data.empty())
    {
        m_readWait.wait(&m_mutex);
    }

    if (!m_data.empty()) 
    {
        object = m_data.dequeue();
    }

    m_mutex.unlock();
    m_writeWait.wakeAll();

    return object;
}

template <typename T>
void AsyncBuffer<T>::append(const T& object)
{
    if (m_cancel)
    {
        return;
    }

    m_mutex.lock();

    while (!m_cancel && m_data.size() >= m_capacity)
    {
        m_writeWait.wait(&m_mutex);
    }    

    m_data.enqueue(object);

    m_mutex.unlock();
    m_readWait.wakeAll();
}

}

#endif // DIGIKAM_ASYNCBUFFER_H