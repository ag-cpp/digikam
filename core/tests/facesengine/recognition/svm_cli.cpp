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

cv::Mat vectortomat(const std::vector<float>& vector)
{
    cv::Mat mat(1, vector.size(), 5);

    memcpy(mat.data, vector.data(), vector.size()*sizeof(float));

    return mat;
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


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    QCommandLineParser* parser = parseOptions(app);

    loadData(parser->value(QLatin1String("data")));

    delete parser;

    return 0;
}

