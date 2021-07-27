/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-06
 * Description : Thread actions container.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_ACTIONS_H
#define DIGIKAM_BQM_ACTIONS_H

// Qt includes

#include <QString>
#include <QMetaType>
#include <QUrl>

namespace Digikam
{

class ActionData
{

public:

    enum ActionStatus
    {
        None = 0,
        BatchStarted,
        BatchDone,
        BatchFailed,
        BatchCanceled,
        TaskDone,
        TaskFailed,
        TaskCanceled
    };

public:

    explicit ActionData()
        : status(None)
    {
    }

    ActionStatus status;

    QString      message;

    QUrl         fileUrl;
    QUrl         destUrl;
};

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::ActionData)

#endif // DIGIKAM_BQM_ACTIONS_H
