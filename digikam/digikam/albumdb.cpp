/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-06-18
 * Description :
 *
 * Copyright 2004 by Renchi Raju

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

/** @file albumdb.cpp */

#include <kdebug.h>
#include <klocale.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <cstdio>
#include <cstdlib>

extern "C" {
#include <sqlite3.h>
#include <sys/time.h>
#include <time.h>
}

#include "albummanager.h"
#include "albumdb.h"

typedef struct sqlite3_stmt sqlite3_stmt;

AlbumDB::AlbumDB()
{
    m_valid = false;
    m_db    = 0;
}

AlbumDB::~AlbumDB()
{
    if (m_db)
    {
        sqlite3_close(m_db);
    }
}

void AlbumDB::setDBPath(const QString& path)
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = 0;
    }

    m_valid = false;

    sqlite3_open(QFile::encodeName(path), &m_db);
    if (m_db == 0)
    {
        kdWarning() << "Cannot open database: "
                    << sqlite3_errmsg(m_db)
                    << endl;
    }
    else
    {
        initDB();
    }
}

void AlbumDB::initDB()
{
    m_valid = false;

    // Check if we have the required tables

    QStringList values;

    if (!execSql( QString("SELECT name FROM sqlite_master"
                          " WHERE type='table'"
                          " ORDER BY name;"),
                  &values ))
    {
        return;
    }

    if (!values.contains("Albums"))
    {
        if (!execSql( QString("CREATE TABLE Albums\n"
                              " (id INTEGER PRIMARY KEY,\n"
                              "  url TEXT NOT NULL UNIQUE,\n"
                              "  date DATE NOT NULL,\n"
                              "  caption TEXT,\n"
                              "  collection TEXT,\n"
                              "  icon INTEGER);") ))
        {
            return;
        }

        if (!execSql( QString("CREATE TABLE Tags\n"
                              " (id INTEGER PRIMARY KEY,\n"
                              "  pid INTEGER,\n"
                              "  name TEXT NOT NULL,\n"
                              "  icon INTEGER,\n"
                              "  iconkde TEXT,\n"
                              "  UNIQUE (name, pid));") ))
        {
            return;
        }

        if (!execSql( QString("CREATE TABLE TagsTree\n"
                              " (id INTEGER NOT NULL,\n"
                              "  pid INTEGER NOT NULL,\n"
                              "  UNIQUE (id, pid));") ))
        {
            return;
        }

        if (!execSql( QString("CREATE TABLE Images\n"
                              " (id INTEGER PRIMARY KEY,\n"
                              "  name TEXT NOT NULL,\n"
                              "  dirid INTEGER NOT NULL,\n"
                              "  caption TEXT,\n"
                              "  datetime DATETIME,\n"
                              "  UNIQUE (name, dirid));") ))
        {
            return;
        }


        if (!execSql( QString("CREATE TABLE ImageTags\n"
                              " (imageid INTEGER NOT NULL,\n"
                              "  tagid INTEGER NOT NULL,\n"
                              "  UNIQUE (imageid, tagid));") ))
        {
            return;
        }

        if (!execSql( QString("CREATE TABLE ImageProperties\n"
                              " (imageid  INTEGER NOT NULL,\n"
                              "  property TEXT    NOT NULL,\n"
                              "  value    TEXT    NOT NULL,\n"
                              "  UNIQUE (imageid, property));") ))
        {
            return;
        }

        if ( !execSql( QString( "CREATE TABLE Searches  \n"
                                " (id INTEGER PRIMARY KEY, \n"
                                "  name TEXT NOT NULL UNIQUE, \n"
                                "  url  TEXT NOT NULL);" ) ) )
        {
            return;
        }

        if (!execSql( QString("CREATE TABLE Settings         \n"
                              "(keyword TEXT NOT NULL UNIQUE,\n"
                              " value TEXT);") ))
            return;
        else
            setSetting("DBVersion","1");

        // TODO: see which more indices are needed
        // create indices
        execSql("CREATE INDEX dir_index ON Images    (dirid);");
        execSql("CREATE INDEX tag_index ON ImageTags (tagid);");

        // create triggers

        // trigger: delete from Images/ImageTags/ImageProperties
        // if Album has been deleted
        execSql("CREATE TRIGGER delete_album DELETE ON Albums\n"
                "BEGIN\n"
                " DELETE FROM ImageTags\n"
                "   WHERE imageid IN (SELECT id FROM Images WHERE dirid=OLD.id);\n"
                " DELETE From ImageProperties\n"
                "   WHERE imageid IN (SELECT id FROM Images WHERE dirid=OLD.id);\n"
                " DELETE FROM Images\n"
                "   WHERE dirid = OLD.id;\n"
                "END;");

        // trigger: delete from ImageTags/ImageProperties
        // if Image has been deleted
        execSql("CREATE TRIGGER delete_image DELETE ON Images\n"
                "BEGIN\n"
                "  DELETE FROM ImageTags\n"
                "    WHERE imageid=OLD.id;\n"
                "  DELETE From ImageProperties\n "
                "    WHERE imageid=OLD.id;\n"
                "  UPDATE Albums SET icon=null \n "
                "    WHERE icon=OLD.id;\n"
                "  UPDATE Tags SET icon=null \n "
                "    WHERE icon=OLD.id;\n"
                "END;");

        // trigger: delete from ImageTags if Tag has been deleted
        execSql("CREATE TRIGGER delete_tag DELETE ON Tags\n"
                "BEGIN\n"
                "  DELETE FROM ImageTags WHERE tagid=OLD.id;\n"
                "END;");

        // trigger: insert into TagsTree if Tag has been added
        execSql("CREATE TRIGGER insert_tagstree AFTER INSERT ON Tags\n"
                "BEGIN\n"
                "  INSERT INTO TagsTree\n"
                "    SELECT NEW.id, NEW.pid\n"
                "    UNION\n"
                "    SELECT NEW.id, pid FROM TagsTree WHERE id=NEW.pid;\n"
                "END;");

        // trigger: delete from TagsTree if Tag has been deleted
        execSql("CREATE TRIGGER delete_tagstree DELETE ON Tags\n"
                "BEGIN\n"
                " DELETE FROM Tags\n"
                "   WHERE id  IN (SELECT id FROM TagsTree WHERE pid=OLD.id);\n"
                " DELETE FROM TagsTree\n"
                "   WHERE id IN (SELECT id FROM TagsTree WHERE pid=OLD.id);\n"
                " DELETE FROM TagsTree\n"
                "    WHERE id=OLD.id;\n"
                "END;");

        // trigger: delete from TagsTree if Tag has been deleted
        execSql("CREATE TRIGGER move_tagstree UPDATE OF pid ON Tags\n"
                "BEGIN\n"
                "  DELETE FROM TagsTree\n"
                "    WHERE\n"
                "      ((id = OLD.id)\n"
                "        OR\n"
                "        id IN (SELECT id FROM TagsTree WHERE pid=OLD.id))\n"
                "      AND\n"
                "      pid IN (SELECT pid FROM TagsTree WHERE id=OLD.id);\n"
                "  INSERT INTO TagsTree\n"
                "     SELECT NEW.id, NEW.pid\n"
                "     UNION\n"
                "     SELECT NEW.id, pid FROM TagsTree WHERE id=NEW.pid\n"
                "     UNION\n"
                "     SELECT id, NEW.pid FROM TagsTree WHERE pid=NEW.id\n"
                "     UNION\n"
                "     SELECT A.id, B.pid FROM TagsTree A, TagsTree B\n"
                "        WHERE\n"
                "        A.pid = NEW.id AND B.id = NEW.pid;\n"
                "END;");
    }

    m_valid = true;
}

