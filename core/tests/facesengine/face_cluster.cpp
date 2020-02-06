/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-08-10
 * Description : CLI tool to test and verify clustering for Face Recognition
 *
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

// C++ includes

#include <cassert>
#include <set>

// Qt includes

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QDebug>
#include <QCommandLineParser>
#include <QList>

// Local includes

#include "dimg.h"
#include "facescansettings.h"
#include "facedetector.h"
#include "recognitiondatabase.h"
#include "coredbaccess.h"
#include "dbengineparameters.h"

using namespace Digikam;

// --------------------------------------------------------------------------------------------------

/**
 * Function to return the
 * intersection vector of v1 and v2
 */
void intersection(const std::vector<int>& v1,
                  const std::vector<int>& v2,
                  std::vector<int>& vout)
{
    // Find the intersection of the two sets

    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),
                          std::inserter(vout, vout.begin()));
}

// Function to return the Jaccard distance of two vectors
double jaccard_distance(const std::vector<int>& v1,
                        const std::vector<int>& v2)
{
    // Sizes of both the sets

    double size_v1       = v1.size();
    double size_v2       = v2.size();

    // Get the intersection set

    std::vector<int> intersect;
    intersection(v1, v2, intersect);

    // Size of the intersection set

    double size_in       = intersect.size();

    // Calculate the Jaccard index
    // using the formula

    double jaccard_index = size_in / (size_v1 + size_v2 - size_in);

    // Calculate the Jaccard distance
    // using the formula

    double jaccard_dist  = 1 - jaccard_index;

    // Return the Jaccard distance

    return jaccard_dist;
}

QStringList toPaths(char** argv, int startIndex, int argc)
{
    QStringList files;

    for (int i = startIndex ; i < argc ; ++i)
    {
        files << QString::fromLatin1(argv[i]);
    }

    return files;
}

QList<QImage> toImages(const QStringList& paths)
{
    QList<QImage> images;

    foreach (const QString& path, paths)
    {
        images << QImage(path);
    }

    return images;
}

int prepareForTrain(QString datasetPath,
                    QStringList& images,
                    std::vector<int>& testClusteredIndices)
{
    if (!datasetPath.endsWith(QLatin1String("/")))
    {
        datasetPath.append(QLatin1String("/"));
    }

    QDir testSet(datasetPath);
    QStringList subjects = testSet.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    int nbOfClusters = subjects.size();

    qDebug() << "Number of clusters to be defined" << nbOfClusters;

    for (int i = 1 ; i <= nbOfClusters ; ++i)
    {
        QString subjectPath               = QString::fromLatin1("%1%2")
                                                     .arg(datasetPath)
                                                     .arg(subjects.takeFirst());
        QDir subjectDir(subjectPath);

        QStringList files                 = subjectDir.entryList(QDir::Files);
        unsigned int nbOfFacesPerClusters = files.size();

        for (unsigned j = 1 ; j <= nbOfFacesPerClusters ; ++j)
        {
            QString path = QString::fromLatin1("%1/%2").arg(subjectPath)
                                                       .arg(files.takeFirst());

            testClusteredIndices.push_back(i - 1);
            images << path;
        }
    }

    qDebug() << "nbOfClusters (prepareForTrain) " << nbOfClusters;

    return nbOfClusters;
}

QList<QRectF> processFaceDetection(const QString& imagePath, FaceDetector detector)
{
    QList<QRectF> detectedFaces = detector.detectFaces(imagePath);

    qDebug() << "(Input CV) Found " << detectedFaces.size() << " faces";

    return detectedFaces;
}

QList<QImage> retrieveFaces(const QList<QImage>& images, const QList<QRectF>& rects)
{
    QList<QImage> faces;
    unsigned index = 0;

    foreach (const QRectF& rect, rects)
    {
        DImg temp(images.at(index));
        faces << temp.copyQImage(rect);
        ++index;
    }

    return faces;
}

void createClustersFromClusterIndices(const std::vector<int>& clusteredIndices,
                                      QList<std::vector<int>>& clusters)
{
    int nbOfClusters = 0;

    for (size_t i = 0 ; i < clusteredIndices.size() ; ++i)
    {
        int nb = clusteredIndices[i];

        if (nb > nbOfClusters)
        {
            nbOfClusters = nb;
        }
    }

    nbOfClusters++;

    for (int i = 0 ; i < nbOfClusters ; ++i)
    {
        clusters << std::vector<int>();
    }

    qDebug() << "nbOfClusters " << clusters.size();

    for (size_t i = 0 ; i < clusteredIndices.size() ; ++i)
    {
        clusters[clusteredIndices[(int)i]].push_back(i);
    }
}

