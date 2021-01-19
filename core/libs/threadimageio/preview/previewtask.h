/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-26
 * Description : Multithreaded loader for previews
 *
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_PREVIEW_TASK_H
#define DIGIKAM_PREVIEW_TASK_H

// Qt includes

#include <QImage>

// Local includes

#include "loadsavetask.h"
#include "metaengine_previews.h"

namespace Digikam
{

class PreviewLoadingTask : public SharedLoadingTask
{
public:

    explicit PreviewLoadingTask(LoadSaveThread* const thread, const LoadingDescription& description);
    ~PreviewLoadingTask() override;

    void execute() override;

private:

    bool loadExiv2Preview(MetaEnginePreviews& previews, int sizeLimit = -1);
    bool loadLibRawPreview(int sizeLimit = -1);
    bool loadHalfSizeRaw();
    bool needToScale();
    bool loadImagePreview(int sizeLimit = -1);
    void convertQImageToDImg();

private:

    QImage m_qimage;
    bool   m_fromRawEmbeddedPreview;

private:

    // Hidden copy constructor and assignment operator.
    PreviewLoadingTask(const PreviewLoadingTask&);
    PreviewLoadingTask& operator=(const PreviewLoadingTask&);
};

} // namespace Digikam

#endif // DIGIKAM_PREVIEW_TASK_H
