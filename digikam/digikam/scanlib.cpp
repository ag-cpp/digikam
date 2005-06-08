////////////////////////////////////////////////////////////////////////////////
//
//    DIGIKAMAPP.CPP
//
//    Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//////////////////////////////////////////////////////////////////////////////


#include <kdebug.h>
#include <kfilemetainfo.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <klocale.h>

#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qwhatsthis.h>

extern "C" {
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
}

#include "albumdb.h"
#include "albummanager.h"
#include "scanlib.h"

/** @file scanlib.cpp*/

ScanLib::ScanLib()
{
    m_progressBar = new KProgressDialog(0);
    m_progressBar->setInitialSize(QSize(300,100), true);
    m_progressBar->hide();
    QWhatsThis::add( m_progressBar, i18n("This shows the progress of the "
        "scanning. During the scanning all files on disk are put in a "
        "database. This is needed for sorting on exif-date and speeds up "
        "overall performance of digiKam.") );
}

ScanLib::~ScanLib()
{
    delete m_progressBar;
}

void ScanLib::startScan()
{
    struct timeval tv1, tv2;

    gettimeofday(&tv1, 0);
    findFoldersWhichDoNotExist();
    gettimeofday(&tv2, 0);
    timing("Finding non-existing folders",tv1, tv2);

    gettimeofday(&tv1, 0);
    findMissingItems();
    gettimeofday(&tv2, 0);
    timing("Finding items not in the database or disk",tv1, tv2);

    gettimeofday(&tv1, 0);
    updateItemsWithoutDate();
    gettimeofday(&tv2, 0);
    timing("Updating items without date",tv1, tv2);

    deleteStaleEntries();

    AlbumDB* db = AlbumManager::instance()->albumDB();
    db->setSetting("Scanned",
                   QDateTime::currentDateTime().toString(Qt::ISODate));

}

void ScanLib::findFoldersWhichDoNotExist()
{
    QMap<QString, int> toBeDeleted;
    QString basePath(AlbumManager::instance()->getLibraryPath());
    AlbumDB* db = AlbumManager::instance()->albumDB();
    AlbumInfo::List aList = db->scanAlbums();
    for (AlbumInfo::List::iterator it = aList.begin(); it != aList.end(); ++it)
    {
        AlbumInfo info = *it;
        info.url = QDir::cleanDirPath(info.url);
        QFileInfo fi(basePath + info.url);
        if (!fi.exists() || !fi.isDir())
            toBeDeleted[info.url] = info.id;
    }

    kapp->processEvents();

    if (!toBeDeleted.isEmpty())
    {
        int rc = KMessageBox::warningYesNoList(   0,
            i18n("There seem to be a folder in the database which does not "
                 "appear to be on disk. This folder should be deleted from "
                 "the database, but that means you might loose information, "
                 "because all images related to this folder will be deleted "
                 "from the database as well. digiKam can not continue "
                 "without deleting the items from the database because all "
                 "views depends on the information in the database. Do you "
                 "want them to be removed?",
                 "There seem to be %n folders in the database which do not "
                 "appear to be on disk. These folders should be deleted from "
                 "the database, but that means you might loose information, "
                 "because all images related to this folder will be deleted "
                 "from the database as well. digiKam can not continue "
                 "without deleting the items from the database because all "
                 "views depends on the information in the database. Do you "
                 "want them to be removed?",
                 toBeDeleted.count()),
            toBeDeleted.keys(),
            i18n("Folders are Missing"));

        if (rc != KMessageBox::Yes)
            exit(0);

        QMapIterator<QString,int> it;
        for (it = toBeDeleted.begin() ; it != toBeDeleted.end(); ++it)
        {
            kdDebug() << "Removing folder: " << it.key() << endl;
            db->deleteAlbum( it.data() );
        }
    }
}

void ScanLib::findMissingItems()
{
    QString albumPath = QDir::cleanDirPath(
            AlbumManager::instance()->getLibraryPath());
    m_progressBar->setAllowCancel( false );
    m_progressBar->showCancelButton (false );
    m_progressBar->progressBar()->setProgress( 0 );
    m_progressBar->setLabel(i18n("Scanning items..."));
    m_progressBar->progressBar()->
            setTotalSteps( countItemsInFolder( albumPath ) );
    m_progressBar->show();
    kapp->processEvents();

    allFiles( albumPath );

    m_progressBar->hide();
    kapp->processEvents();
}

