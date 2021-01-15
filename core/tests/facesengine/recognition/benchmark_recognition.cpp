/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-05-20
 * Description : Testing tool for dnn face recognition of face engines
 *
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

// STD include
#include <vector>
#include <sstream>
#include <iterator>

// Qt includes

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QDebug>
#include <QHash>
#include <QTextStream>

// Local includes

#include "opencvdnnfacedetector.h"
#include "facedetector.h"
#include "dnnfaceextractor.h"
#include "opencvdnnfacerecognizer.h"
#include "facialrecognition_wrapper.h"
#include "dbengineparameters.h"
#include "coredbaccess.h"

using namespace Digikam;

/**
 * @brief The Benchmark class: a benchmark especially made for testing the performance of face recognition module
 * on faces images. The dataset should contain only face data. It's not built for other purposes.
 */
class Benchmark : public QObject
{
    Q_OBJECT

public:

    Benchmark(QObject* const parent = nullptr);
    ~Benchmark() override;

public:

    void verifyTestSet();
    void splitData(const QDir& dataDir, float splitRatio);

public:

    QCommandLineParser* m_parser;
    float               m_error;
    int                 m_trainSize;
    int                 m_testSize;

private:

    QImage* detect(const QImage& image)                 const;

    QList<QImage*> detect(const QList<QImage*>& images) const;

    bool preprocess(QImage* faceImg, cv::Mat& face)     const;

public Q_SLOTS:

    void fetchData();
    void registerTrainingSet();
    void saveData();

private:

    QHash<QString, QList<QImage*> > m_trainSet;
    QHash<QString, QList<QImage*> > m_testSet;

    FaceDetector*                   m_detector;
    FacialRecognitionWrapper*       m_recognizer;
};

// --------------------------------------------------------

// NOTE: dnn face detector can be parallelized but it takes longer than a single thread

class ParallelDetector: public cv::ParallelLoopBody
{
public:

    ParallelDetector(FaceDetector* const detector,
                     const QList<QImage*>& images,
                     QVector<QList<QRectF> >& rects)
        : m_detector(detector),
          m_images  (images),
          m_rects   (rects)
    {
        m_rects.resize(images.size());
    }

    void operator()(const cv::Range& range) const override
    {
        for (int i = range.start ; i < range.end ; ++i)
        {
            if (m_images[i]->isNull())
            {
                m_rects[i] = QList<QRectF>();
            }
            else
            {
                m_rects[i] = m_detector->detectFaces(*m_images[i]);
            }
        }
    }

private:

    FaceDetector*            m_detector;
    const QList<QImage*>&    m_images;
    QVector<QList<QRectF> >& m_rects;

private:

    Q_DISABLE_COPY(ParallelDetector)
};

// --------------------------------------------------------

Benchmark::Benchmark(QObject* const parent)
    : QObject    (parent),
      m_parser   (nullptr),
      m_error    (-1),
      m_trainSize(0),
      m_testSize (0)
{
    DbEngineParameters prm = DbEngineParameters::parametersFromConfig();
    CoreDbAccess::setParameters(prm, CoreDbAccess::MainApplication);

    m_detector             = new FaceDetector();
    m_recognizer           = new FacialRecognitionWrapper();

    m_recognizer->clearAllTraining(QLatin1String("train face classifier"));
    m_recognizer->deleteIdentities(m_recognizer->allIdentities());
}

Benchmark::~Benchmark()
{
    delete m_detector;
    delete m_recognizer;
    delete m_parser;
}

void Benchmark::registerTrainingSet()
{
    m_trainSize = 0;

    QElapsedTimer timer;
    timer.start();
/*
    QMap<QString, QString> attributes;
    attributes[QLatin1String("fullName")] = m_trainSet.begin().key();

    Identity newIdentity = m_recognizer->addIdentity(attributes);

    qDebug() << "add new identity to database" << newIdentity.id();

    m_recognizer->train(newIdentity, m_trainSet.begin().value(), QLatin1String("train face classifier"));

    m_trainSize += m_trainSet.begin().value().size();
*/

    for (QHash<QString, QList<QImage*> >::iterator iter  = m_trainSet.begin();
                                                   iter != m_trainSet.end();
                                                   ++iter)
    {
        QMap<QString, QString> attributes;
        attributes[QLatin1String("fullName")] = iter.key();

        Identity newIdentity = m_recognizer->addIdentity(attributes);

        qDebug() << "add new identity to database" << newIdentity.id();

        m_recognizer->train(newIdentity, iter.value(), QLatin1String("train face classifier"));

        m_trainSize += iter.value().size();
    }

    unsigned int elapsedDetection = timer.elapsed();

    qDebug() << "Registered <<  :" << m_trainSize
             << "faces in training set";

    if (m_trainSize != 0)
    {
        qDebug() << "with average" << float(elapsedDetection) / m_trainSize << "ms / face";
    }
}

