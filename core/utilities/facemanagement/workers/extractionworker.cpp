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
namespace Digikam
{

template <typename T>
class Q_DECL_HIDDEN AsyncBuffer 
{
public:

    explicit AsyncBuffer(int capacity);
    ~AsyncBuffer();

public:

    T read();
    void append(const T& object);

private:

    QQueue<T> m_data;
    QMutex m_mutex;
    QWaitCondition m_readWait;
    QWaitCondition m_writeWait;
    const int m_capacity;
};

template <typename T>
AsyncBuffer<T>::AsyncBuffer(int capacity)
    : m_capacity(capacity)
{
    Q_ASSERT(capacity > 0);   
}

template <typename T>
AsyncBuffer<T>::~AsyncBuffer()
{
    m_writeWait.wakeAll();
    m_readWait.wakeAll();
}

template <typename T>
T AsyncBuffer<T>::read()
{
    m_mutex.lock();

    while (m_data.empty())
    {
        m_readWait.wait(&m_mutex);
    }

    T object = m_data.dequeue();

    m_mutex.unlock();
    m_writeWait.wakeAll();

    return object;
}

template <typename T>
void AsyncBuffer<T>::append(const T& object)
{
    m_mutex.lock();

    while (m_data.size() >= m_capacity)
    {
        m_writeWait.wait(&m_mutex);
    }

    m_data.enqueue(object);

    m_mutex.unlock();
    m_readWait.wakeAll();
}

QList<QImage> crop(const DImg& faceImg, const QList<QRectF>& detectedFaces)
{
    if (detectedFaces.isEmpty())
    {
        return {};
    }

    QList<QImage> croppedFaces;

    for (int i = 0; i < detectedFaces.size(); ++i) 
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

QString encodeTagID(int imageID, const FaceTagsIface& tag)
{
    return QString::number(imageID) + QLatin1Char('-') + tag.region().toXml();
}

class Q_DECL_HIDDEN ExtractionWorker::Private 
{
public:

    explicit Private(int nbExtractor)
        : buffer(1000),
          cancel(false)
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
    AsyncBuffer<FacePipelineExtendedPackage::Ptr> buffer;
    FaceEmbeddingManager db;
    bool cancel;

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
      d (new Private(1))
{
}

ExtractionWorker::~ExtractionWorker()
{
    cancel();
    delete d;
}

void ExtractionWorker::run()
{
    while (!d->cancel)
    {
        FacePipelineExtendedPackage::Ptr package = d->buffer.read();

        QVector<QString> tagIDs;

        for (int i = 0; i < package->databaseFaces.size(); ++i)
        {
            tagIDs.append(encodeTagID(package->info.id(), package->databaseFaces[i]));
        }

        QVector<cv::Mat> embeddings;    
        QList<QImage> croppedFaces = crop(package->image, package->detectedFaces);

        cv::parallel_for_(cv::Range(0, croppedFaces.size()), Private::ParallelExtractors(d, croppedFaces, embeddings));

        qDebug() << "Extract" << embeddings.size() << "embedding from" << package->info.filePath();

        for (int i = 0; i < embeddings  .size(); ++i)
        {
            qDebug() << "Save";
            d->db.saveEmbedding(embeddings[i], tagIDs[i]);
        }

        emit processed(package);
    }
}

void ExtractionWorker::process(FacePipelineExtendedPackage::Ptr package)
{
    d->buffer.append(package);
}

void ExtractionWorker::cancel()
{
    d->cancel = true;
}

} // namespace Digikam

