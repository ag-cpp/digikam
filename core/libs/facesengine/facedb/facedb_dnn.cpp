/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train face recognizer.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "facedb_p.h"
#include "opencvdnnfacerecognizer.h"

namespace Digikam
{

void FaceDb::insertFaceVector(const cv::Mat& faceEmbedding,
                              const QString& tagID) const
{    
    QVariantList bindingValues;

    bindingValues << -1;
    bindingValues << tagID;
    bindingValues << QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * faceEmbedding.cols));

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("INSERT INTO FaceMatrices (identity, tagId, embedding) "
                                                            "VALUES (?,?,?);"),
                                              bindingValues);

    if (query.lastInsertId().isNull())
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "fail to insert face embedding, last query"
                                      << query.lastQuery()
                                      << "bound values" << query.boundValues()
                                      << query.lastError();
    }
    else
    {
        qCDebug(DIGIKAM_FACEDB_LOG) << "Commit face mat data "
                                    << query.lastInsertId().toInt();
    }
}

QVector<FaceEmbeddingData> FaceDb::faceVectors() const
{
    QVector<FaceEmbeddingData> data;

    cv::Mat feature, label;
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT id, identity, tagId, embedding "
                                                            "FROM FaceMatrices;"));

    while (query.next())
    {   
        FaceEmbeddingData embeddingData;
        embeddingData.id = query.value(0).toInt();
        embeddingData.identity = query.value(1).toInt();
        embeddingData.tagId = query.value(2).toString();
        embeddingData.embedding = cv::Mat(1, 512, CV_32F, query.value(3).toByteArray().data()).clone();

        data.append(embeddingData);
    }

    return data;
}

void FaceDb::clearDNNTraining(const QString& tagId)
{
    if (tagId.isNull())
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices;"));
    }
    else
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE tagId=?;"),
                       tagId);
    }
}

void FaceDb::clearDNNTraining(const QList<int>& identities, const QString& tagId)
{
    foreach (int id, identities)
    {
        if (tagId.isNull())
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=?;"),
                           id);
        }
        else
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=? AND tagId=?;"),
                           id, tagId);
        }
    }
}

} // namespace Digikam
