/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-08
 * Description : Async face detection pipeline
 *
 * Copyright (C)      2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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

#include "detectionpipeline.h"

#include <QDebug>

#include "faceutils.h"
#include "util/asyncbuffer.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionPipeline::Private
{
public:

    explicit Private()
        : scanAll(true),
          overWrite(true)
    {
    }

    ~Private()
    {
    }

public:

    bool scanAll;
    bool overWrite;
};

DetectionPipeline::DetectionPipeline(bool scanAll, bool overWrite)
    : d(new Private())
{
}

DetectionPipeline::~DetectionPipeline()
{
    delete d;
}

bool filter(const ItemInfo& info)
{
    FaceUtils utils;
    if (!utils.hasBeenScanned(info))
    {
        return false;
    }

    return true;
}

void DetectionPipeline::process(const QList<ItemInfo>& info) 
{
    // TODO assemble pipeline
    /*
        d->pipeline.plugDatabaseFilter(filterMode);
        d->pipeline.plugFacePreviewLoader();

        if (settings.useFullCpu)
        {
            d->pipeline.plugParallelFaceDetectors();
        }
        else
        {
            d->pipeline.plugFaceDetector();
        }

        d->pipeline.plugDatabaseWriter(writeMode);
        d->pipeline.setAccuracyAndModel(settings.accuracy,
                                        settings.useYoloV3);
        d->pipeline.construct();
    */

    for (int i = 0; i < info.size(); ++i)
    {
        if (!d->scanAll && filter(info[i]))
        {
            emit processed();
            continue;
        }

        qDebug() << info[i].fileUrl(); 
    }
}

void DetectionPipeline::cancel()
{
    // TODO
}



}