void Benchmark::verifyTestSet()
{
    int nbNotRecognize = 0;
    int nbWrongLabel   = 0;
    m_testSize         = 0;

    QElapsedTimer timer;
    timer.start();

    for (QHash<QString, QList<QImage*> >::iterator iter  = m_testSet.begin();
                                                   iter != m_testSet.end();
                                                   ++iter)
    {
        QList<Identity> predictions = m_recognizer->recognizeFaces(iter.value());

        for (int i = 0 ; i < predictions.size() ; ++i)
        {
/*
            Identity prediction = m_recognizer->identity(ids[i]);
*/
            if      (predictions[i].isNull())
            {
                if (m_trainSet.contains(iter.key()))
                {
                    // cannot recognize when label is already register

                    ++nbNotRecognize;
                }
            }
            else if (predictions[i].attribute(QLatin1String("fullName")) != iter.key())
            {
                // wrong label

                ++nbWrongLabel;
            }
        }

        m_testSize += iter.value().size();
    }

    unsigned int elapsedDetection = timer.elapsed();

    if (m_testSize == 0)
    {
        qWarning() << "test set is empty";

        return;
    }

    m_error = float(nbNotRecognize + nbWrongLabel)/m_testSize;

    qDebug() << "nb Not Recognized :" << nbNotRecognize;
    qDebug() << "nb Wrong Label :"    << nbWrongLabel;

    qDebug() << "Accuracy :" << (1 - m_error) * 100 << "%"
             << "on total"   << m_trainSize << "training faces, and"
                             << m_testSize << "test faces";
    if (m_testSize != 0)
    {
        qDebug() << "(" << float(elapsedDetection) / m_testSize << "ms/face )";
    }
}

QImage* Benchmark::detect(const QImage& faceImg) const
{
    if (faceImg.isNull())
    {
        return nullptr;
    }

    QList<QRectF> faces = m_detector->detectFaces(faceImg);

    if (faces.isEmpty())
    {
        return nullptr;
    }

    QRect rect                = FaceDetector::toAbsoluteRect(faces[0], faceImg.size());

    QImage* const croppedFace = new QImage();
    *croppedFace              = faceImg.copy(rect);

    return croppedFace;
}

QList<QImage*> Benchmark::detect(const QList<QImage*>& images) const
{
    QVector<QList<QRectF> > faces;

    cv::parallel_for_(cv::Range(0, images.size()), ParallelDetector(m_detector, images, faces));

    QList<QImage*> croppedFaces;

    for (int i = 0 ; i < faces.size() ; ++i)
    {
        if (faces[i].isEmpty())
        {
            croppedFaces << nullptr;
        }
        else
        {
            QRect rect                = FaceDetector::toAbsoluteRect(faces[i][0], images[i]->size());
            QImage* const croppedFace = new QImage();
            *croppedFace              = images[i]->copy(rect);

            croppedFaces << croppedFace;
        }
    }

    return croppedFaces;
}

bool Benchmark::preprocess(QImage* faceImg, cv::Mat& face) const
{
    QList<QRectF> faces = m_detector->detectFaces(*faceImg);

    if (faces.isEmpty())
    {
        return false;
    }

    QRect rect          = FaceDetector::toAbsoluteRect(faces[0], faceImg->size());
    QImage croppedFace  = faceImg->copy(rect);
    cv::Mat cvImageWrapper;

    switch (croppedFace.format())
    {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:

            // I think we can ignore premultiplication when converting to grayscale

            cvImageWrapper = cv::Mat(croppedFace.height(), croppedFace.width(), CV_8UC4, croppedFace.scanLine(0), croppedFace.bytesPerLine());
            cv::cvtColor(cvImageWrapper, face, CV_RGBA2RGB);

            break;

        default:

            croppedFace = croppedFace.convertToFormat(QImage::Format_RGB888);
            face        = cv::Mat(croppedFace.height(), croppedFace.width(), CV_8UC3, croppedFace.scanLine(0), croppedFace.bytesPerLine());
/*
            cvtColor(cvImageWrapper, cvImage, CV_RGB2GRAY);
*/
            break;
    }

    return true;
}

void Benchmark::splitData(const QDir& dataDir, float splitRatio)
{
    int nbData = 0;

    qsrand(QTime::currentTime().msec());

    // Each subdirectory in data directory should match with a label

    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0 ; i < subDirs.size() ; ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());

        QString label = subDirs[i].fileName();

        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);

        // suffle dataset

        QList<QFileInfo>::iterator it = filesInfo.begin();
        QList<QFileInfo>::iterator it1;

        for (int j = 0 ; j < filesInfo.size() ; ++j)
        {
            int inc = (int) (float(filesInfo.size()) * qrand() / (RAND_MAX + 1.0));

            it1     = filesInfo.begin();
            it1    += inc;

            std::swap(*(it++), *(it1));
         }

        //QString faceDir = QLatin1String("./cropped_face/");
