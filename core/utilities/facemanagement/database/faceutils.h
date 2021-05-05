/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-08
 * Description : FacesEngine database interface allowing easy manipulation of face tags
 *
 * Copyright (C) 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACE_UTILS_H
#define DIGIKAM_FACE_UTILS_H

// Qt includes

#include <QStringList>

// Local includes

#include "identity.h"
#include "facialrecognition_wrapper.h"
#include "iteminfo.h"
#include "facetagseditor.h"
#include "digikam_export.h"

class QImage;

namespace Digikam
{

class DImg;
class ThumbnailLoadThread;
class ThumbnailImageCatcher;

class FaceUtils : public QObject,
                  public FaceTagsEditor
{
    Q_OBJECT

public:

    enum FaceRecognitionSteps
    {
        DetectFaceRegions,
        DetectAndRecognize
    };

public:

    explicit FaceUtils(QObject* const parent = nullptr);
    ~FaceUtils()                                                                              override;

    // --- Face detection and recognition ---

    /**
     * The given face list is a result of automatic detection and possibly recognition.
     * The results are written to the database and merged with existing entries.
     * The returned list contains the faces written to the database and has the same size as the given list.
     * If a face was skipped (because of an existing entry), a null FaceTagsIface will be at this place.
     */
    QList<FaceTagsIface> writeUnconfirmedResults(qlonglong imageid,
                                                 const QList<QRectF>& detectedFaces,
                                                 const QList<Identity>& recognitionResults,
                                                 const QSize& fullSize);

    // --- Status flags ---

    /**
     * Tells if the image has been scanned for faces or not
     */
    bool                hasBeenScanned(const ItemInfo& info)                            const;
    bool                hasBeenScanned(qlonglong imageid)                               const;

    /**
     * Marks the image as scanned for faces.
     */
    void                markAsScanned(qlonglong imageid, bool hasBeenScanned = true)    const;
    void                markAsScanned(const ItemInfo& info, bool hasBeenScanned = true) const;

    // --- Utilities ---

    /**
     * This uses a thumbnail load thread to load the image detail.
     * If requested, the faces will be scaled to the given (fixed) size.
     */
/*
    void                fillImageInFaces(ThumbnailImageCatcher* const catcher,
                                         const QString& filePath,
                                         QList<Face>& faceList,
                                         const QSize& scaleSize = QSize())              const;
*/

    /**
     * Store the needed thumbnails for the given faces. This can be a huge optimization
     * when the has already been loaded anyway.
     */
    void                storeThumbnails(ThumbnailLoadThread* const thread,
                                        const QString& filePath,
                                        const QList<FaceTagsIface>& databaseFaces,
                                        const DImg& image);

    /**
     * Conversion
     */
    QList<FaceTagsIface> toFaceTagsIfaces(qlonglong imageid,
                                         const QList<QRectF>& detectedFaces,
                                         const QList<Identity>& recognitionResults,
                                         const QSize& fullSize)                         const;

    /**
     * Rotate face tags
     */
    QSize                rotateFaces(const ItemInfo& info, int newOrientation,
                                                           int oldOrientation);

    /**
     * For display, it may be desirable to display a slightly larger region than the strict
     * face rectangle. This returns a pixel margin commonly used to increase the rectangle size
     * in all four directions.
     */
    static int          faceRectDisplayMargin(const QRect& rect);

    // TODO: investigate this method
    Identity identityForTag(int tagId, FacialRecognitionWrapper& recognizer)            const;
    int      tagForIdentity(const Identity& identity)                                   const;

protected:

    // Reimplemented
    void addNormalTag(qlonglong imageid, int tagId)                                           override;
    void removeNormalTag(qlonglong imageid, int tagId)                                        override;
    void removeNormalTags(qlonglong imageid, const QList<int>& tagId)                         override;

private:

    // Disable
    FaceUtils(const FaceUtils&)            = delete;
    FaceUtils& operator=(const FaceUtils&) = delete;
};

} // Namespace Digikam

#endif // DIGIKAM_FACE_UTILS_H
