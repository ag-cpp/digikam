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
#include "detectionworker.h"
#include "extractionworker.h"
#include "databasewriter.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionPipeline::Private
{
public:

    explicit Private()
    {
    }

    ~Private()
    {
    }

public:

    bool scanAll;
    DetectionWorker*  detector;
    ExtractionWorker* extractor;
    DatabaseWriter*   writer;
};

DetectionPipeline::DetectionPipeline(bool scanAll, bool overWrite, QObject* parent)
    : ActionThreadBase(parent),
      d(new Private())
{
    d->scanAll   = scanAll;
    d->detector  = new DetectionWorker();
    /*
    d->extractor = new ExtractionWorker(overWrite);
    d->writer    = new DatabaseWriter(1);

    qRegisterMetaType< QList<QRectF> >("QList<QRectF>");
    qRegisterMetaType< QVector<int> >("QVector<int>");
    qRegisterMetaType< QVector<cv::Mat> >("QVector<cv::Mat>");

    connect(d->detector, SIGNAL(faceDetected(const ItemInfo&, const QImage&, const QList<QRectF>&)),
            d->extractor, SLOT(process(const ItemInfo&, const QImage&, const QList<QRectF>&)), Qt::DirectConnection);

    connect(d->extractor, SIGNAL(embeddingExtracted(const QVector<cv::Mat>&, const QVector<int>&)),
            d->writer, SLOT(saveExtractedFaceEmbeddings(const QVector<cv::Mat>&, const QVector<int>&)), Qt::DirectConnection);

    connect(d->writer, SIGNAL(saved(int)),
            this, SIGNAL(processed(int)), Qt::DirectConnection);
    */        

    ActionJobCollection jobs;
    jobs[d->detector]   = 1;
    /*
    jobs[d->extractor]  = 1;
    jobs[d->writer]     = 1;
    */
    appendJobs(jobs);
}

DetectionPipeline::~DetectionPipeline()
{
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
            emit processed(1);
            continue;
        }

        qDebug() << info[i].fileUrl(); 
        d->detector->process(info[i]);
    }
}

}