void ScanLib::updateItemsWithoutDate()
{
    AlbumDB* db = AlbumManager::instance()->albumDB();
    QStringList urls = db->getAllItemURLsWithoutDate();

    if (urls.isEmpty())
    {
        m_progressBar->progressBar()->setTotalSteps(1);
        m_progressBar->progressBar()->setProgress(1);
        m_progressBar->hide();
        return;
    }

    m_progressBar->setAllowCancel( false );
    m_progressBar->showCancelButton (false );
    m_progressBar->progressBar()->setProgress(0);
    m_progressBar->progressBar()->setTotalSteps(urls.count());
    m_progressBar->setLabel(i18n("Updating items..."));
    m_progressBar->show();
    kapp->processEvents();

    QString base = QDir::cleanDirPath(
            AlbumManager::instance()->getLibraryPath());

    QMap<QString, int> filesToBeDeleted;
    int counter=0;
    for (QStringList::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        m_progressBar->progressBar()->advance(1);
        ++counter;
        if ( counter % 30 == 0 )
            kapp->processEvents();
        QFileInfo fi(*it);
        QString albumURL = fi.dirPath();
        albumURL = QDir::cleanDirPath(albumURL.remove(base));
        int albumID = db->getOrCreateAlbumId(albumURL);
        if (fi.exists())
            updateItemDate(albumURL, fi.fileName(), albumID);
        else
            if (m_filesToBeDeleted.findIndex(qMakePair(fi.fileName(),albumID))
                == -1)
                m_filesToBeDeleted.append(qMakePair(fi.fileName(),albumID));
    }

    m_progressBar->hide();
    kapp->processEvents();
}

int ScanLib::countItemsInFolder(const QString& directory)
{
    int items = 0;

    QDir dir( directory );
    if ( !dir.exists() or !dir.isReadable() )
        return 0;

    const QFileInfoList *list = dir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    items += list->count();

    while ( (fi = it.current()) != 0 )
    {
        if ( fi->isDir() && fi->fileName() != "." && fi->fileName() != "..")
            items += countItemsInFolder( fi->filePath() );
        ++it;
    }
    return items;
}

void ScanLib::allFiles(const QString& directory)
{
    QDir dir( directory );
    if ( !dir.exists() or !dir.isReadable() )
    {
        kdWarning() << "Folder does not exist or is not readable: "
                    << directory << endl;
        return;
    }

    QString base = QDir::cleanDirPath(
            AlbumManager::instance()->getLibraryPath());
    QString albumURL = directory;
    albumURL = QDir::cleanDirPath(albumURL.remove(base));

    AlbumDB* db = AlbumManager::instance()->albumDB();
    int albumID = db->getOrCreateAlbumId(albumURL);
    QStringList filesInAlbum = db->getItemNamesInAlbum( albumID );

    QMap<QString, bool> filesFoundInDB;
    for (QStringList::iterator it = filesInAlbum.begin();
         it != filesInAlbum.end(); ++it)
    {
        if (albumURL.isEmpty())
        {
            kdDebug() << "Root item found: " << *it
                    << " in " << albumID << endl;
            if (m_filesToBeDeleted.findIndex(qMakePair(*it,albumID)) == -1)
                m_filesToBeDeleted.append(qMakePair(*it,albumID));
        }
        else
            filesFoundInDB.insert(*it, true);
    }

    const QFileInfoList *list = dir.entryInfoList();
    if (!list)
        return;

    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    m_progressBar->progressBar()->advance(list->count());
    kapp->processEvents();

    while ( (fi = it.current()) != 0 )
    {
        if ( fi->isFile())
            if (filesFoundInDB.contains(fi->fileName()) )
                filesFoundInDB.erase(fi->fileName());
            else
                storeItemInDatabase(albumURL, fi->fileName(), albumID);
        else if ( fi->isDir() && fi->fileName() != "." && fi->fileName() != "..")
            allFiles( fi->filePath() );
        ++it;
    }

    // Removing items from the db which we did not see on disk.
    if (!filesFoundInDB.isEmpty())
    {
        QMapIterator<QString,bool> it2;
        for (it2 = filesFoundInDB.begin() ; it2 != filesFoundInDB.end(); ++it2)
            if (m_filesToBeDeleted.findIndex(qMakePair(it2.key(),albumID))
                == -1)
                m_filesToBeDeleted.append(qMakePair(it2.key(),albumID));
    }
}