AlbumInfo::List AlbumDB::scanAlbums()
{
    AlbumInfo::List aList;

    QString basePath(AlbumManager::instance()->getLibraryPath());

    QStringList values;
    execSql( "SELECT A.id, A.url, A.date, A.caption, A.collection, I.name \n "
             "FROM Albums AS A LEFT OUTER JOIN Images AS I \n "
             "ON A.icon=I.id;", &values);

    for (QStringList::iterator it = values.begin(); it != values.end();)
    {
        AlbumInfo info;

        info.id = (*it).toInt();
        ++it;
        info.url = *it;
        ++it;
        info.date = QDate::fromString(*it, Qt::ISODate);
        ++it;
        info.caption = *it;
        ++it;
        info.collection = *it;
        ++it;
        if (!(*it).isEmpty())
        {
            info.icon = basePath + info.url + "/" + *it;
        }
        ++it;

        aList.append(info);
    }

    return aList;
}

TagInfo::List AlbumDB::scanTags()
{
    TagInfo::List tList;

    QString basePath(AlbumManager::instance()->getLibraryPath());

    QStringList values;
    execSql( "SELECT T.id, T.pid, T.name, A.url, I.name, T.iconkde \n "
             "FROM Tags AS T LEFT OUTER JOIN Images AS I ON I.id=T.icon \n "
             "  LEFT OUTER JOIN Albums AS A ON A.id=I.dirid; ", &values );

    QString iconName, iconKDE, albumURL;

    for (QStringList::iterator it = values.begin(); it != values.end();)
    {
        TagInfo info;

        info.id     = (*it).toInt();
        ++it;
        info.pid    = (*it).toInt();
        ++it;
        info.name   = *it;
        ++it;
        albumURL    = *it;
        ++it;
        iconName    = *it;
        ++it;
        iconKDE     = *it;
        ++it;

        if ( albumURL.isEmpty() )
        {
            info.icon = iconKDE;
        }
        else
        {
            info.icon = basePath + albumURL + "/" + iconName;
        }

        tList.append(info);
    }

    return tList;
}

