/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-07
 * Description : a tool to create panorama by fusion of several images.
 *
 * Copyright (C) 2015-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef DIGIKAM_COMMAND_TASK_H
#define DIGIKAM_COMMAND_TASK_H

// Qt includes

#include <QProcess>

// Local includes

#include "panotask.h"

namespace DigikamGenericPanoramaPlugin
{

class CommandTask : public PanoTask
{
public:

    explicit CommandTask(PanoAction action, const QString& workDirPath, const QString& commandPath);
    ~CommandTask()          override = default;

    void    requestAbort()  override;

protected:

    QString output;

protected:

    void    runProcess(QStringList& args);
    QString getProgram();
    QString getCommandLine();
    QString getProcessError();
    void    printDebug(const QString& binaryName);

private:

    QSharedPointer<QProcess> process;
    QString                  commandPath;

private:

    // Disable
    CommandTask();

    Q_DISABLE_COPY(CommandTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_COMMAND_TASK_H
