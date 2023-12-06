// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//


#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <QCoreApplication>

#include <cstdlib>

#include <QStandardPaths>

#ifdef Q_OS_WIN
//for getting appdata path
//mingw-w64 Internet Explorer 5.01
#define _WIN32_IE 0x0501
#include <shlobj.h>
#endif

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

using namespace Marble;

namespace
{
    QString runTimeMarbleDataPath;

    QString runTimeMarblePluginPath;
}

MarbleDirs::MarbleDirs()
    : d( nullptr )
{
}

QString MarbleDirs::path( const QString& relativePath )
{ 
    QString  localpath = localPath() + QLatin1Char('/') + relativePath;	// local path
    QString  systempath  = systemPath() + QLatin1Char('/') + relativePath;	// system path

    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }
    return QDir( fullpath ).canonicalPath(); 
}

QString MarbleDirs::pluginPath( const QString& relativePath )
{ 
    const QString localpath = pluginLocalPath() + QDir::separator() + relativePath;    // local path
    const QString systempath  = pluginSystemPath() + QDir::separator() + relativePath; // system path


    QString fullpath = systempath;
    if ( QFile::exists( localpath ) ) {
        fullpath = localpath;
    }

    return QDir( fullpath ).canonicalPath(); 
}

QStringList MarbleDirs::entryList( const QString& relativePath, QDir::Filters filters )
{
    QStringList filesLocal = QDir(MarbleDirs::localPath() + QLatin1Char('/') + relativePath).entryList(filters);
    QStringList filesSystem = QDir(MarbleDirs::systemPath() + QLatin1Char('/') + relativePath).entryList(filters);
    QStringList allFiles( filesLocal );
    allFiles << filesSystem;

    // remove duplicate entries
    allFiles.sort();
    for ( int i = 1; i < allFiles.size(); ++i ) {
        if ( allFiles.at(i) == allFiles.at( i - 1 ) ) {
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
    if (!pluginSystemPath.isEmpty()) {
        allFiles << QDir(pluginSystemPath + QLatin1Char('/') + relativePath).entryList(filters);
    }

    // remove duplicate entries
    allFiles.sort();
    for ( int i = 1; i < allFiles.size(); ++i ) {
        if ( allFiles.at(i) == allFiles.at( i - 1 ) ) {
            allFiles.removeAt(i);
            --i;
        }
    }

    return allFiles;
}

QString MarbleDirs::systemPath()
{
    if (!runTimeMarbleDataPath.isEmpty()) {
        return runTimeMarbleDataPath;
    }

    QString systempath;

#ifdef Q_OS_WIN
	return QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String("data");
#endif

#ifdef Q_OS_MACX
    //
    // On OSX lets try to find any file first in the bundle
    // before branching out to home and sys dirs
    //
    CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
    const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
    CFRelease(myBundleRef);
    QString myPath(mypPathPtr);
    CFRelease(myMacPath);
    //do some magick so that we can still find data dir if
    //marble was not built as a bundle
    if (myPath.contains(QLatin1String(".app"))) {  //its a bundle!
      systempath = myPath + QLatin1String("/Contents/Resources/data");
    }

    if ( QFile::exists( systempath ) ){ 
      return systempath;
    }
#endif   // mac bundle

    // This is the preferred fallback location if no marbleDataPath is set.
    systempath = QDir( QCoreApplication::applicationDirPath() + QLatin1String( "/data" ) ).canonicalPath();

    if ( QFile::exists( systempath ) ){
      return systempath;
    }

    // This fallback location is for compatibility with KDE installations.
    return QDir( QCoreApplication::applicationDirPath()
                 + QLatin1String( "/../share/digikam/marble/data" )
                 ).canonicalPath();
}

QString MarbleDirs::pluginSystemPath()
{
    if (!runTimeMarblePluginPath.isEmpty()) {
        return runTimeMarblePluginPath;
    }

    QString systempath;

#ifdef Q_OS_MACX
    //
    // On OSX lets try to find any file first in the bundle
    // before branching out to home and sys dirs
    //
    CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
    const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
    CFRelease(myBundleRef);
    CFRelease(myMacPath);
    QString myPath(mypPathPtr);
    //do some magick so that we can still find data dir if
    //marble was not built as a bundle
    if (myPath.contains(QLatin1String(".app"))) {  //its a bundle!
      systempath = myPath + QLatin1String("/Contents/Resources/plugins");
    }

    if ( QFile::exists( systempath ) ){ 
      return systempath;
    }
#endif   // mac bundle

#ifdef Q_OS_WIN
	return QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String("plugins");
#endif

    // This is the preferred fallback location if no marblePluginPath is set.
    systempath = QDir( QCoreApplication::applicationDirPath() + QLatin1String( "/plugins" ) ).canonicalPath();

    if ( QFile::exists( systempath ) ){
      return systempath;
    }

    // This ultimate fallback location is for compatibility with KDE installations.
    return QDir( QCoreApplication::applicationDirPath()
                 + QLatin1String( "/../lib/kde4/plugins/marble" )
                 ).canonicalPath();
}

QString MarbleDirs::localPath() 
{
#ifndef Q_OS_WIN
    QString dataHome = QString::fromUtf8(getenv( "XDG_DATA_HOME") );
    if( dataHome.isEmpty() )
        dataHome = QDir::homePath() + QLatin1String("/.local/share");

    return dataHome + QLatin1String("/marble"); // local path
#else
	return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/.marble/data");
#endif
}

QStringList MarbleDirs::oldLocalPaths()
{
    QStringList possibleOldPaths;

#ifndef Q_OS_WIN
    const QString oldDefault = QDir::homePath() + QLatin1String("/.marble/data");
    possibleOldPaths.append( oldDefault );

    const QString xdgDefault = QDir::homePath() + QLatin1String("/.local/share/marble");
    possibleOldPaths.append( xdgDefault );

    QString xdg = QString::fromUtf8(getenv( "XDG_DATA_HOME" ));
    xdg += QLatin1String("/marble/");
    possibleOldPaths.append( xdg );
#endif

#ifdef Q_OS_WIN
	HWND hwnd = 0;
	WCHAR *appdata_path = new WCHAR[MAX_PATH + 1];

	SHGetSpecialFolderPathW(hwnd, appdata_path, CSIDL_APPDATA, 0);
	QString appdata = QString::fromUtf16(reinterpret_cast<ushort*>(appdata_path));
	delete[] appdata_path;
	possibleOldPaths << QString(QDir::fromNativeSeparators(appdata) + QLatin1String("/.marble/data")); // local path
#endif

    QString currentLocalPath = QDir( MarbleDirs::localPath() ).canonicalPath();
    QStringList oldPaths;
    for( const QString& possibleOldPath: possibleOldPaths ) {
        if( !QDir().exists( possibleOldPath ) ) {
            continue;
        }

        QString canonicalPossibleOldPath = QDir( possibleOldPath ).canonicalPath();
        if( canonicalPossibleOldPath == currentLocalPath ) {
            continue;
        }

        oldPaths.append( canonicalPossibleOldPath );
    }

    return oldPaths;
}

QString MarbleDirs::pluginLocalPath() 
{
#ifndef Q_OS_WIN
    return QDir::homePath() + QLatin1String("/.marble/plugins"); // local path
#else
	return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/.marble/plugins");
#endif
}

QString MarbleDirs::marbleDataPath()
{
    return runTimeMarbleDataPath;
}

QString MarbleDirs::marblePluginPath()
{
    return runTimeMarblePluginPath;
}

void MarbleDirs::setMarbleDataPath( const QString& adaptedPath )
{
    if ( !QDir::root().exists( adaptedPath ) )
    {
        qWarning() << QString::fromUtf8( "Invalid MarbleDataPath \"%1\". Using \"%2\" instead." ).arg( adaptedPath, systemPath() );
        return;
    }

    runTimeMarbleDataPath = adaptedPath;
}

void MarbleDirs::setMarblePluginPath( const QString& adaptedPath )
{
    if ( !QDir::root().exists( adaptedPath ) )
    {
        qWarning() << QString::fromUtf8( "Invalid MarblePluginPath \"%1\". Using \"%2\" instead." ).arg( adaptedPath, pluginSystemPath() );
        return;
    }

    runTimeMarblePluginPath = adaptedPath;
}

void MarbleDirs::debug()
{
    mDebug() << "=== MarbleDirs: ===";
    mDebug() << "Local Path:" << localPath();
    mDebug() << "Plugin Local Path:" << pluginLocalPath();
    mDebug() << "";
    mDebug() << "Marble Data Path (Run Time) :" << runTimeMarbleDataPath; 
    mDebug() << "";
    mDebug() << "Marble Plugin Path (Run Time) :" << runTimeMarblePluginPath; 
    mDebug() << "";
    mDebug() << "System Path:" << systemPath();
    mDebug() << "Plugin System Path:" << pluginSystemPath();
    mDebug() << "===================";
}
