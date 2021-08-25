/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-08-22
 * Description : Caching face embedding
 *
 * Copyright (C) 2021      by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "faceembeddingcache.h"

#include <QVector>
#include <QMutexLocker>

#include "faceembedding_manager.h"
#include "dimension_reducer.h"

namespace Digikam
{

QVector<FaceEmbeddingData>& reduceDimension(QVector<FaceEmbeddingData>& data, int nbCPU)
{
    cv::Mat embeddings;

    for (int i = 0; i < data.size(); ++i)
    {
        embeddings.push_back(data[i].embedding);
    }

    cv::Mat projectedEmbedings = Digikam::DimensionReducer::reduceDimension(embeddings, 2, nbCPU);

    for (int i = 0; i < data.size(); ++i)
    {
        data[i].embedding = projectedEmbedings.row(i);
    }

    return data;
}

FaceEmbeddingCache* FaceEmbeddingCache::s_instance = nullptr;
QMutex FaceEmbeddingCache::s_mutex;

FaceEmbeddingCache::FaceEmbeddingCache()
    : m_isValid(false)
{
}

FaceEmbeddingCache::~FaceEmbeddingCache()
{
}

void FaceEmbeddingCache::init()
{
    QVector<FaceEmbeddingData> data = FaceEmbeddingManager().getFaceEmbeddings();
    // NOTE: if the data set is large enough, T-SNE projection will take a very long time. 
    // Plus, when the data is large enough, we can avoid the curse of dimensionality, thus there is no need to project data
    if (data.size() < 13000)
    {
        data = reduceDimension(data, 4);
    }

    for (int i = 0; i < data.size(); ++i)
    {
        s_instance->m_faceembeddingMap[data[i].tagId] = data[i];
    }
}

void FaceEmbeddingCache::invalidCache()
{
    QMutexLocker locker(&s_mutex);

    if (!s_instance)
    {
        return;
    }
    
    s_instance->m_isValid = false;
}

QMap<QString, FaceEmbeddingData> FaceEmbeddingCache::getData()
{
    QMutexLocker locker(&s_mutex);

    if (!s_instance)
    {
        s_instance = new FaceEmbeddingCache();
    }

    if (!s_instance->m_isValid)
    {
        init();
        s_instance->m_isValid = true;
    }
    
    return s_instance->m_faceembeddingMap;
}


} //namespace Digikam
