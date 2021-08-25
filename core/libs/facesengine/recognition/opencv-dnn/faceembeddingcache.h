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

#ifndef FACE_EMBEDDING_CACHE_H
#define FACE_EMBEDDING_CACHE_H

#include <QMap>
#include <QMutex>

// Local includes
#include "digikam_opencv.h"

namespace Digikam
{

struct Q_DECL_HIDDEN FaceEmbeddingData
{
    int id;
    cv::Mat embedding;
    int identity;
    QString tagId;
};

class Q_DECL_HIDDEN FaceEmbeddingCache
{
public:

    static void invalidCache();
    static QMap<QString, FaceEmbeddingData> getData();

private:

    static void init();

private:

    FaceEmbeddingCache();
    ~FaceEmbeddingCache();

private:

    QMap<QString, FaceEmbeddingData> m_faceembeddingMap;
    bool m_isValid;

private:

    static QMutex s_mutex;
    static FaceEmbeddingCache* s_instance;
};

} // namespace Digikam

#endif // FACE_EMBEDDING_CACHE_H