SearchInfo::List AlbumDB::scanSearches()
{
    SearchInfo::List searchList;    

    QStringList values;
    execSql( "SELECT id, name, url FROM Searches;", &values);

    for (QStringList::iterator it = values.begin(); it != values.end();)
    {
        SearchInfo info;

        info.id   = (*it).toInt();
        ++it;
        info.name = (*it);
        ++it;
        info.url  = (*it);
        ++it;

        searchList.append(info);        
    }

    return searchList;
}

void AlbumDB::beginTransaction()
{
    execSql( "BEGIN TRANSACTION;" );
}

void AlbumDB::commitTransaction()
{
    execSql( "COMMIT TRANSACTION;" );
}

int AlbumDB::addAlbum(const QString& url, const QString& caption,
                      const QDate& date, const QString& collection)
{
    if (!m_db)
        return -1;

    execSql( QString("REPLACE INTO Albums (url, date, caption, collection) "
                     "VALUES('%1', '%2', '%3', '%4');")
             .arg(escapeString(url))
             .arg(date.toString(Qt::ISODate))
             .arg(escapeString(caption))
             .arg(escapeString(collection)));

    int id = sqlite3_last_insert_rowid(m_db);
    return id;
}

void AlbumDB::setAlbumCaption(int albumID, const QString& caption)
{
    execSql( QString("UPDATE Albums SET caption='%1' WHERE id=%2;")
             .arg(escapeString(caption))
             .arg(albumID) );
}

void AlbumDB::setAlbumCollection(int albumID, const QString& collection)
{
    execSql( QString("UPDATE Albums SET collection='%1' WHERE id=%2;")
             .arg(escapeString(collection))
             .arg(albumID) );
}

void AlbumDB::setAlbumDate(int albumID, const QDate& date)
{
    execSql( QString("UPDATE Albums SET date='%1' WHERE id=%2;")
             .arg(date.toString(Qt::ISODate))
             .arg(albumID) );
}

void AlbumDB::setAlbumIcon(int albumID, Q_LLONG iconID)
{
    execSql( QString("UPDATE Albums SET icon=%1 WHERE id=%2;")
             .arg(iconID)
             .arg(albumID) );
}

    
QString AlbumDB::getAlbumIcon(int albumID)
{
    QStringList values;
    execSql( QString("SELECT Albums.url, Images.name FROM Albums "
                     "LEFT OUTER JOIN Images ON Albums.icon=Images.id "
                     "WHERE Albums.id=%1;")
             .arg(albumID), &values );
    if (values.isEmpty())
        return QString();

    QStringList::iterator it = values.begin();
    QString url  = *it;
    ++it;
    QString icon = *it;
    if (icon.isEmpty())
        return QString();

    QString basePath(AlbumManager::instance()->getLibraryPath());
    basePath += url;
    basePath += "/" + icon;

    return basePath;
}

void AlbumDB::deleteAlbum(int albumID)
{
    execSql( QString("DELETE FROM Albums WHERE id='%1'")
             .arg(albumID) );
}

