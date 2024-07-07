/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"

class QDateTime;

namespace Marble
{

class MarbleClockPrivate;

class DIGIKAM_EXPORT MarbleClock : public QObject
{
    Q_OBJECT

public:

    explicit MarbleClock(QObject* parent = nullptr);

    ~MarbleClock() override;

    /**
     * @brief Determine how much of the current day has elapsed
     * @return A value between 0 and 1 corresponding to the fraction of the day that has elapsed
     */
    qreal dayFraction() const;

Q_SIGNALS:

    /**
     * @brief the timeChanged signal will be triggered at updateInterval() rate
     * or at most every second.
     **/
    void timeChanged();

    /**
     * @brief Emitted when setUpdateInterval() is called.
     */
    void updateIntervalChanged(int seconds);

public:

    /**
     * @brief Sets the internal date and time a custom one
     * @param datetime The custom date and time
     **/
    void setDateTime(const QDateTime& datetime);

    /**
     * @brief Returns the internal date and time
     **/
    QDateTime dateTime() const;

    /**
     * @brief Set the interval at which dateTime() is updated and timeChanged() is Q_EMITted.
     * @param seconds The interval in seconds
     * @see updateInterval
     */
    void setUpdateInterval(int seconds);

    /**
     * @brief Returns the interval at which dateTime() is updated and timeChanged() is Q_EMITted,
     * The default is 60 seconds.
     * @return The interval in seconds.
     * @see setUpdateInterval
     */
    int updateInterval() const;

    /**
     * @brief Sets the speed of the timer which is how fast the marble clock can run relative to actual speed of time.
     * @param speed The new speed (integer)
     **/
    void setSpeed(int speed);

    /**
     * @brief Returns the speed of the timer
     **/
    int speed() const;

    /**
     * @brief Sets the timezone of the clock
     * @param timeInSec The new timezone ( in seconds w.r.t. UTC )
     **/
    void setTimezone(int timeInSec);

    /**
     * @brief Returns the timezone of the clock
     **/
    int timezone() const;

private:

    Q_DISABLE_COPY(MarbleClock)

    Q_PRIVATE_SLOT(d,  void timerTimeout())

private:

    MarbleClockPrivate* const d = nullptr;

    friend class MarbleClockPrivate;
};

} // Namespace Marble
