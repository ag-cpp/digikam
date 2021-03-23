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

#ifndef DIGIKAM_CREATE_PREVIEW_TASK_H
#define DIGIKAM_CREATE_PREVIEW_TASK_H

// Qt includes

#include <QPointer>

// Local includes

#include "panotask.h"
#include "ptotype.h"
#include "metaengine.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class CreatePreviewTask : public PanoTask
{
public:

    explicit CreatePreviewTask(const QString& workDirPath,
                               QSharedPointer<const PTOType> inputPTO,
                               QUrl& previewPtoUrl,
                               const PanoramaItemUrlsMap& preProcessedUrlsMap);
    ~CreatePreviewTask() override;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread* thread) override;

private:

    QUrl&                         previewPtoUrl;
    QSharedPointer<const PTOType> ptoData;
    const PanoramaItemUrlsMap     preProcessedUrlsMap;
    MetaEngine                    m_meta;

private:

    // Disable
    CreatePreviewTask() = delete;

    Q_DISABLE_COPY(CreatePreviewTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_CREATE_PREVIEW_TASK_H