int AlbumDB::addTag(int parentTagID, const QString& name, const QString& iconKDE,
                    Q_LLONG iconID)
{
    if (!m_db)
        return -1;

    if (!execSql( QString("INSERT INTO Tags (pid, name) "
                          "VALUES( %1, '%2')")
                  .arg(parentTagID)
                  .arg(escapeString(name))))
    {
        return -1;
    }

    int id = sqlite3_last_insert_rowid(m_db);

    if (!iconKDE.isEmpty())
    {
        execSql( QString("UPDATE Tags SET iconkde='%1' WHERE id=%2;")
                 .arg(escapeString(iconKDE))
                 .arg(id));
    }
    else
    {
        execSql( QString("UPDATE Tags SET icon=%1 WHERE id=%2;")
                 .arg(iconID)
                 .arg(id));
    }
    
    return id;
}

void AlbumDB::deleteTag(int tagID)
{
    execSql( QString("DELETE FROM Tags WHERE id=%1")
                 .arg(tagID) );
}

void AlbumDB::setTagIcon(int tagID, const QString& iconKDE, Q_LLONG iconID)
{
    if (!iconKDE.isEmpty())
    {
        execSql( QString("UPDATE Tags SET iconkde='%1' WHERE id=%2;")
                 .arg(escapeString(iconKDE))
                 .arg(tagID));
    }
    else
    {
        execSql( QString("UPDATE Tags SET icon=%1 WHERE id=%2;")
                 .arg(iconID)
                 .arg(tagID));
    }
}

QString AlbumDB::getTagIcon(int tagID)
{
    QStringList values;
    execSql( QString("SELECT A.url, I.name, T.iconkde \n "
                     "FROM Tags AS T LEFT OUTER JOIN Images AS I ON I.id=T.icon \n "
                     "  LEFT OUTER JOIN Albums AS A ON A.id=I.dirid \n "
                     "WHERE T.id=%1;")
             .arg(tagID), &values );

    if (values.isEmpty())
        return QString();
    
    QString basePath(AlbumManager::instance()->getLibraryPath());

    QString iconName, iconKDE, albumURL, icon;

    QStringList::iterator it = values.begin();

    albumURL    = *it;
    ++it;
    iconName    = *it;
    ++it;
    iconKDE     = *it;
    ++it;

    if ( albumURL.isEmpty() )
    {
        icon = iconKDE;
    }
    else
    {
        icon = basePath + albumURL + "/" + iconName;
    }

    return icon;
}

void AlbumDB::setTagParentID(int tagID, int newParentTagID)
{
    execSql( QString("UPDATE Tags SET pid=%1 WHERE id=%2;")
             .arg(newParentTagID)
             .arg(tagID) );
}

int AlbumDB::addSearch(const QString& name, const KURL& url)
{
    if (!m_db)
	return -1;

    QString str("INSERT INTO Searches (name, url) \n"
                "VALUES('$$@@$$', '$$##$$');");
    str.replace("$$@@$$", escapeString(name));
    str.replace("$$##$$", escapeString(url.url()));
    
    if (!execSql(str))
    {
	return -1;
    }

    return sqlite3_last_insert_rowid(m_db);
}

void AlbumDB::updateSearch(int searchID, const QString& name,
			   const KURL& url)
{
    QString str = QString("UPDATE Searches SET name='$$@@$$', url='$$##$$' \n"
                          "WHERE id=%1")
                  .arg(searchID);
    str.replace("$$@@$$", escapeString(name));
    str.replace("$$##$$", escapeString(url.url()));

    execSql(str);
}

void AlbumDB::deleteSearch(int searchID)
{
    execSql( QString("DELETE FROM Searches WHERE id=%1")
             .arg(searchID) );
}

void AlbumDB::setSetting(const QString& keyword,
                         const QString& value )
{
    execSql( QString("REPLACE into Settings VALUES ('%1','%2');")
            .arg( escapeString(keyword) )
            .arg( escapeString(value) ));
}

QString AlbumDB::getSetting(const QString& keyword)
{
    QStringList values;
    execSql( QString("SELECT value FROM Settings "
                     "WHERE keyword='%1';")
            .arg(escapeString(keyword)),
            &values );

    if (values.isEmpty())
        return QString::null;
    else
        return values[0];
}

