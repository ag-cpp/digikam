/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-11
 * Description : Integrated, multithread face recognition
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

#ifndef DIGIKAM_RECOGNITION_WORKER_H
#define DIGIKAM_RECOGNITION_WORKER_H

#include <QVector>

#include "actionthreadbase.h"
#include "iteminfo.h"
#include "digikam_opencv.h"

namespace Digikam
{

class FaceEmbeddingData;

class Q_DECL_HIDDEN RecognitionWorker: public ActionJob
{
    Q_OBJECT
public:

    explicit RecognitionWorker(const QVector<FaceEmbeddingData>& data, QObject* parent = nullptr);
    ~RecognitionWorker();

public:

    void run() override;

public:

private:

    cv::Mat reduceDimension(cv::Mat data, int nbCPU) const;

private:

    class Private;
    Private* d;

    // Disable
    RecognitionWorker(const RecognitionWorker&)            = delete;
    RecognitionWorker& operator=(const RecognitionWorker&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_RECOGNITION_WORKER_H
