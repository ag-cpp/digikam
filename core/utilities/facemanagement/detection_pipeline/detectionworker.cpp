/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "detectionworker.h"

#include <QRect>

// Local includes
#include "facedetector.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionWorker::Private
{
public:
    explicit Private()
    {
    }

    ~Private()
    {
    }

public:

    FaceDetector detector;
};

DetectionWorker::DetectionWorker()
    : d(new Private())
{
}

DetectionWorker::~DetectionWorker()
{
    // TODO wait for the job is finished
    delete d;
}

QList<QRectF> DetectionWorker::process(const QImage& image)
{
    if (image.isNull())
    {
        return {};
    }

    QList<QRectF> detectedFaces = d->detector.detectFaces(image);
    qCDebug(DIGIKAM_GENERAL_LOG) << "Found" << detectedFaces.size();

    return detectedFaces;
}

void DetectionWorker::setAccuracyAndModel(double accuracy, bool yolo)
{
    QVariantMap params;
    params[QLatin1String("accuracy")]    = accuracy;
    params[QLatin1String("useyolov3")]   = yolo;
    params[QLatin1String("specificity")] = 0.8;     // TODO: add UI for sensitivity - specificity
    d->detector.setParameters(params);
}

} // namespace Digikam