bool AlbumDB::execSql(const QString& sql, QStringList* const values,
                      const bool debug)
{
    if ( debug )
        kdDebug() << "SQL-query: " << sql << endl;

    if ( !m_db )
    {
        kdWarning() << k_funcinfo << "SQLite pointer == NULL"
                    << endl;
        return false;
    }

    const char*   tail;
    sqlite3_stmt* stmt;
    int           error;

    //compile SQL program to virtual machine
    error = sqlite3_prepare(m_db, sql.utf8(), -1, &stmt, &tail);
    if ( error != SQLITE_OK )
    {
        kdWarning() << k_funcinfo
                    << "sqlite_compile error: "
                    << sqlite3_errmsg(m_db)
                    << " on query: "
                    << sql << endl;
        return false;
    }

    int cols = sqlite3_column_count(stmt);

    while ( true )
    {
        error = sqlite3_step( stmt );

        if ( error == SQLITE_DONE || error == SQLITE_ERROR )
            break;

        //iterate over columns
        for ( int i = 0; values && i < cols; i++ )
        {
            *values << QString::fromUtf8( (const char*)sqlite3_column_text( stmt, i ) );
        }
    }

    sqlite3_finalize( stmt );

    if ( error != SQLITE_DONE )
    {
        kdWarning() << "sqlite_step error: "
                    << sqlite3_errmsg( m_db )
                    << " on query: "
                    << sql << endl;
        return false;
    }

    return true;
}

QString AlbumDB::escapeString(QString str) const
{
    str.replace( "'", "''" );
    return str;
}

QString AlbumDB::getItemCaption(Q_LLONG imageID)
{
    QStringList values;

    execSql( QString("SELECT caption FROM Images "
                     "WHERE id=%1;")
             .arg(imageID),
             &values );

    if (!values.isEmpty())
        return values[0];
    else
        return QString::null;
}

QString AlbumDB::getItemCaption(int albumID, const QString& name)
{
    QStringList values;

    execSql( QString("SELECT caption FROM Images "
                     "WHERE dirid=%1 AND name='%2';")
             .arg(albumID)
             .arg(escapeString(name)),
             &values );

    if (!values.isEmpty())
        return values[0];
    else
        return QString::null;
}

QDateTime AlbumDB::getItemDate(Q_LLONG imageID)
{
    QStringList values;

    execSql( QString("SELECT datetime FROM Images "
                     "WHERE id=%1;")
             .arg(imageID),
             &values );

    if (!values.isEmpty())
        return QDateTime();
    else
        return QDateTime::fromString(values[0], Qt::ISODate);
}

QStringList AlbumDB::getItemTagNames(Q_LLONG imageID)
{
    QStringList values;

    execSql( QString("SELECT name FROM Tags \n "
                     "WHERE id IN (SELECT tagid FROM ImageTags \n "
                     "             WHERE imageid=%1) \n "
                     "ORDER BY name;")
             .arg(imageID),
             &values );

    return values;
}

IntList AlbumDB::getItemTagIDs(Q_LLONG imageID)
{
    QStringList values;

    execSql( QString("SELECT tagid FROM ImageTags \n "
                     "WHERE imageID=%1;")
             .arg(imageID),
             &values );

    IntList ids;

    if (values.isEmpty())
        return ids;

    for (QStringList::iterator it=values.begin(); it != values.end(); ++it)
    {
        ids << (*it).toInt();
    }
    return ids;
}

IntList AlbumDB::getItemCommonTagIDs(const LLongList& imageIDList)
{
    IntList ids;

    if (imageIDList.isEmpty())
        return ids;

    QStringList values;

    QString sql = QString("SELECT DISTINCT tagid FROM ImageTags "
                          "WHERE imageid=%1 ")
                  .arg(imageIDList.first());

    LLongList::const_iterator iter = imageIDList.begin();
    ++iter;

    while (iter != imageIDList.end())
    {
        sql += QString(" OR imageid=%2 ")
               .arg(*iter);
        ++iter;
    }

    sql += QString(";");
    execSql( sql, &values );

    if (values.isEmpty())
        return ids;

    for (QStringList::iterator it=values.begin(); it != values.end(); ++it)
    {
        ids << (*it).toInt();
    }
    return ids;
}

void AlbumDB::setItemCaption(Q_LLONG imageID,const QString& caption)
{
    QStringList values;

    execSql( QString("UPDATE Images SET caption='%1' "
                     "WHERE id='%1';")
             .arg(escapeString(caption))
             .arg(imageID) );
}

