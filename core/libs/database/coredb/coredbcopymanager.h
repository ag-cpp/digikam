/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : Core database copy manager for migration operations.
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CORE_DB_COPY_MANAGER_H
#define DIGIKAM_CORE_DB_COPY_MANAGER_H

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "coredbbackend.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT CoreDbCopyManager : public QObject
{
    Q_OBJECT

public:

    enum FinishStates
    {
        success,
        failed,
        canceled
    };

public:

    CoreDbCopyManager();
    ~CoreDbCopyManager() override;

    void copyDatabases(const DbEngineParameters& fromDBParameters,
                       const DbEngineParameters& toDBParameters);

Q_SIGNALS:

    void stepStarted(const QString& stepName);
    void smallStepStarted(int currValue, int maxValue);
    void finished(int finishState, const QString& errorMsg);

public Q_SLOTS:

    void stopProcessing();

private:

    // Disabled
    explicit CoreDbCopyManager(QObject*) = delete;

    bool copyTable(CoreDbBackend& fromDBbackend,
                   const QString& fromActionName,
                   CoreDbBackend& toDBbackend,
                   const QString& toActionName);

    void handleClosing(bool isstopThread,
                       CoreDbBackend& fromDBbackend,
                       CoreDbBackend& toDBbackend);

private:

    volatile bool m_isStopProcessing;
};

} // namespace Digikam

#endif // DIGIKAM_CORE_DB_COPY_MANAGER_H
