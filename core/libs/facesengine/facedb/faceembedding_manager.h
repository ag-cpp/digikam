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

#ifndef FACE_EMBEDDING_MANAGER_H
#define FACE_EMBEDDING_MANAGER_H

// Qt includes
#include <QList>

// Local includes
#include "digikam_export.h"
#include "identity.h"
#include "digikam_opencv.h"

namespace Digikam
{

class FaceEmbeddingData;

class DIGIKAM_GUI_EXPORT FaceEmbeddingManager
{
public:

    explicit FaceEmbeddingManager();
    ~FaceEmbeddingManager();

public:

    /**
     * Save face embeddings extracted from face images
     */
    void saveEmbedding(const cv::Mat& faceEmbedding,
                       const QString& tagID) const;

    /**
     * Retrieve face embeddings with their labels
     */
    QVector<FaceEmbeddingData> getFaceEmbeddings() const;

    // TODO facesengine 4 review delete saved data
    /**
     * Deletes the training data for all identities,
     * leaving the identities as such in the database.
     */
    void clearAllEmbedding(const QString& tagId = QString());

    /**
     * Deletes the training data for the given identity,
     * leaving the identity as such in the database.
     */
    void clearEmbedding(const QList<Identity>& identitiesToClean,
                        const QString& tagId = QString());

private:

    // Disable
    FaceEmbeddingManager& operator=(const FaceEmbeddingManager&) = delete;
    FaceEmbeddingManager(const FaceEmbeddingManager&) = delete;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif // FACE_EMBEDDING_MANAGER_H