void AlbumDB::setItemCaption(int albumID, const QString& name, const QString& caption)
{
    QStringList values;

    execSql( QString("UPDATE Images SET caption='%1' "
                     "WHERE dirid='%1' AND name='%2';")
             .arg(escapeString(caption))
             .arg(albumID)
             .arg(escapeString(name)) );
}

void AlbumDB::addItemTag(Q_LLONG imageID, int tagID)
{
    execSql( QString("REPLACE INTO ImageTags (imageid, tagid) "
                     "VALUES(%1, %2);")
                 .arg(imageID)
                 .arg(tagID) );
}

void AlbumDB::addItemTag(int albumID, const QString& name, int tagID)
{
    execSql( QString("REPLACE INTO ImageTags (imageid, tagid) \n "
                     "(SELECT id, %1 FROM Images \n "
                     " WHERE dirid=%2 AND name='%3');")
             .arg(tagID)
             .arg(albumID)
             .arg(escapeString(name)) );
}

void AlbumDB::removeItemTag(Q_LLONG imageID, int tagID)
{
    execSql( QString("DELETE FROM ImageTags "
                     "WHERE imageID=%1 AND tagid=%3;")
             .arg(imageID)
             .arg(tagID) );
}

void AlbumDB::removeItemAllTags(Q_LLONG imageID)
{
    execSql( QString("DELETE FROM ImageTags "
                     "WHERE imageID=%1;")
             .arg(imageID) );
}

QStringList AlbumDB::getItemNamesInAlbum(int albumID)
{
    QStringList values;
    execSql( QString("SELECT Images.name "
                     "FROM Images "
                     "WHERE Images.dirid=%1")
             .arg(albumID), &values );

    return values;
}

QStringList AlbumDB::getAllItemURLsWithoutDate()
{
    QStringList values;
    execSql( QString("SELECT Albums.url||'/'||Images.name "
                     "FROM Images, Albums "
                     "WHERE Images.dirid=Albums.Id "
                     "AND (Images.datetime is null or "
                     "     Images.datetime == '');"),
             &values );

    QString libraryPath = AlbumManager::instance()->getLibraryPath() + "/";
    for (QStringList::iterator it = values.begin(); it != values.end();
         ++it)
    {
        *it = libraryPath + *it;
    }

    return values;
}

int AlbumDB::getOrCreateAlbumId(const QString& folder)
{
    QStringList values;
    execSql( QString("SELECT id FROM Albums WHERE url ='%1';")
            .arg( escapeString(folder) ), &values);

    int albumID;
    if (values.isEmpty())
    {
        execSql( QString ("INSERT INTO Albums (url, date) "
                          "VALUES ('%1','%2')")
                 .arg(escapeString(folder))
                 .arg(QDateTime::currentDateTime().toString(Qt::ISODate)) );
        albumID = sqlite3_last_insert_rowid(m_db);
    } else
        albumID = values[0].toInt();

    return albumID;
}

Q_LLONG AlbumDB::addItem(int albumID,
                         const QString& name,
                         const QDateTime& datetime,
                         const QString& comment)
{
    execSql ( QString ("REPLACE INTO Images "
                       "( caption , datetime, name, dirid ) "
                       " VALUES ('%1','%2','%3',%4) " )
              .arg(escapeString(comment))
              .arg(datetime.toString(Qt::ISODate))
              .arg(escapeString(name))
              .arg(albumID) );

    return sqlite3_last_insert_rowid(m_db);
}

bool AlbumDB::setItemDate(Q_LLONG imageID,
                          const QDateTime& datetime)
{
    execSql ( QString ("UPDATE Images SET datetime='%1'"
                       "WHERE id=%1;")
              .arg(datetime.toString(Qt::ISODate))
              .arg(imageID) );

    return true;
}

bool AlbumDB::setItemDate(int albumID, const QString& name,
                          const QDateTime& datetime)
{
    execSql ( QString ("UPDATE Images SET datetime='%1'"
                       "WHERE dirid='%1' AND name='%2';")
              .arg(datetime.toString(Qt::ISODate))
              .arg(albumID)
              .arg(escapeString(name)) );

    return true;
}

