/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-06-17
 * Description : Extracting tool for Face embedding extraction
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include <memory>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QElapsedTimer>
#include "digikam_opencv.h"
#include "recognitionpreprocessor.h"
#include "opencvdnnfacedetector.h"
#include "facedetector.h"

// --------------------------------------------------------

std::shared_ptr<QCommandLineParser> parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("data"), QLatin1String("Data directory"), QLatin1String("path relative to data directory")));
    parser->addHelpOption();
    parser->process(app);

    return std::shared_ptr<QCommandLineParser>(parser);
}

class Extractor {
public:
    explicit Extractor() {
        m_preprocessor = new Digikam::RecognitionPreprocessor;
        m_preprocessor->init(Digikam::PreprocessorSelection::OPENFACE);
        m_net = cv::dnn::readNetFromTensorflow(
                "/home/minhnghiaduong/Documents/Projects/digikam/core/tests/facesengine/scripts/facenet_opencv_dnn/models/graph_final.pb",
                "/home/minhnghiaduong/Documents/Projects/digikam/core/tests/facesengine/scripts/facenet_opencv_dnn/models/graph_final.pbtxt"
            );
    }

    QImage* detect(const QImage& faceImg) const;
    cv::Mat getFaceEmbedding(const cv::Mat& faceImage);

private:

    Digikam::FaceDetector* m_detector;
    Digikam::RecognitionPreprocessor* m_preprocessor;
    cv::dnn::Net           m_net;
};

QImage* Extractor::detect(const QImage& faceImg) const
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

    QRect rect                = Digikam::FaceDetector::toAbsoluteRect(faces[0], faceImg.size());
    QImage* const croppedFace = new QImage();
    *croppedFace              = faceImg.copy(rect);

    return croppedFace;
}

cv::Mat Extractor::getFaceEmbedding(const cv::Mat& faceImage)
{
    cv::Mat face_descriptors;
    cv::Mat alignedFace;
    QElapsedTimer timer;

    timer.start();
    alignedFace = m_preprocessor->preprocess(faceImage);

    qDebug() << "Finish aligning face in " << timer.elapsed() << " ms";
    qDebug() << "Start neural network";

    timer.start();
    cv::Size imageSize = cv::Size(96, 96);
    float scaleFactor = 1.0F / 255.0F;

    cv::Mat blob = cv::dnn::blobFromImage(alignedFace, scaleFactor, imageSize, cv::Scalar(), true, false);
    m_net.setInput(blob);
    face_descriptors = m_net.forward();

    qDebug() << "Finish computing face embedding in " << timer.elapsed() << " ms";

    return face_descriptors;
}

cv::Mat prepareForRecognition(QImage& inputImage)
{
    cv::Mat cvImage;    // = cv::Mat(image.height(), image.width(), CV_8UC3);
    cv::Mat cvImageWrapper;

    if (inputImage.format() != QImage::Format_ARGB32_Premultiplied)
    {
        inputImage = inputImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    cvImageWrapper = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4, inputImage.scanLine(0), inputImage.bytesPerLine());
    cv::cvtColor(cvImageWrapper, cvImage, CV_RGBA2RGB);

    return cvImage;
}

cv::Ptr<cv::ml::TrainData> extract(const QDir& dataDir) {
    cv::Mat predictors, labels;

    Extractor extractor;
    // Each subdirectory in data directory should match with a label
    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0 ; i < subDirs.size() ; ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());
        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);       

        for (int j = 0 ; j < filesInfo.size() ; ++j)
        {
            QImage img(filesInfo[j].absoluteFilePath());
            QImage* const croppedFace = extractor.detect(img);

            if (croppedFace && !croppedFace->isNull())
            {
                // extract face embedding
                cv::Mat faceEmbedding = extractor.getFaceEmbedding(prepareForRecognition(*croppedFace)); 
                labels.push_back(i);
                predictors.push_back(faceEmbedding);
            }

            delete croppedFace;
        }
    }

    return cv::ml::TrainData::create(predictors, 0, labels);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    return 0;
}
