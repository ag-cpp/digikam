/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-06-16
 * Description : Testing tool for SVM classifier of face engines
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
#include <QStringList>
#include <QFile>
#include <QDebug>
#include "digikam_opencv.h"


// --------------------------------------------------------

std::shared_ptr<QCommandLineParser> parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("data"), QLatin1String("Data file"), QLatin1String("path relative to data file")));
    parser->addHelpOption();
    parser->process(app);

    return std::shared_ptr<QCommandLineParser>(parser);
}

/**
 * @brief loadData: load data from csv file into a train dataset and a leftout dataset to simulate unknown label
 */ 
std::pair<cv::Ptr<cv::ml::TrainData>, cv::Ptr<cv::ml::TrainData>> loadData(const QString& fileName) 
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

cv::Ptr<cv::ml::TrainData> filter(cv::Mat samples, cv::Mat labels, int selectedLabel) 
{
    cv::Mat predictors, y;
    for (int i = 0; i < samples.rows; ++i) 
    {
        if (labels.row(i).at<int>(0) == selectedLabel) 
        {
            predictors.push_back(samples.row(i));
            y.push_back(selectedLabel);
        }
    }

    return cv::ml::TrainData::create(predictors, 0, y);
}

std::vector<cv::Ptr<cv::ml::SVM>> oneclassClassifiers(cv::Mat samples, cv::Mat labels) 
{
    std::vector<cv::Ptr<cv::ml::SVM>> classifier(27);
    for (int i = 1; i <= 27; ++i)
    {
        cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
        svm->setType(cv::ml::SVM::ONE_CLASS);
        svm->setNu(0.1);
        svm->setGamma(0.3);
        svm->setKernel(cv::ml::SVM::RBF);
        svm->setC(10);

        svm->train(filter(samples, labels, i));
        classifier[i-1] = svm;
    }

    return classifier;
}

double testClassification(cv::Ptr<cv::ml::TrainData> data)
{
    data->setTrainTestSplitRatio(0.2);

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
    data->setTrainTestSplitRatio(0.2);

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
        int prediction = svm->predict(leftout->getSamples().row(i));
        if (prediction != 0) 
        {
            ++falsePositive;
        }
    }

    qDebug() << falseNegative << "false negative," << falsePositive << "false positive"; 

    return (falseNegative + falsePositive) / (data->getTestSamples().rows + leftout->getSamples().rows);
}

double testMultipleClassifiers(cv::Ptr<cv::ml::TrainData> data, cv::Ptr<cv::ml::TrainData> leftout)
{
    data->setTrainTestSplitRatio(0.2);
    std::vector<cv::Ptr<cv::ml::SVM>> noveltyClassifiers = oneclassClassifiers(data->getTrainSamples(), data->getTrainResponses());

    cv::Ptr<cv::ml::SVM> classifier = cv::ml::SVM::create();
    classifier->setType(cv::ml::SVM::NU_SVC);
    classifier->setNu(0.1);
    classifier->setKernel(cv::ml::SVM::RBF);
    classifier->setC(10);

    classifier->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    double falseNegative = 0, falsePositive = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {
        int prediction = classifier->predict(data->getTestSamples().row(i));
        if (prediction == data->getTestResponses().row(i).at<int>(0) && noveltyClassifiers[prediction-1]->predict(data->getTestSamples().row(i)) != 1) 
        {
            ++falseNegative;
        }
        else if (prediction != data->getTestResponses().row(i).at<int>(0))
        {
            ++falsePositive;
        }
    }

    for (int i = 0; i < leftout->getSamples().rows; ++i) 
    {
        int prediction = classifier->predict(leftout->getSamples().row(i));
        if (noveltyClassifiers[prediction-1]->predict(leftout->getSamples().row(i)) != 0) 
        {
            ++falsePositive;
        }
    }

    qDebug() << falseNegative << "false negative," << falsePositive << "false positive"; 

    return (falseNegative + falsePositive) / (data->getTestSamples().rows + leftout->getSamples().rows);
}

double test2Classifiers(cv::Ptr<cv::ml::TrainData> data, cv::Ptr<cv::ml::TrainData> leftout)
{
    data->setTrainTestSplitRatio(0.2);

    cv::Ptr<cv::ml::SVM> classifier = cv::ml::SVM::create();
    classifier->setType(cv::ml::SVM::NU_SVC);
    classifier->setNu(0.1);
    classifier->setKernel(cv::ml::SVM::RBF);
    classifier->setC(10);
    classifier->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    cv::Ptr<cv::ml::SVM> noveltyClassifier = cv::ml::SVM::create();
    noveltyClassifier->setType(cv::ml::SVM::ONE_CLASS);
    noveltyClassifier->setNu(0.1);
    noveltyClassifier->setGamma(0.3);
    noveltyClassifier->setKernel(cv::ml::SVM::RBF);
    noveltyClassifier->setC(10);
    noveltyClassifier->train(cv::ml::TrainData::create(data->getTrainSamples(), 0, data->getTrainResponses()));

    double falseNegative = 0, falsePositive = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {
        if (noveltyClassifier->predict(data->getTestSamples().row(i)) != 1)
        {
            ++falseNegative;
        }
        else 
        {
            int prediction = classifier->predict(data->getTestSamples().row(i));
            if (prediction != data->getTestResponses().row(i).at<int>(0))
            {
                ++falsePositive;
            }
        }
    }

    for (int i = 0; i < leftout->getSamples().rows; ++i) 
    {
        if (noveltyClassifier->predict(leftout->getSamples().row(i)) != 0)
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
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    std::pair<cv::Ptr<cv::ml::TrainData>, cv::Ptr<cv::ml::TrainData>> data = loadData(parser->value(QLatin1String("data")));

    qDebug() << "Classification Error rate" << testClassification(data.first);
    qDebug() << "Novelty detection Error rate" << testNoveltyDetection(data.first, data.second);
    qDebug() << "Multiple classifier Error rate" << testMultipleClassifiers(data.first, data.second);
    qDebug() << "2 classifiers Error rate" << test2Classifiers(data.first, data.second);

    return 0;
}

