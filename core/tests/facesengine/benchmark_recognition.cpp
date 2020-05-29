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

// Qt includes
#include <QApplication>
#include <QCommandLineParser>
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QLabel>
#include <QPen>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>

#include <QHash>


// lib digikam includes
#include "opencvdnnfacedetector.h"
#include "facedetector.h"
#include "faceextractor.h"
#include "facerecognizer.h"
#include "identity.h"

using namespace Digikam;
using namespace RecognitionTest;

// TODO: Recognition is incorrect where human are wearing glasses
// TODO: Verify cosnine distance Vs L2 distance



/**
 * @brief The Benchmark class: a benchmark especially made for testing the performance of face recognition module
 * on faces images. The dataset should contain only face data (un-cropped or cropped). It's not built for other purposes.
 *
 * If the faces are already cropped, the photo will be passed directly the FaceRecognizer for face recognition.
 * If the faces are un-cropped in original image, it will pass through FaceDetector before it landing on Facerecognizer
 *
 * These 2 types of images can be easily detected by FaceDetector because FaceDetector cannot detected already cropped face.
 */
class Benchmark : public QObject
{
    Q_OBJECT
public:

    Benchmark();
    ~Benchmark();

public:

    void registerTrainingSet();

    void verifyTestSet();

    QVector<QListWidgetItem*> splitData(const QDir& dataDir, float splitRatio);

public:

    float m_error;
    int   m_trainSize;
    int   m_testSize;

private:

    cv::Mat preprocess(QImage* faceImg);

private:

    QHash<QString, QVector<QImage*> > m_trainSet;
    QHash<QString, QVector<QImage*> > m_testSet;

    OpenCVDNNFaceDetector* m_detector;
    FaceRecognizer*        m_recognizer;
};

Benchmark::Benchmark()
    : QObject(),
      m_error(-1),
      m_trainSize(0),
      m_testSize(0)
{
    m_detector   = new OpenCVDNNFaceDetector(DetectorNNModel::YOLO);
    m_recognizer = new FaceRecognizer(true);
}

Benchmark::~Benchmark()
{
    QImage* img;

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_trainSet.begin();
                                                     iter != m_trainSet.end();
                                                   ++iter)
    {
        while ((img = iter.value().at(0)) != nullptr)
        {
            delete img;
        }
    }

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_testSet.begin();
                                                     iter != m_testSet.end();
                                                   ++iter)
    {
        while ((img = iter.value().at(0)) != nullptr)
        {
            delete img;
        }
    }

    delete m_detector;
    delete m_recognizer;
}

void Benchmark::registerTrainingSet()
{
    m_trainSize = 0;

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_trainSet.begin();
                                                     iter != m_trainSet.end();
                                                   ++iter)
    {
        for (int i = 0; i < iter.value().size(); ++i)
        {
            Identity newIdentity = m_recognizer->newIdentity(preprocess(iter.value().at(i)));

            newIdentity.setAttribute(QLatin1String("fullName"), iter.key());

            m_recognizer->saveIdentity(newIdentity);

            ++m_trainSize;
        }
    }
}

void Benchmark::verifyTestSet()
{
    int nbError = 0;
    m_testSize = 0;

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_trainSet.begin();
                                                     iter != m_trainSet.end();
                                                   ++iter)
    {
        for (int i = 0; i < iter.value().size(); ++i)
        {
            Identity newIdentity = m_recognizer->findIdenity(preprocess(iter.value().at(i)));

            if (newIdentity.isNull() && m_trainSet.contains(iter.key()))
            {
                // cannot recognize when label is already register
                ++nbError;
            }
            else if (newIdentity.attribute(QLatin1String("fullName")) != iter.key())
            {
                // wrong label
                ++nbError;
            }

            ++m_testSize;
        }
    }

    if (m_testSize == 0)
    {
        qWarning() << "test set is empty";

        return;
    }

    m_error = float(nbError)/m_testSize;

    qDebug() << "Error" << m_error;
}

