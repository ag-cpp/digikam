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

#ifndef DIGIKAM_DETECTION_WORKER_H
#define DIGIKAM_DETECTION_WORKER_H

#include "actionthreadbase.h"
#include "iteminfo.h"

namespace Digikam
{

class Q_DECL_HIDDEN DetectionWorker: public ActionJob
{
    Q_OBJECT
public:

    explicit DetectionWorker();
    ~DetectionWorker();

public:

    void run() override;

public Q_SLOTS:

    void process(const ItemInfo& image);

Q_SIGNALS:

    void faceDetected(const ItemInfo&, const QImage&, const QList<QRectF>&);

private:

    class Private;
    Private* d;

    // Disable
    DetectionWorker(const DetectionWorker&)            = delete;
    DetectionWorker& operator=(const DetectionWorker&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_DETECTION_WORKER_H
