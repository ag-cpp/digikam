/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2017-07-13
 * Description : Face recognition using deep learning
 *
 * Copyright (C) 2017      by Yingjie Liu <yingjiewudi at gmail dot com>
 * Copyright (C) 2017-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#include "opencvdnnfacerecognizer.h"

// Qt includes

#include <QTime>

// Local includes

#include "facedbaccess.h"
#include "facedb.h"
#include "dnnfacemodel.h"
#include "digikam_debug.h"
#include "dnnfaceextractor.h"
#include "recognitionpreprocessor.h"
#include "dbscan.h"

namespace Digikam
{

float OpenCVDNNFaceRecognizer::m_threshold = 15000.0;

class Q_DECL_HIDDEN OpenCVDNNFaceRecognizer::Private
{
public:

    explicit Private(bool debug)
        : loaded(false),
          debug(debug)
    {
        m_preprocessor = new RecognitionPreprocessor;
        m_preprocessor->init(PreprocessorSelection::OPENFACE);

        m_extractor    = new DNNFaceExtractor(m_preprocessor);
    }

    ~Private()
    {
        delete m_preprocessor;
        delete m_extractor;
    }

public:

    DNNFaceModel& dnn()
    {
        if (!loaded)
        {
            m_dnn  = FaceDbAccess().db()->dnnFaceModel(debug);
            loaded = true;
        }

        return m_dnn;
    }

private:

    bool         loaded;
    bool         debug;

    DNNFaceModel m_dnn;

public:

