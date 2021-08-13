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

#include "recognitionworker.h"
#include "dimension_reducer.h"
#include "opencvdnnfacerecognizer.h"

namespace Digikam
{

class RecognitionWorker::Private
{
public:

    explicit Private()
    {
    }

    ~Private()
    {
    }
};

RecognitionWorker::RecognitionWorker(const QVector<FaceEmbeddingData>& data, QObject* parent)
    : ActionJob(parent),
      d(new Private)
{
}

RecognitionWorker::~RecognitionWorker()
{
    delete d;
}


void RecognitionWorker::run()
{
    while (!m_cancel)
    {
        // TODO
    }
}

cv::Mat RecognitionWorker::reduceDimension(cv::Mat data, int nbCPU) const
{
    return Digikam::DimensionReducer::reduceDimension(data, 2, nbCPU);
}

} // namespace Digikam
