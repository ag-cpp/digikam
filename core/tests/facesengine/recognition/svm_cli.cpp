/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-06-16
 * Description : Testing tool for SVM classifier of face engines
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

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDebug>
#include "digikam_opencv.h"


// --------------------------------------------------------

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("data"), QLatin1String("Data file"), QLatin1String("path relative to data file")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}

cv::Ptr<cv::ml::TrainData> loadData(QString fileName) 
{
    cv::Mat predictors, labels;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return cv::ml::TrainData::create(predictors, 0, labels);
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> data = line.split(',');

        cv::Mat predictor(1, data.size()-1, CV_32F);
        for (int i = 1; i < data.size(); ++i) 
        {
            predictor.at<float>(i-1) = data[i].toFloat();
        }

        labels.push_back(data[0].toInt());
        predictors.push_back(predictor);
    }

    return cv::ml::TrainData::create(predictors, 0, labels);
}

double test(cv::Ptr<cv::ml::TrainData> data)
{
    data->setTrainTestSplitRatio(0.8);

    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setKernel(cv::ml::SVM::LINEAR);

    svm->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    double error = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {
        int prediction = svm->predict(data->getTestSamples().row(i));
        if (prediction != data->getTestResponses().row(i).at<int>(0)) 
        {
            qDebug() << prediction << " != " << data->getTestResponses().row(i).at<int>(0);
            ++error;
        }
    }

    return error / data->getTestSamples().rows;
}


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    QCommandLineParser* parser = parseOptions(app);

    qDebug() << "Error rate" << test(loadData(parser->value(QLatin1String("data"))));

    delete parser;

    return 0;
}

