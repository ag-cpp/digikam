// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#include "MarbleDirs.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStandardPaths>

#include "digikam_debug.h"

using namespace Marble;

MarbleDirs::MarbleDirs()
{
}

QString MarbleDirs::path( const QString& relativePath )
{
    QString localpath  = localPath()  + QDir::separator() + relativePath;   // local path
    QString systempath = systemPath() + QDir::separator() + relativePath;   // system path
    QString fullpath   = systempath;

    if ( QFile::exists( localpath ) )
    {
        fullpath = localpath;
    }

    return QDir( fullpath ).canonicalPath();
}

QString MarbleDirs::pluginPath( const QString& relativePath )
{
    const QString localpath  = pluginLocalPath()  + QDir::separator() + relativePath; // local path
    const QString systempath = pluginSystemPath() + QDir::separator() + relativePath; // system path
    QString fullpath         = systempath;

    if ( QFile::exists( localpath ) )
    {
        fullpath = localpath;
    }

    return QDir( fullpath ).canonicalPath();
}

QStringList MarbleDirs::entryList( const QString& relativePath, QDir::Filters filters )
{
    QStringList filesLocal  = QDir(MarbleDirs::localPath()  + QLatin1Char('/') + relativePath).entryList(filters);
    QStringList filesSystem = QDir(MarbleDirs::systemPath() + QLatin1Char('/') + relativePath).entryList(filters);
    QStringList allFiles( filesLocal );
    allFiles << filesSystem;

    // remove duplicate entries
    allFiles.sort();

    for ( int i = 1 ; i < allFiles.size() ; ++i )
    {
        if ( allFiles.at(i) == allFiles.at( i - 1 ) )
        {
            allFiles.removeAt(i);
            --i;
        }
    }

    return allFiles;
}

QStringList MarbleDirs::pluginEntryList( const QString& relativePath, QDir::Filters filters )
{
    QStringList allFiles = QDir(MarbleDirs::pluginLocalPath() + QLatin1Char('/') + relativePath).entryList(filters);
    auto const pluginSystemPath = MarbleDirs::pluginSystemPath();

    if (!pluginSystemPath.isEmpty())
    {
        allFiles << QDir(pluginSystemPath + QLatin1Char('/') + relativePath).entryList(filters);
    }

    // remove duplicate entries

    allFiles.sort();

    for ( int i = 1 ; i < allFiles.size() ; ++i )
    {
        if ( allFiles.at(i) == allFiles.at( i - 1 ) )
        {
            allFiles.removeAt(i);
            --i;
        }
    }

    return allFiles;
}

QString MarbleDirs::systemPath()
{
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          QLatin1String("digikam/marble"),
                                          QStandardPaths::LocateDirectory);

    return path;
}

QString MarbleDirs::localPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/digikam/marble_data");
}

QString MarbleDirs::pluginLocalPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/digikam/marble_plugins");
}

QString MarbleDirs::pluginSystemPath()
{
    return QLibraryInfo::path(QLibraryInfo::PluginsPath) + QLatin1String("/digikam/marble");
}

void MarbleDirs::debug()
{
    qCDebug(DIGIKAM_MARBLE_LOG) << "=== MarbleDirs: ===";
    qCDebug(DIGIKAM_MARBLE_LOG) << "Local Path        :" << localPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "Plugin Local Path :" << pluginLocalPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "System Path       :" << systemPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "Plugin System Path:" << pluginSystemPath();
    qCDebug(DIGIKAM_MARBLE_LOG) << "===================";
}