QStringList AlbumDB::getItemURLsInTag(int tagID)
{
    QStringList values;

    QString basePath(AlbumManager::instance()->getLibraryPath());

    execSql( QString("SELECT Albums.url||'/'||Images.name FROM Images, Albums "
                     "WHERE Images.id IN (SELECT imageid FROM ImageTags WHERE tagid=%1) "
                     "AND Albums.id=Images.dirid;")
             .arg(tagID), &values );

    for (QStringList::iterator it = values.begin(); it != values.end(); ++it)
    {
        *it = basePath + *it;
    }

    return values;
}

QDate AlbumDB::getAlbumAverageDate(int albumID)
{
    QStringList values;
    execSql( QString("SELECT datetime FROM Images WHERE dirid=%1")
            .arg( albumID ), &values);

    int differenceInSecs = 0;
    int amountOfImages = 0;
    QDateTime baseDateTime;

    for (QStringList::iterator it = values.begin(); it != values.end(); ++it)
    {
        QDateTime itemDateTime = QDateTime::fromString( *it, Qt::ISODate );
        if (itemDateTime.isValid())
        {
            ++amountOfImages;
            if ( baseDateTime.isNull() )
                baseDateTime=itemDateTime;
            else
                differenceInSecs += itemDateTime.secsTo( baseDateTime );
        }
    }

    if ( amountOfImages > 0 )
    {
        QDateTime averageDateTime;
        averageDateTime.setTime_t( baseDateTime.toTime_t() -
                                   (int)( differenceInSecs/amountOfImages ) );
        return ( averageDateTime.date() );
    }
    else
        return QDate();
}

void AlbumDB::deleteItem(int albumID, const QString& file)
{
    execSql( QString("DELETE FROM Images "
                     "WHERE dirid=%1 AND name='%2';")
             .arg(albumID)
             .arg(escapeString(file)) );
}

void AlbumDB::setAlbumURL(int albumID, const QString& url)
{
    QString u = escapeString(url);

    // first delete any stale albums left behind
    execSql( QString("DELETE FROM Albums WHERE url = '%1'")
             .arg(u) );

    // now update the album url
    execSql( QString("UPDATE Albums SET url = '%1' WHERE id = %2;")
             .arg(u)
             .arg(albumID) );
}

void AlbumDB::setTagName(int tagID, const QString& name)
{
    execSql( QString("UPDATE Tags SET name='%1' WHERE id=%2;")
             .arg(escapeString(name))
             .arg(tagID) );
}

void AlbumDB::moveItem(int srcAlbumID, const QString& srcName,
                       int dstAlbumID, const QString& dstName)
{

    // first delete any stale database entries if any
    deleteItem(dstAlbumID, dstName);

    execSql( QString("UPDATE Images SET dirid=%1, name='%2' "
                     "WHERE dirid=%3 AND name='%4';")
             .arg(dstAlbumID)
             .arg(escapeString(dstName))
             .arg(srcAlbumID)
             .arg(escapeString(srcName)) );
}

void AlbumDB::copyItem(int srcAlbumID, const QString& srcName,
                       int dstAlbumID, const QString& dstName)
{
    // first delete any stale database entries if any
    deleteItem(dstAlbumID, dstName);

    execSql( QString("INSERT INTO Images (dirid, name, caption, datetime) "
                     "SELECT %1, '%2', caption, datetime FROM Images "
                     "WHERE dirid=%3 AND name='%4';")
             .arg(dstAlbumID)
             .arg(escapeString(dstName))
             .arg(srcAlbumID)
             .arg(escapeString(srcName)) );

    execSql( QString("INSERT INTO ImageTags (imageid, tagid) \n"
                     "SELECT I.id, T.tagid FROM Images AS I, ImageTags AS T WHERE \n"
                     "     I.id=(SELECT Images.id FROM Images WHERE \n"
                     "           dirid=%1 AND name='%2') \n"
                     "AND  T.tagid IN (SELECT tagid FROM ImageTags WHERE \n"
                     "                   imageid=(SELECT id FROM Images WHERE \n"
                     "                             dirid=%3 AND name='%3'))")
             .arg(dstAlbumID)
             .arg(escapeString(dstName))
             .arg(srcAlbumID)
             .arg(escapeString(srcName)) );
}

Q_LLONG AlbumDB::lastInsertedRow()
{
    return sqlite3_last_insert_rowid(m_db);    
}
