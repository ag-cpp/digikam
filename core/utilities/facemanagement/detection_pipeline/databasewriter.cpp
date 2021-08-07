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
#include "digikam_debug.h"
#include "faceutils.h"
#include "faceembedding_manager.h"

namespace Digikam
{

class Q_DECL_HIDDEN DatabaseWriter::Private
{
public:

    explicit Private()
    {

    }

    ~Private()
    {
    }

public:

    FaceEmbeddingManager db;
};

DatabaseWriter::DatabaseWriter()
{
}

DatabaseWriter::~DatabaseWriter()
{
}

QList<FaceTagsIface> DatabaseWriter::saveDetectedRects(const ItemInfo& info, 
                                                       const QSize& imgSize, 
                                                       const QList<QRectF>& detectedFaces, 
                                                       bool overwriteUnconfirmed) 
{
    if (!info.isNull() && !detectedFaces.isEmpty())
    {
        return {};
    }

    // Detection / Recognition
    FaceUtils utils;

    // overwriteUnconfirmed means that a new scan discarded unconfirmed results of previous scans
    // (assuming at least equal or new, better methodology is in use compared to the previous scan)

    if (overwriteUnconfirmed)
    {
        QList<FaceTagsIface> oldEntries = utils.unconfirmedFaceTagsIfaces(info.id());
        qCDebug(DIGIKAM_GENERAL_LOG) << "Removing old entries" << oldEntries;
        utils.removeFaces(oldEntries);
    }

    // mark the whole image as scanned-for-faces
    utils.markAsScanned(info);
    
    QList<FaceTagsIface> faceTags = utils.writeUnconfirmedResults(info.id(),
                                                                  detectedFaces,
                                                                  QList<Identity>(),
                                                                  imgSize);

    // TODO facesengine 3: what does Role serve?
    //faceTags.setRole(FacePipelineFaceTagsIface::DetectedFromImage);
        
    // TODO facesengine 3: Review the necessity of thumbnails
    /*
    if (!package->image.isNull())
    {
        utils.storeThumbnails(thumbnailLoadThread, package->filePath,
                              package->databaseFaces.toFaceTagsIfaceList(), package->image);
    }
    */

    return faceTags;
}

void DatabaseWriter::saveExtractedFaceEmbeddings(const QVector<cv::Mat>& faceEmbeddings,
                                                 const QVector<int>&     facetagIds,
                                                 const QString&          context)
{
    d->db.saveEmbeddings(faceEmbeddings, facetagIds, context);
}

} // namespace Digikam
