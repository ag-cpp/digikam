/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-07
 * Description : Integrated, multithread face embedding extraction
 *
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

#include "extractionworker.h"

#include <QList>
#include <QVector>

// Local includes
#include "dnnfaceextractor.h"
#include "faceutils.h"
#include "iteminfo.h"
#include "facetagsiface.h"
#include "digikam_debug.h"
#include "util/asyncbuffer.h"

namespace Digikam
{

QList<QImage> crop(const QImage& faceImg, const QList<QRect>& detectedFaces)
{
    if (detectedFaces.isEmpty())
    {
        return {};
    }

    QList<QImage> croppedFaces;

    for (int i = 0; i < detectedFaces.size(); ++i) 
    {
        QImage face;
        face = faceImg.copy(detectedFaces[i]);

        croppedFaces.append(face);
    }

    return croppedFaces;
}

cv::Mat prepareForRecognition(QImage& inputImage)
{
    cv::Mat cvImageWrapper;

    if (inputImage.format() != QImage::Format_ARGB32_Premultiplied)
    {
        inputImage = inputImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4, inputImage.scanLine(0), inputImage.bytesPerLine());
    cv::Mat cvImage;    // = cv::Mat(image.height(), image.width(), CV_8UC3);
    cv::cvtColor(cvImageWrapper, cvImage, CV_BGR2RGB);

    return cvImage;
}

QList<FaceTagsIface> saveDetectedRects(const ItemInfo& info, 
                                       const QSize& imgSize, 
                                       const QList<QRectF>& detectedFaces, 
                                       bool overwriteUnconfirmed)
{
    if (info.isNull() || detectedFaces.isEmpty())
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

        // TODO facesengine: remove associated embedding
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

struct Q_DECL_HIDDEN FacesPackage
{
public:

    FacesPackage(const ItemInfo& info, const QImage& image, const QList<QRectF>& detectedFaces)
        : info(info),
          image(image),
          detectedFaces(detectedFaces)
    {
    }

public:

    ItemInfo info;
    QImage image;
    QList<QRectF> detectedFaces;
};

class Q_DECL_HIDDEN ExtractionWorker::Private 
{
public:

    explicit Private(int nbExtractor, bool overwriteUnconfirmed)
        : buffer(1000),
          overwriteUnconfirmed(overwriteUnconfirmed)
    {
        for (int i = 0 ; i < nbExtractor; ++i)
        {
            extractors << new DNNFaceExtractor;
        }
    }

    ~Private()
    {
        QVector<DNNFaceExtractor*>::iterator extractor = extractors.begin();

        while (extractor != extractors.end())
        {
            delete *extractor;
            extractor = extractors.erase(extractor);
        }
    }

public:

    QVector<DNNFaceExtractor*> extractors;
    AsyncBuffer<FacesPackage> buffer;
    bool overwriteUnconfirmed;

public:

    class ParallelExtractors : public cv::ParallelLoopBody
    {
    public:

        ParallelExtractors(ExtractionWorker::Private* d,
                        QList<QImage>& images,
                        QVector<cv::Mat>& embeddings)
            : images     (images),
            embeddings (embeddings),
            d          (d)
        {
            embeddings.resize(images.size());
        }

        void operator()(const cv::Range& range) const override
        {
            for(int i = range.start ; i < range.end ; ++i)
            {
                embeddings[i] = d->extractors[i%(d->extractors.size())]->getFaceEmbedding(prepareForRecognition(images[i]));
            }
        }

    private:

        QList<QImage>&                   images;
        QVector<cv::Mat>&                embeddings;

        ExtractionWorker::Private* const d;

    private:

        Q_DISABLE_COPY(ParallelExtractors)
    };
};

ExtractionWorker::ExtractionWorker(bool overwriteUnconfirmed)
    : d (new Private(1, overwriteUnconfirmed))
{
}

ExtractionWorker::~ExtractionWorker()
{
    // TODO wait for finish
    delete d;
}

void ExtractionWorker::run()
{
    while (!m_cancel)
    {
        FacesPackage package = d->buffer.read();

        QList<FaceTagsIface> tags = saveDetectedRects(package.info, 
                                                      package.image.size(),
                                                      package.detectedFaces,
                                                      d->overwriteUnconfirmed);

        qDebug() << "save" << tags.size() << "detected";

        QVector<int> tagIDs;
        QList<QRect> detectedFaces;

        for (int i = 0; i < tags.size(); ++i)
        {
            tagIDs.append(tags[i].tagId());
            detectedFaces.append(tags[i].region().toRect());
        }

        QVector<cv::Mat> embeddings;    
        QList<QImage> croppedFaces = crop(package.image, detectedFaces);

        cv::parallel_for_(cv::Range(0, croppedFaces.size()), Private::ParallelExtractors(d, croppedFaces, embeddings));

        qDebug() << "Extract" << embeddings.size() << "embedding from" << package.info.filePath();

        emit embeddingExtracted(embeddings, tagIDs);
    }
}

void ExtractionWorker::process(const ItemInfo& info, const QImage& image, const QList<QRectF>& detectedFaces)
{
    d->buffer.append(FacesPackage(info, image, detectedFaces));
}

} // namespace Digikam
