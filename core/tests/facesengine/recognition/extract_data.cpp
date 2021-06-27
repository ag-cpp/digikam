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


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    return 0;
}