void ScanLib::storeItemInDatabase(const QString& albumURL,
                                  const QString& filename,
                                  int albumID)
{
    QString comment;
    QDateTime datetime;
    QDir albumPath( AlbumManager::instance()->getLibraryPath());

    // Do not store items found in the root of the albumdb
    if (albumURL.isEmpty())
        return;
    KFileMetaInfo itemMetaInfo( albumPath.path()+albumURL+'/'+filename );

    if (itemMetaInfo.isValid() &&
        itemMetaInfo.containsGroup("Jpeg EXIF Data"))
    {
        comment = itemMetaInfo.group("Jpeg EXIF Data").
                  item("Comment").value().toString();
        datetime = itemMetaInfo.group("Jpeg EXIF Data").
                   item("Date/time").value().toDateTime();
    }

    if ( !datetime.isValid() )
    {
        QFileInfo info( albumPath.path()+albumURL+'/'+filename );
        datetime = info.lastModified();
    }

    AlbumDB* dbstore = AlbumManager::instance()->albumDB();
    dbstore->addItem(albumID, filename, datetime,comment);
}

void ScanLib::updateItemDate(const QString& albumURL,
                             const QString& filename,
                             int albumID)
{
    QDateTime datetime;
    QDir albumPath( AlbumManager::instance()->getLibraryPath());

    KFileMetaInfo itemMetaInfo( albumPath.path()+albumURL+'/'+filename );
    if (itemMetaInfo.isValid() &&
        itemMetaInfo.containsGroup("Jpeg EXIF Data"))
    {
        datetime = itemMetaInfo.group("Jpeg EXIF Data").
                   item("Date/time").value().toDateTime();
    }

    if ( !datetime.isValid() )
    {
        QFileInfo info( albumPath.path()+albumURL+'/'+filename );
        datetime = info.lastModified();
    }

    AlbumDB* dbstore = AlbumManager::instance()->albumDB();
    dbstore->setItemDate(albumID, filename, datetime);
}

void ScanLib::deleteStaleEntries()
{
    QStringList listToBeDeleted;
    QValueList< QPair<QString,int> >::iterator it;
    for (it = m_filesToBeDeleted.begin() ; it != m_filesToBeDeleted.end();
         ++it)
        listToBeDeleted.append((*it).first);

    if ( !m_filesToBeDeleted.isEmpty() )
    {
        int rc = KMessageBox::warningYesNoList(   0,
          i18n("There seem to be an item in the database which does not "
               "appear to be on disk or is located in the root folder of "
               "the path. This file should be deleted from the "
               "database, but that means you might loose information. "
               "digiKam can not continue without deleting the item from "
               "the database because all views depends on the information "
               "in the database. Do you want it to be removed?",
               "There seem to be %n items in the database which do not "
               "appear to be on disk or are located in the root folder of "
               "the path. These files should be deleted from the "
               "database, but that means you might loose information. "
               "digiKam can not continue without deleting these item from "
               "the database because all views depends on the information "
               "in the database. Do you want them to be removed?",
               listToBeDeleted.count()),
          listToBeDeleted,
          i18n("Files are Missing"));

        if (rc != KMessageBox::Yes)
            exit(0);

        AlbumDB* db = AlbumManager::instance()->albumDB();
        for (it = m_filesToBeDeleted.begin() ; it != m_filesToBeDeleted.end();
             ++it)
        {
            kdDebug() << "Removing: " << (*it).first << " in "
                      << (*it).second << endl;
            db->deleteItem( (*it).second, (*it).first );
        }
    }
}

void ScanLib::timing(const QString& text, struct timeval tv1, struct timeval tv2)
{
    kdDebug() << "ScanLib: "
            << text + ": "
            << (((tv2.tv_sec-tv1.tv_sec)*1000000 +
                 (tv2.tv_usec-tv1.tv_usec))/1000)
            << " ms" << endl;
}
