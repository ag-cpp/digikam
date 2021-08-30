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

// Qt includes

#include <QList>
#include <QVector>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

// Local includes

#include "dnnfaceextractor.h"
#include "faceutils.h"
#include "iteminfo.h"
#include "facetagsiface.h"
#include "digikam_debug.h"
#include "faceembedding_manager.h"
#include "asyncbuffer.h"

namespace Digikam
{

QList<QImage> crop(const DImg& faceImg, const QList<QRectF>& detectedFaces)
{
    if (detectedFaces.isEmpty())
    {
        return {};
    }

    QList<QImage> croppedFaces;

    for (int i = 0 ; i < detectedFaces.size() ; ++i)
    {
        QImage face;
        face = faceImg.copyQImage(detectedFaces[i]);

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

class Q_DECL_HIDDEN ExtractionWorker::Private 
{
public:

    explicit Private(int nbExtractor)
        : buffer(100)
    {
        for (int i = 0 ; i < nbExtractor ; ++i)
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
    AsyncBuffer<FacePipelineExtendedPackage::Ptr> buffer;
    FaceEmbeddingManager db;

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

ExtractionWorker::ExtractionWorker()
    : QThread(),
      d      (new Private(1))
{
}

ExtractionWorker::~ExtractionWorker()
{
    cancel();
    wait();

    delete d;
}

void ExtractionWorker::run()
{
    while (true)
    {
        FacePipelineExtendedPackage::Ptr package = d->buffer.read();

        if (package == nullptr)
        {
            break;
        }

        QVector<QString> tagIDs;

        for (int i = 0 ; i < package->databaseFaces.size() ; ++i)
        {
            tagIDs.append(encodeTagID(package->info.id(), package->databaseFaces[i]));
        }

        QList<QImage> croppedFaces = crop(package->image, package->detectedFaces);
        extract(tagIDs, croppedFaces);

        emit processed(package);
    }

    emit canceled();
}

void ExtractionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    d->buffer.append(package);
}

void ExtractionWorker::cancel()
{
    d->buffer.cancel();
}

void ExtractionWorker::extract(const QVector<QString>& tagIDs, QList<QImage>& faces, const QVector<int>& identities) const
{
    Q_ASSERT(tagIDs.size() == faces.size());

    QVector<cv::Mat> embeddings;
    cv::parallel_for_(cv::Range(0, faces.size()), Private::ParallelExtractors(d, faces, embeddings));

    for (int i = 0 ; i < embeddings.size() ; ++i)
    {
        int id = -1;
        if (identities.size() > i)
        {
            id = identities[i];
        }

        d->db.saveEmbedding(embeddings[i], tagIDs[i], id);
    }
}

QString ExtractionWorker::encodeTagID(int imageID, const FaceTagsIface& tag)
{
    return QString::number(imageID) + QLatin1Char('-') + tag.region().toXml();
}

} // namespace Digikam

