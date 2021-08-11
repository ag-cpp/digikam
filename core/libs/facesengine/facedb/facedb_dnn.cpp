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

namespace Digikam
{

void FaceDb::insertFaceVector(const cv::Mat& faceEmbedding,
                              const int tagID,
                              const QString& context) const
{    
    QVariantList bindingValues;

    bindingValues << -1;
    bindingValues << context;
    bindingValues << QByteArray::fromRawData((char*)faceEmbedding.ptr<float>(), (sizeof(float) * faceEmbedding.cols));

    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("INSERT INTO FaceMatrices (identity, `context`, embedding) "
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

    int insertedID = query.lastInsertId().toInt();

    bindingValues.clear();
    bindingValues << tagID;
    bindingValues << insertedID;
    // TODO facesengine: find out how to get the unique ID of detected face region
    query = d->db->execQuery(QLatin1String("INSERT INTO FaceReferences (tagId, matriceId) "
                                            "VALUES (?,?);"),
                                            bindingValues);

    if(!query.exec())
    {
        qCWarning(DIGIKAM_FACEDB_LOG) << "fail to insert face reference, last query"
                                      << query.lastQuery()
                                      << "bound values" << query.boundValues()
                                      << query.lastError();
    }
}

cv::Ptr<cv::ml::TrainData> FaceDb::trainData() const
{
    cv::Mat feature, label;
    DbEngineSqlQuery query = d->db->execQuery(QLatin1String("SELECT identity, embedding "
                                                            "FROM FaceMatrices;"));

    while (query.next())
    {
        label.push_back(query.value(0).toInt());
        feature.push_back(cv::Mat(1, 128, CV_32F, query.value(1).toByteArray().data()).clone());
    }

    return cv::ml::TrainData::create(feature, 0, label);
}

void FaceDb::clearDNNTraining(const QString& context)
{
    if (context.isNull())
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices;"));
    }
    else
    {
        d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE `context`=?;"),
                       context);
    }
}

void FaceDb::clearDNNTraining(const QList<int>& identities, const QString& context)
{
    foreach (int id, identities)
    {
        if (context.isNull())
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=?;"),
                           id);
        }
        else
        {
            d->db->execSql(QLatin1String("DELETE FROM FaceMatrices WHERE identity=? AND `context`=?;"),
                           id, context);
        }
    }
}

} // namespace Digikam