    RecognitionPreprocessor* m_preprocessor;
    DNNFaceExtractor* m_extractor;

};

// ----------------------------------------------------------------------------

OpenCVDNNFaceRecognizer::OpenCVDNNFaceRecognizer(bool debug)
    : d(new Private(debug))
{
}

OpenCVDNNFaceRecognizer::~OpenCVDNNFaceRecognizer()
{
    delete d;
}

namespace
{
    enum
    {
        TargetInputSize = 256
    };
}

cv::Mat OpenCVDNNFaceRecognizer::prepareForRecognition(const QImage& inputImage)
{
    QImage image(inputImage);

    if (inputImage.width() > TargetInputSize || inputImage.height() > TargetInputSize)
    {
        image = inputImage.scaled(TargetInputSize, TargetInputSize, Qt::IgnoreAspectRatio);
    }

    cv::Mat cvImage;// = cv::Mat(image.height(), image.width(), CV_8UC3);
    cv::Mat cvImageWrapper;

    switch (image.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // I think we can ignore premultiplication when converting to grayscale
            cvImageWrapper = cv::Mat(image.height(), image.width(), CV_8UC4, image.scanLine(0), image.bytesPerLine());
            cvtColor(cvImageWrapper, cvImage, CV_RGBA2RGB);
            break;
        default:
            image          = image.convertToFormat(QImage::Format_RGB888);
            cvImage        = cv::Mat(image.height(), image.width(), CV_8UC3, image.scanLine(0), image.bytesPerLine());
            //cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);
            break;
    }

    //resize(cvImage, cvImage, Size(256, 256), (0, 0), (0, 0), INTER_LINEAR);
    //equalizeHist(cvImage, cvImage);
    return cvImage;
}

int OpenCVDNNFaceRecognizer::recognize(const cv::Mat& inputImage)
{
    int predictedLabel = -1;
    double confidence  = 0;
    d->dnn()->predict(inputImage, predictedLabel, confidence, d->m_extractor);
    qCDebug(DIGIKAM_FACESENGINE_LOG) << predictedLabel << confidence;

    /** confidence must be greater than threshold, because distance used is cosine distance
     * in case that we use euclidean distance, confidence must be less than threshold
     */
    if (confidence < m_threshold)
    {
        return -1;
    }

    return predictedLabel;
}

void OpenCVDNNFaceRecognizer::cluster(const std::vector<cv::Mat>& images,
                                      std::vector<int>& clusteredIndices,
                                      QStringList dataset,
                                      int nbOfClusters)
{

    d->dnn();

/**
 * Flann (error while running)

    // Compute face embeddings of all images

    cv::Mat_<float> faceEmbeddings(images.size(), 128);
    int row = 0;

    for (const cv::Mat& image: images)
    {
        std::vector<float> faceEmbedding;
        d->m_extractor->getFaceEmbedding(image, faceEmbedding);

        for (int col = 0 ; col < 128 ; ++col)
        {
            faceEmbeddings.at<float>(row, col) = faceEmbedding[col];
        }

        row++;
    }

    // Construct index for flann search

    cvflann::KMeansIndexParams indexParams;

    QTime timer;
    timer.start();
    cv::flann::GenericIndex<cvflann::L2<float>> flannIndex(faceEmbeddings, indexParams);
    qDebug() << "time to build flann index " << timer.elapsed() << " ms";

    cv::Mat_<float> centers(10,128);
    timer.start();
    cv::flann::hierarchicalClustering<cvflann::L2<float>>(faceEmbeddings, centers, indexParams);
    qDebug() << "time to compute cluster centers " << timer.elapsed() << " ms";

    // std::vector<std::vector<float>> centerVectors;
    // cvMatToStdVector(centers, centerVectors);

    // Search for nearest neighbors around center
    cv::Mat indices, dists;
    float radius = 0.01;
    timer.start();
    flannIndex.radiusSearch(centers, indices, dists, radius, cvflann::SearchParams());
    qDebug() << "time to search for nearest neighbors " << timer.elapsed() << " ms";
*/
/**
 * k-means opencv
 * Not well clustered

    cv::Mat_<float> faceEmbeddings(images.size(), 128), centers(images.size(), 128);
    int row = 0;

    for (const cv::Mat& image: images)
    {
        std::vector<float> faceEmbedding;
        d->m_extractor->getFaceEmbedding(image, faceEmbedding);

        for (int col = 0 ; col < 128 ; ++col)
        {
            faceEmbeddings.at<float>(row, col) = faceEmbedding[col];
        }

        row++;
    }

    std::vector<int> labels(clusteredIndices.size());
    double compactness = cv::kmeans(faceEmbeddings, nbOfClusters, labels, cv::TermCriteria(cv::TermCriteria::Type::MAX_ITER | cv::TermCriteria::Type::EPS, 10000, 0.00001),
                                    10, cv::KmeansFlags::KMEANS_PP_CENTERS, centers);

    std::vector<QStringList> groups(nbOfClusters);

    for (int i = 0 ; i < labels.size() ; ++i)
    {
        groups[labels[i]] << dataset[i];
    }

    for (int i = 0 ; i < groups.size() ; ++i)
    {
        qDebug() << "Group " << i;
        foreach(const QString& image, groups[i])
        {
            qDebug() << image;
        }
    }

    qDebug() << "N Groups = " << groups.size();
    qDebug() << "N LAbels = " << labels.size();

    std::copy(labels.begin(), labels.end(), clusteredIndices.begin());

//*/

/**
 * DBSCAN clustering
 */
    std::vector<PointCustomized> faceEmbeddings;

    for (const cv::Mat& image: images)
    {
        std::vector<float> faceEmbedding;
        d->m_extractor->getFaceEmbedding(image, faceEmbedding);
        faceEmbeddings.push_back(PointCustomized(faceEmbedding));
    }

    DBSCAN dbscan(m_threshold, 3, faceEmbeddings);
    QTime timer;
    timer.start();
    dbscan.run();
    qDebug() << "time to run dbscan " << timer.elapsed() << " ms";
    int nClusters = dbscan.getCluster(clusteredIndices);

    std::vector<QStringList> groups(nClusters);

    qDebug() << "N Groups = " << groups.size();
    qDebug() << "N LAbels = " << clusteredIndices.size();

    for (int i = 0 ; i < clusteredIndices.size() ; ++i)
    {
        // qDebug() << "Image " << i << " in group " << clusteredIndices[i];
        groups[clusteredIndices[i]] << dataset[i];
    }

    for (int i = 0 ;  i < groups.size() ; ++i)
    {
        qDebug() << "Group " << i;

        foreach (const QString& image, groups[i])
        {
            qDebug() << image;
        }
    }
//*/

}

void OpenCVDNNFaceRecognizer::train(const std::vector<cv::Mat>& images,
                                    const std::vector<int>& labels,
                                    const QString& context,
                                    const std::vector<cv::Mat>& images_rgb)
{
    if (images.empty() || labels.size() != images.size())
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "DNN Train: nothing to train...";
        return;
    }

    d->dnn().update(images_rgb, labels, context, d->m_extractor);
    qCDebug(DIGIKAM_FACESENGINE_LOG) << "DNN Train: Adding model to Facedb";

    // add to database waiting only when not in mode debug
    if (!context.contains(QLatin1String("debug"), Qt::CaseInsensitive) &&
        !context.contains(QLatin1String("test"), Qt::CaseInsensitive))
    {
        FaceDbAccess().db()->updateDNNFaceModel(d->dnn());
    }
}

} // namespace Digikam
