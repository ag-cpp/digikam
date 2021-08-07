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

namespace Digikam
{

class Q_DECL_HIDDEN ExtractionWorker::Private 
{
public:

    explicit Private(int nbExtractor)
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

public:
    class ParallelExtractors;
};

class ExtractionWorker::Private::ParallelExtractors : public cv::ParallelLoopBody
{
public:

    ParallelExtractors(ExtractionWorker::Private* d,
                       const QList<QImage*>& images,
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
            // TODO
            //embeddings[i] = d->extractors[i%(d->extractors.size())]->getFaceEmbedding();
        }
    }

private:

    const QList<QImage*>&                   images;
    QVector<cv::Mat>&                       embeddings;

    ExtractionWorker::Private* const d;

private:

    Q_DISABLE_COPY(ParallelExtractors)
};


ExtractionWorker::ExtractionWorker()
    : d (new Private(1))
{
}

ExtractionWorker::~ExtractionWorker()
{
    // TODO wait for finish
    delete d;
}

void ExtractionWorker::process()
{
   
}

} // namespace Digikam
