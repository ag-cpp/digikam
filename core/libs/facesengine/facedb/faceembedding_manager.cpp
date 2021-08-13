/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-08-07
 * Description : Manage Face embedding retrieval and assignment
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020-2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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

#include "faceembedding_manager.h"

#include <QMutex>

#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "facedbaccess.h"
#include "facedb.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceEmbeddingManager::Private
{
public:

    explicit Private()
        : mutex (QMutex::Recursive)
    {
        DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
        params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
        FaceDbAccess::setParameters(params);

        dbAvailable = FaceDbAccess::checkReadyForUse(nullptr);
    }

    ~Private()
    {
    }

public:

public:

    bool                        dbAvailable;
    mutable QMutex              mutex;
};

FaceEmbeddingManager::FaceEmbeddingManager()
    : d(new Private())
{
}

FaceEmbeddingManager::~FaceEmbeddingManager()
{
    delete d;
}

void FaceEmbeddingManager::saveEmbedding(const cv::Mat& faceEmbedding,
                                         const QString& tagID) const
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);
    FaceDbAccess().db()->insertFaceVector(faceEmbedding, tagID);
}

QVector<FaceEmbeddingData> FaceEmbeddingManager::getFaceEmbeddings() const
{
    if (!d || !d->dbAvailable)
    {
        return {};
    }

    QMutexLocker lock(&d->mutex);
    
    return FaceDbAccess().db()->faceVectors();
}

}