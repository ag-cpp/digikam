/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2020-08-23
 * Description : Convert image format to PNG
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
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QDebug>

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("source"), QLatin1String("source folder"), QLatin1String("path relative to original data folder")));
    parser->addOption(QCommandLineOption(QLatin1String("destination"), QLatin1String("destination location"), QLatin1String("path relative to result data folder")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCommandLineParser* parser = parseOptions(app);

    if (! parser->isSet(QLatin1String("source")) &&  !parser->isSet(QLatin1String("destination")))
    {
        qWarning("Folders are not set !!!");

        return 1;
    }

    QDir source(parser->value(QLatin1String("source")));
    QDir destination(parser->value(QLatin1String("destination")));

    if (! source.exists())
    {
        return 1;
    }

    QFileInfoList subDirs = source.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < subDirs.size(); ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());

        QString path = destination.absolutePath() + QLatin1String("/") + subDir.dirName();

        destination.mkpath(path);

        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);

        for (int j = 0; j < filesInfo.size(); ++j)
        {
            QImage img(filesInfo[j].absoluteFilePath());

            img.save(path + QLatin1String("/") + subDir.dirName() + QLatin1String("_") + QString::number(j) + QLatin1String(".png"), "PNG");
        }
    }

    return 0;
}
