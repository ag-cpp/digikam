/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-04
 * Description : Testing tool for KNN classifier of face engines
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
#include <QElapsedTimer>

#include "digikam_opencv.h"
#include "kd_tree.h"

// --------------------------------------------------------

std::shared_ptr<QCommandLineParser> parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("data"), QLatin1String("Data file"), QLatin1String("path relative to data file")));
    parser->addOption(QCommandLineOption(QLatin1String("threshold"), QLatin1String("Recognition threshold"), QLatin1String("Threshold of KNN")));
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

Digikam::KDTree* trainKNN(cv::Mat samples, cv::Mat labels) 
{
    Digikam::KDTree* tree = new Digikam::KDTree(samples.cols);
    for (int i = 0; i < samples.rows; ++i) 
    {
        tree->add(samples.row(i), labels.row(i).at<int>(0));
    }

    return tree;
}

int predict(std::shared_ptr<Digikam::KDTree> knn, cv::Mat predictors, double threshold) 
{
    QMap<double, QVector<int> > closestNeighbors = knn->getClosestNeighbors(predictors, threshold, 0.0, 5);
    QMap<int, QVector<double> > votingGroups;

    for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                     iter != closestNeighbors.cend();
                                                   ++iter)
    {
        for (QVector<int>::const_iterator node  = iter.value().cbegin();
                                          node != iter.value().cend();
                                        ++node)
        {
            int label = (*node);
            votingGroups[label].append(iter.key());
        }
    }

    double maxScore = 0.0;
    int prediction  = -1;

    for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                     group != votingGroups.cend();
                                                   ++group)
    {
        double score = 0.0;

        for (int i = 0 ; i < group.value().size() ; ++i)
        {
            score += (threshold - group.value()[i]);
        }

        if (score > maxScore)
        {
            maxScore   = score;
            prediction = group.key();
        }
    }

    return prediction;
}


double testClassification(cv::Ptr<cv::ml::TrainData> data, cv::Ptr<cv::ml::TrainData> leftout, double threshold)
{
    data->setTrainTestSplitRatio(0.2);
    std::shared_ptr<Digikam::KDTree> knn = std::shared_ptr<Digikam::KDTree>(trainKNN(data->getTrainSamples(), data->getTrainResponses()));

    QElapsedTimer timer;
    timer.start();
    
    double falseNegative = 0, falsePositive = 0;
    for (int i = 0; i < data->getTestSamples().rows; ++i) 
    {   
        int pred = predict(knn, data->getTestSamples().row(i), threshold);
        if (pred != data->getTestResponses().row(i).at<int>(0)) 
        {
            qDebug() << pred << "!=" << data->getTestResponses().row(i).at<int>(0);

            ++falsePositive;
        }
    }

    for (int i = 0; i < leftout->getSamples().rows; ++i) 
    {
        if (predict(knn, leftout->getSamples().row(i), threshold) != -1) 
        {
            ++falseNegative;
        }
    }

    qDebug() << falseNegative << "false negative," << falsePositive << "false positive"; 
    qDebug() << "Timer per face" << timer.elapsed() / (data->getTestSamples().rows + leftout->getSamples().rows);

    return (falseNegative + falsePositive) / (data->getTestSamples().rows + leftout->getSamples().rows);
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    std::pair<cv::Ptr<cv::ml::TrainData>, cv::Ptr<cv::ml::TrainData>> data = loadData(parser->value(QLatin1String("data")));

    float threshold = 10;
    if (parser->isSet(QLatin1String("threshold")))
    {
        threshold = parser->value(QLatin1String("threshold")).toDouble();
    }

    qDebug() << "Classification Error rate" << testClassification(data.first, data.second, threshold);
    return 0;
}

