/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Database writer for facial detection pipeline
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

#include "databasewriter.h"

// Local includes
#include "faceembedding_manager.h"
#include "util/asyncbuffer.h"

namespace Digikam
{

struct DataPackage
{
    DataPackage(cv::Mat embedding, int tagID)
        : embedding(embedding),
          tagID(tagID)
    {
    }

    cv::Mat embedding;
    int tagID;
};

class Q_DECL_HIDDEN DatabaseWriter::Private
{
public:

    explicit Private(int batchSize)
        : buffer(1000),
          batchSize(batchSize)
    {
    }

    ~Private()
    {
    }

public:

    FaceEmbeddingManager db;
    AsyncBuffer<DataPackage> buffer;
    int batchSize;
};

DatabaseWriter::DatabaseWriter(int batchSize)
    : d(new Private(batchSize))
{
}

DatabaseWriter::~DatabaseWriter()
{
    delete d;
}

void DatabaseWriter::run()
{
    QVector<cv::Mat> tempEmbeddings;
    QVector<int> tempTagIDs;

    while (!m_cancel)
    {
        DataPackage package = d->buffer.read();
        tempEmbeddings.append(package.embedding);
        tempTagIDs.append(package.tagID);

        if (tempEmbeddings.size() >= d->batchSize)
        {
            d->db.saveEmbeddings(tempEmbeddings, tempTagIDs, QLatin1String("Face detection"));
            tempEmbeddings.clear();
            tempTagIDs.clear();

            emit saved(d->batchSize);
        }
    }

    // flush out data
    d->db.saveEmbeddings(tempEmbeddings, tempTagIDs, QLatin1String("Face detection"));
    emit saved(tempEmbeddings.size());
}

void DatabaseWriter::saveExtractedFaceEmbeddings(const QVector<cv::Mat>& faceEmbeddings,
                                                 const QVector<int>&     facetagIds)
{
    for (int i = 0; i < faceEmbeddings.size(); ++i)
    {
        d->buffer.append(DataPackage(faceEmbeddings[i], facetagIds[i]));
    }
}

} // namespace Digikam
