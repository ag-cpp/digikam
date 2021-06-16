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

/**
 * @brief loadData: load data from csv file into a train dataset and a leftout dataset to simulate unknown label
 */ 
std::pair<cv::Ptr<cv::ml::TrainData>, cv::Ptr<cv::ml::TrainData>> loadData(QString fileName) 
{
    cv::Mat predictors, labels;
    cv::Mat leftoutPredictors, leftoutLabels;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return {cv::ml::TrainData::create(predictors, 0, labels),cv::ml::TrainData::create(leftoutPredictors, 0, leftoutLabels)};
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> data = line.split(',');

        cv::Mat predictor(1, data.size()-1, CV_32F);
        for (int i = 1; i < data.size(); ++i) 
        {
            predictor.at<float>(i-1) = data[i].toFloat();
        }

        if (data[0].toInt() == 0) 
        {
            leftoutLabels.push_back(data[0].toInt());
            leftoutPredictors.push_back(predictor);
        }
        else 
        {
            labels.push_back(data[0].toInt());
            predictors.push_back(predictor);
        }
    }

    return {cv::ml::TrainData::create(predictors, 0, labels),cv::ml::TrainData::create(leftoutPredictors, 0, leftoutLabels)};
}

double testClassification(cv::Ptr<cv::ml::TrainData> data)
{
    data->setTrainTestSplitRatio(0.8);

    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::NU_SVC);
    svm->setNu(0.1);
    svm->setKernel(cv::ml::SVM::RBF);
    svm->setC(10);

    svm->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    double error = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {
        int prediction = svm->predict(data->getTestSamples().row(i));
        if (prediction != data->getTestResponses().row(i).at<int>(0)) 
        {
            ++error;
        }
    }

    return error / data->getTestSamples().rows;
}


double testNoveltyDetection(cv::Ptr<cv::ml::TrainData> data, cv::Ptr<cv::ml::TrainData> leftout)
{
    data->setTrainTestSplitRatio(0.8);

    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::ONE_CLASS);
    svm->setNu(0.1);
    svm->setGamma(0.3);
    svm->setKernel(cv::ml::SVM::RBF);
    svm->setC(10);

    svm->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    double falseNegative = 0, falsePositive = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {
        int prediction = svm->predict(data->getTestSamples().row(i));
        if (prediction != 1) 
        {
            ++falseNegative;
        }
    }

    for (int i = 0; i < leftout->getSamples().rows; ++i) 
    {
        int prediction = svm->predict(data->getSamples().row(i));
        if (prediction != 0) 
        {
            ++falsePositive;
        }
    }

    qDebug() << falseNegative << "false negative," << falsePositive << "false positive"; 

    return (falseNegative + falsePositive) / (data->getTestSamples().rows + leftout->getSamples().rows);
}


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    QCommandLineParser* parser = parseOptions(app);

    std::pair<cv::Ptr<cv::ml::TrainData>, cv::Ptr<cv::ml::TrainData>> data = loadData(parser->value(QLatin1String("data")));

    qDebug() << "Classification Error rate" << testClassification(data.first);
    qDebug() << "Novelty detection Error rate" << testNoveltyDetection(data.first, data.second);

    delete parser;

    return 0;
}

