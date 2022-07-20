/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 07/07/2022
 * Description : a command line tool to calculate quality level using NIMA model
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/dnn.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <filesystem>

// Local includes

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // Load and preprocess image
	cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        qDebug() << "Parse image to calculate quality score";
        qDebug() << "Usage: <image file>";
        return -1;
    }
    cv::Mat img_rgb;
    cv::cvtColor(img, img_rgb, cv::COLOR_BGR2RGB);
	cv::Mat cv_resized;
	cv::resize(img_rgb, cv_resized, cv::Size(224, 224), 0, 0, cv::INTER_NEAREST_EXACT);
	cv_resized.convertTo(cv_resized, CV_32FC3);
	cv_resized = cv_resized.mul(1.0 / float(127.5));
	subtract(cv_resized, cv::Scalar(1, 1, 1), cv_resized);

    // Serve model
	cv::dnn::Net net = cv::dnn::readNetFromTensorflow(argv[2]);
    if (net.empty()) {
        qDebug() << "Parse net to calculate quality score";
        qDebug() << "Usage: <image file>";
        return -1;
    }
	cv::Mat blob = cv::dnn::blobFromImage(cv_resized, 1, cv::Size(224, 224), cv::Scalar(0, 0, 0), false, false);
	net.setInput(blob);
	cv::Mat out = net.forward();
    
    // Post processs
    std::cout << "score : " << out << "\n";
    double min=0, max=0;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(out, &min, &max, &minLoc, &maxLoc);
    std::cout << "class : " << maxLoc << "\n";

}
