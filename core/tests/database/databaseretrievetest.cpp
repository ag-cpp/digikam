/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-14
 * Description : stand alone test application for database exportation
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

// Qt Includes

#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>
#include <QIcon>
#include <QDebug>
#include <QLibraryInfo>

// Local includes
#include "coredbaccess.h"
#include "dbenginebackend.h"

using namespace Digikam;

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addHelpOption();
    parser->setApplicationDescription(QLatin1String("Test application to export data from database to a text file\n"));
    parser->addOption(QCommandLineOption(QLatin1String("list"), QLatin1String("List all available database")));

    parser->process(app);

    return parser;
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    QCommandLineParser* parser = parseOptions(app);

    DbEngineParameters prm = DbEngineParameters::parametersFromConfig();
    qDebug() << prm;

    CoreDbAccess::setParameters(prm, CoreDbAccess::MainApplication);

    DbEngineLocking* lock = new DbEngineLocking();

    BdEngineBackend* db = new BdEngineBackend(QLatin1String("dbexport"), lock);

    if (!db->open(prm))
    {
        qDebug() << db->lastError();
        return -1;
    }

    QList<QVariant> ids;
    db->execSql(QLatin1String("SELECT id FROM Identities;"), &ids);

    foreach (const QVariant& v, ids)
    {
        qDebug() << v.toInt();
    }

    qDebug() << db->databaseType();

    qDebug() << db->lastSQLError();

    return 0;
}