void verifyClusteringResults(const std::vector<int>& clusteredIndices,
                             const std::vector<int>& testClusteredIndices,
                             const QStringList& dataset,
                             QStringList& falsePositiveCases)
{
    QList<std::vector<int>> clusters, testClusters;
    createClustersFromClusterIndices(clusteredIndices, clusters);
    createClustersFromClusterIndices(testClusteredIndices, testClusters);

    std::set<int> falsePositivePoints;
    int testClustersSize = testClusters.size();
    std::vector<float> visited(testClustersSize, 1.0);
    std::vector<std::set<int>> lastVisit(testClustersSize, std::set<int>{});

    for (int i = 0 ; i < testClustersSize ; ++i)
    {
        std::vector<int> refSet = testClusters.at(i);
        double minDist          = 1.0;
        int indice              = 0;

        for (int j = 0 ; j < clusters.size() ; ++j)
        {
            double dist = jaccard_distance(refSet, clusters.at(j));

            if (dist < minDist)
            {
                indice  = j;
                minDist = dist;
            }
        }

        qDebug() << "testCluster " << i << " with group " << indice;

        std::vector<int> similarSet = clusters.at(indice);

        if (minDist < visited[indice])
        {
            visited[indice]            = minDist;
            std::set<int> lastVisitSet = lastVisit[indice];
            std::set<int> newVisitSet;
            std::set_symmetric_difference(refSet.begin(), refSet.end(), similarSet.begin(), similarSet.end(), 
                                          std::inserter(newVisitSet, newVisitSet.begin()));

            for (int elm: lastVisitSet)
            {
                falsePositivePoints.erase(elm);
            }

            lastVisit[indice] = newVisitSet;
            falsePositivePoints.insert(newVisitSet.begin(), newVisitSet.end());
        }
        else
        {
            std::set_intersection(refSet.begin(), refSet.end(), similarSet.begin(), similarSet.end(), 
                                  std::inserter(falsePositivePoints, falsePositivePoints.begin()));
        }
    }

    for (auto indx: falsePositivePoints)
    {
        falsePositiveCases << dataset[indx];
    }
}

// --------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    // Options for commandline parser

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QLatin1String("db"),
                     QLatin1String("Faces database"),
                     QLatin1String("path to db folder")));
    parser.addHelpOption();
    parser.process(app);

    // Parse arguments

    bool optionErrors = false;

    if (parser.optionNames().empty())
    {
        qWarning() << "No options!!!";
        optionErrors = true;
    }
    else if (!parser.isSet(QLatin1String("db")))
    {
        qWarning() << "Missing database for test!!!";
        optionErrors = true;
    }

    if (optionErrors)
    {
        parser.showHelp();
        return 1;
    }

    QString facedb = parser.value(QLatin1String("db"));

    // Init config for digiKam

    DbEngineParameters prm = DbEngineParameters::parametersFromConfig();
    CoreDbAccess::setParameters(prm, CoreDbAccess::MainApplication);
    RecognitionDatabase db;
    db.activeFaceRecognizer(RecognitionDatabase::RecognizeAlgorithm::DNN);

    db.setRecognizerThreshold(0.91F);       // This is sensitive for the performance of face clustering

    // Construct test set, data set

    QStringList dataset;
    std::vector<int> testClusteredIndices;
    int nbOfClusters = prepareForTrain(facedb, dataset, testClusteredIndices);

    // Init FaceDetector used for detecting faces and bounding box
    // before recognizing

    FaceDetector detector;

    // Evaluation metrics

    unsigned totalClustered    = 0;
    unsigned elapsedClustering = 0;

    QStringList undetectedFaces;

    QList<QImage> detectedFaces;
    QList<QRectF> bboxes;
    QList<QImage> rawImages = toImages(dataset);

    foreach (const QImage& image, rawImages)
    {
        QString imagePath                 = dataset.takeFirst();
        QList<QRectF> detectedBoundingBox = processFaceDetection(imagePath, detector);

        if (detectedBoundingBox.size())
        {
            detectedFaces << image;
            bboxes        << detectedBoundingBox.first();
            dataset       << imagePath;

            ++totalClustered;
        }
        else
        {
            undetectedFaces << imagePath;
        }
    }

    std::vector<int> clusteredIndices(dataset.size(), -1);
    QList<QImage> faces = retrieveFaces(detectedFaces, bboxes);

    QElapsedTimer timer;

    timer.start();
    db.clusterFaces(faces, clusteredIndices, dataset, nbOfClusters);
    elapsedClustering  += timer.elapsed();

    // Verify clustering

    QStringList falsePositiveCases;
    verifyClusteringResults(clusteredIndices, testClusteredIndices, dataset, falsePositiveCases);

    // Display results

    unsigned nbUndetectedFaces = undetectedFaces.size();
    qDebug() << "\n" << nbUndetectedFaces << " / " << dataset.size() + nbUndetectedFaces
             << " (" << float(nbUndetectedFaces) / (dataset.size() + nbUndetectedFaces) * 100 << "%)"
             << " faces cannot be detected";

    foreach (const QString& path, undetectedFaces)
    {
        qDebug() << path;
    }

    unsigned nbOfFalsePositiveCases = falsePositiveCases.size();
    qDebug() << "\nFalse positive cases";
    qDebug() << "\n" << nbOfFalsePositiveCases << " / " << dataset.size()
             << " (" << float(nbOfFalsePositiveCases*100) / dataset.size()<< "%)"
             << " faces were wrongly clustered";

    foreach (const QString& imagePath, falsePositiveCases)
    {
        qDebug() << imagePath;
    }

    qDebug() << "\n Time for clustering " << elapsedClustering << " ms";

    return 0;
}
