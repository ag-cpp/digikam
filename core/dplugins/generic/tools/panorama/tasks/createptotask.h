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

#ifndef DIGIKAM_CREATE_PTO_TASK_H
#define DIGIKAM_CREATE_PTO_TASK_H

// Qt includes

#include <QPointer>

// Local includes

#include "panotask.h"
#include "metaengine.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class CreatePtoTask : public PanoTask
{
public:

    explicit CreatePtoTask(const QString& workDirPath,
                           PanoramaFileType fileType,
                           QUrl& ptoUrl,
                           const QList<QUrl>& inputFiles,
                           const PanoramaItemUrlsMap& preProcessedMap,
                           bool addGPlusMetadata,
                           const QString& huginVersion);
    ~CreatePtoTask() override;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread* thread) override;

private:

    QUrl&                            ptoUrl;
    const PanoramaItemUrlsMap* const preProcessedMap;
    const PanoramaFileType           fileType;
    const QList<QUrl>&               inputFiles;
    const bool                       addGPlusMetadata;
    const QString&                   huginVersion;
    MetaEngine                       m_meta;

private:

    // Disable
    CreatePtoTask();

    Q_DISABLE_COPY(CreatePtoTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_CREATE_PTO_TASK_H