cv::Mat Benchmark::preprocess(QImage* faceImg)
{
    QList<QRectF> faces;

    try
    {
        QElapsedTimer timer;
        unsigned int elapsedDetection = 0;
        timer.start();

        // NOTE detection with filePath won't work when format is not standard
        // NOTE unexpected behaviour with detecFaces(const QString&)
        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = m_detector->prepareForDetection(*faceImg, paddedSize);
        QList<QRect> absRects = m_detector->detectFaces(cvImage, paddedSize);
        faces                 = FaceDetector::toRelativeRects(absRects,
                                                              QSize(cvImage.cols - 2*paddedSize.width,
                                                              cvImage.rows - 2*paddedSize.height));
        elapsedDetection = timer.elapsed();

        qDebug() << "(Input CV) Found " << absRects.size() << " faces, in " << elapsedDetection << "ms";
    }
    catch (cv::Exception& e)
    {
        qWarning() << "cv::Exception:" << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from OpenCV";
    }

    if (faces.isEmpty())
    {
        return (m_recognizer->prepareForRecognition(*faceImg));
    }

    QRect rect = FaceDetector::toAbsoluteRect(faces[0], faceImg->size());

    return (m_recognizer->prepareForRecognition(faceImg->copy(rect)));
}

QVector<QListWidgetItem*> Benchmark::splitData(const QDir& dataDir, float splitRatio)
{
    qsrand(QTime::currentTime().msec());

    QVector<QListWidgetItem*> imageItems;

    // Each subdirectory in data directory should match with a label
    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    for (int i = 0; i < subDirs.size(); ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());

        QString label = subDirs[i].fileName();

        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);

        // suffle dataset
        QList<QFileInfo>::iterator it = filesInfo.begin();
        QList<QFileInfo>::iterator it1;

        for (int i = 0; i < filesInfo.size(); ++i)
        {
            int inc = (int) (float(filesInfo.size()) * qrand() / (RAND_MAX + 1.0));

            it1     = filesInfo.begin();
            it1    += inc;

            std::swap(*(it++), *(it1));
         }

        // split train/test
        for (int i = 0; i < filesInfo.size(); ++i)
        {
            QImage* img = new QImage(filesInfo[i].absoluteFilePath());

            if (i < filesInfo.size() * splitRatio)
            {
                if (! img->isNull())
                {
                    m_trainSet[label].append(img);
                    imageItems.append(new QListWidgetItem(QIcon(filesInfo[i].absoluteFilePath()), filesInfo[i].absoluteFilePath()));
                }
            }
            else
            {
                if (! img->isNull())
                {
                    m_testSet[label].append(img);
                    imageItems.append(new QListWidgetItem(QIcon(filesInfo[i].absoluteFilePath()), filesInfo[i].absoluteFilePath()));
                }
            }
        }
    }

    return imageItems;
}


QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addOption(QCommandLineOption(QLatin1String("split"), QLatin1String("Split ratio"), QLatin1String("split ratio of training data")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));

    // Options for commandline parser

   QCommandLineParser* parser = parseOptions(app);

   if (! parser->isSet(QLatin1String("dataset")))
   {
       qWarning("Data set is not set !!!");

       return 1;
   }

   QDir dataset(parser->value(QLatin1String("dataset")));

   float splitRatio = 0.8;
   if (parser->isSet(QLatin1String("split")))
   {
       splitRatio = parser->value(QLatin1String("split")).toFloat();
   }

   Benchmark test;

   test.splitData(dataset, splitRatio);

   test.registerTrainingSet();
   test.verifyTestSet();

   qDebug() << "Test Finish, recognition error :" << test.m_error
            << "on total" << test.m_trainSize << "training faces, and"
                          << test.m_testSize << "test faces";

   return app.exec();
}

#include "benchmark_recognition.moc"
