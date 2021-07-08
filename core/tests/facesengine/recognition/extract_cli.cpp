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
#include <algorithm> 
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QChar>
#include <QStringList>
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
    parser->addOption(QCommandLineOption(QLatin1String("out"), QLatin1String("Output file"), QLatin1String("path relative to output file")));
    parser->addHelpOption();
    parser->process(app);

    return std::shared_ptr<QCommandLineParser>(parser);
}

class Extractor {
public:
    explicit Extractor() {
        m_detector = new Digikam::FaceDetector();
        m_preprocessor = new Digikam::RecognitionPreprocessor;
        m_preprocessor->init(Digikam::PreprocessorSelection::OPENFACE);
        m_net = cv::dnn::readNetFromTensorflow("../scripts/facenet_opencv_dnn/models/graph_final.pb");
    }

    QImage* detect(const QImage& faceImg) const;
    cv::Mat getFaceEmbedding(cv::Mat faceImage);

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

cv::Mat normalize(cv::Mat mat) 
{
    cv::Mat floatMat;
    mat.convertTo(floatMat, CV_32F);

    cv::Mat subtractedMat;
    cv::subtract(floatMat, cv::Scalar(127.5,127.5,127.5), subtractedMat);

    cv::Mat normalizedMat;
    cv::multiply(subtractedMat, cv::Scalar(1.0/127.5,1.0/127.5,1.0/127.5), normalizedMat);

    return normalizedMat;
}


cv::Mat Extractor::getFaceEmbedding(cv::Mat faceImage)
{
    cv::Size imageSize = cv::Size(160, 160);
    /*
    cv::Mat resizedImage;
    cv::resize(faceImage.clone(), resizedImage, imageSize, 0, 0, cv::INTER_LINEAR);

    cv::Scalar mean, std;
    cv::meanStdDev(resizedImage, mean, std);

    std::cout << "Mean " << mean << "std " << std << std::endl;
    */
    cv::Mat blob = cv::dnn::blobFromImage(faceImage, 1.0/127.5, imageSize, cv::Scalar(127.5,127.5,127.5), true, false);
    m_net.setInput(blob);
    cv::Mat face_descriptors = m_net.forward();
    return face_descriptors;
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

cv::Ptr<cv::ml::TrainData> extract(const QDir& dataDir) {
    cv::Mat predictors, labels;

    Extractor extractor;
    // Each subdirectory in data directory should match with a label
    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

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


void save(cv::Ptr<cv::ml::TrainData> data, const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << file.errorString();
        return;
    }

    QTextStream streamOut(&file);

    cv::Mat samples = data->getSamples();
    cv::Mat labels = data->getResponses();

    for (int i = 0; i < samples.rows; ++i) 
    {
        QStringList line;
        line << QString::number(labels.row(i).at<int>(0));

        for(int j=0; j<samples.row(i).cols; ++j)
        {
            line << QString::number(double(samples.row(i).at<float>(j)));
        } 

        streamOut << line.join(QLatin1Char(',')) << "\n"; 
    }
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    QDir dataset(parser->value(QLatin1String("data")));

    save(extract(dataset), parser->value(QLatin1String("out")));

    return 0;
}