/*
        QList<QImage*> images;

        for (int i = 0 ; i < filesInfo.size() ; ++i)
        {
            images << new QImage(filesInfo[i].absoluteFilePath());
        }

        QList<QImage*> croppedFaces = detect(images);

        // split train/test

        for (int i = 0 ; i < croppedFaces.size() ; ++i)
        {
            if (croppedFaces[i])
            {
                croppedFaces[i]->save(faceDir + label + QLatin1String("_") + QString::number(i) + QLatin1String(".png"), "PNG");
            }

            if (i < (filesInfo.size() * splitRatio))
            {
                if (croppedFaces[i] && !croppedFaces[i]->isNull())
                {
                    m_trainSet[label].append(croppedFaces[i]);
                    ++nbData;
                }
            }
            else
            {
                if (croppedFaces[i] && !croppedFaces[i]->isNull())
                {
                    m_testSet[label].append(croppedFaces[i]);
                    ++nbData;
                }
            }
        }
*/
        for (int j = 0 ; j < filesInfo.size() ; ++j)
        {
            QImage img(filesInfo[j].absoluteFilePath());

            QImage* const croppedFace = detect(img);

            // Save cropped face for detection testing

            if (croppedFace)
            {
/*
                croppedFace->save(faceDir + label + QLatin1String("_") + QString::number(i) + QLatin1String(".png"), "PNG");
*/
            }

            if (j < (filesInfo.size() * splitRatio))
            {
                if (croppedFace && !croppedFace->isNull())
                {
                    m_trainSet[label].append(croppedFace);
                    ++nbData;
                }
            }
            else
            {
                if (croppedFace && !croppedFace->isNull())
                {
                    m_testSet[label].append(croppedFace);
                    ++nbData;
                }
            }
        }
    }

    unsigned int elapsedDetection = timer.elapsed();

    qDebug() << "Fetched dataset with" << nbData << "samples";

    if (nbData != 0)
    {
        qDebug() << "with average" << float(elapsedDetection) / nbData << "ms/image.";
    }
}

void Benchmark::fetchData()
{
    if (! m_parser->isSet(QLatin1String("dataset")))
    {
        qWarning("Data set is not set !!!");

        return;
    }

    QDir dataset(m_parser->value(QLatin1String("dataset")));

    float splitRatio = 0.8;

    if (m_parser->isSet(QLatin1String("split")))
    {
        splitRatio = m_parser->value(QLatin1String("split")).toFloat();
    }

    splitData(dataset, splitRatio);
}

void Benchmark::saveData()
{
    if (! m_parser->isSet(QLatin1String("dataset")))
    {
        qWarning("Data set is not set !!!");

        return;
    }

    QDir dataDir(m_parser->value(QLatin1String("dataset")));

    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QFile dataFile(QLatin1String("face_data.txt"));

    if (!dataFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }

    QTextStream stream(&dataFile);

    QElapsedTimer timer;
    timer.start();

    qDebug() << "Scanned " << dataDir.dirName() << " with " << subDirs.size() << "subdirs";

    for (int i = 0 ; i < subDirs.size() ; ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());
        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);
        QList<QImage*> faces;

        for (int j = 0 ; j < filesInfo.size() ; ++j)
        {
            QImage img(filesInfo[j].absoluteFilePath());

            QImage* const croppedFace = detect(img);

            if (croppedFace && !croppedFace->isNull())
            {
                faces.append(croppedFace);
            }
        }

        QList<std::vector<float> > faceEmbeddings = m_recognizer->extractFaceEmbeddings(faces);

        qDebug() << "Subdir " << i << ": " << faces.size();

        for (int j = 0; j < faceEmbeddings.size(); ++j)
        {
            stream << i << ", ";

            std::ostringstream dataEntry;

            std::copy(faceEmbeddings[j].begin(), faceEmbeddings[j].end()-1, std::ostream_iterator<float>(dataEntry, ", "));

            dataEntry << faceEmbeddings[j].back();

            stream << QString::fromStdString(dataEntry.str()) << '\n';

            //qDebug() << i << ", " << QString::fromStdString(dataEntry.str());
        }

        faces.clear();
    }

    qDebug() << "Save face embedding in" << timer.elapsed() << "ms/face";
    dataFile.close();
}

// --------------------------------------------------------

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addOption(QCommandLineOption(QLatin1String("split"), QLatin1String("Split ratio"), QLatin1String("split ratio of training data")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    Benchmark benchmark;
    benchmark.m_parser = parseOptions(app);

    benchmark.saveData();
/*
    benchmark.fetchData();
    benchmark.registerTrainingSet();
    benchmark.verifyTestSet();
*/
    return 0;
}

#include "benchmark_recognition.moc"
