/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a tool to create panorama by fusion of several images.
 *
 * Copyright (C) 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef DIGIKAM_PANO_TASK_H
#define DIGIKAM_PANO_TASK_H

// KDE includes

#include <ThreadWeaver/Job>

// Local includes

#include "panoactions.h"

namespace DigikamGenericPanoramaPlugin
{

class PanoTask : public ThreadWeaver::Job
{
public:

    explicit PanoTask(PanoAction action, const QString& workDirPath);
    ~PanoTask()             override = default;

public:

    bool success() const    override;
    void requestAbort()     override;

public:

    QString          errString;
    const PanoAction action;
    bool             isAbortedFlag;

protected:

    bool             successFlag;
    const QUrl       tmpDir;

private:

    // Disable
    PanoTask() = delete;

    Q_DISABLE_COPY(PanoTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_PANO_TASK_H
