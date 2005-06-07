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

/** @file albumdb.h */

#ifndef ALBUMDB_H
#define ALBUMDB_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include "albuminfo.h"

typedef struct sqlite3 sqleet3; // hehe.

typedef QValueList<int>     IntList;
typedef QValueList<Q_LLONG> LLongList;

/**
 * This class is responsible for the communication
 * with the sqlite database.
 */
class AlbumDB
{
public:

    /**
     * Constructor
     */
    AlbumDB();
    
    /**
     * Destructor
     */
    ~AlbumDB();

    /**
     * Makes a connection to the database and makes sure all tables
     * are available.
     * @param path The database to open
     */
    void setDBPath(const QString& path);

    /**
     * Returns all albums and their attributes in the database
     * @return a list of albums and their attributes
     */
    AlbumInfo::List scanAlbums();
    
    /**
     * Returns all tags and their attributes in the database
     * @return a list of tags and their attributes
     */
    TagInfo::List scanTags();

    /**
     * Add a new album to the database with the given attributes
     * @param url        url of the album
     * @param caption    the album caption
     * @param date       the date for the album
     * @param collection the album collection
     * @return the id of the album added or -1 if it failed
     */
    int addAlbum(const QString& url, const QString& caption,
                 const QDate& date, const QString& collection);
    
    /**
     * Set a new url for the album. This will not affect the url
     * of the subalbums.
     * @param albumID the id of the album
     * @param url     the new url for the album
     */
    void setAlbumURL(int albumID, const QString& url);

    /**
     * Set a caption for the album.
     * @param albumID the id of the album
     * @param caption the new caption for the album
     */
    void setAlbumCaption(int albumID, const QString& caption);

    /**
     * Set a collection for the album.
     * @param albumID    the id of the album
     * @param collection the new collection for the album
     */
    void setAlbumCollection(int albumID, const QString& collection);

    /**
     * Set a date for the album.
     * @param albumID  the id of the album
     * @param date     the date for the album
     */
    void setAlbumDate(int albumID, const QDate& date);
    
    /**
     * Set the icon for the album.
     * @param albumID the id of the album
     * @param iconID  the id of the icon file
     */
    void setAlbumIcon(int albumID, Q_LLONG iconID);

    /**
     * Deletes an album from the database. This will not delete the
     * subalbums of the album.
     * @param albumID the id of the album
     */
    void deleteAlbum(int albumID);

    /**
     * Adds a new tag to the database with given name, icon and parent id.
     * @param parentTagID the id of the tag which will become the new tags parent
     * @param name        the name of the tag
     * @param icon        the id of the icon file
     * @return the id of the tag added or -1 if it failed
     */
    int addTag(int parentTagID, const QString& name, Q_LLONG iconID);
    
    /**
     * Set a new name for the tag. 
     * @param tagID the id of the tag
     * @param name  the new name for the tag
     */
    void setTagName(int tagID, const QString& name);

    /**
     * Set the icon for the tag.
     * @param tagID the id of the tag
     * @param iconID the id of the icon file
     */
    void setTagIcon(int tagID, Q_LLONG iconID);

    /**
     * Set the parent tagid for the tag. This is equivalent to reparenting
     * the tag
     * @param tagID          the id of the tag
     * @param newParentTagID the new parentid for the tag
     */
    void setTagParentID(int tagID, int newParentTagID);
    
    /**
     * Deletes a tag from the database. This will not delete the
     * subtags of the tag.
     * @param tagID the id of the tag
     */
    void deleteTag(int tagID);

    
    void beginTransaction();
    void commitTransaction();

    /**
     * This adds a keyword-value combination to the database Settings table
     * if the keyword already exists, the value will be replaced with the new
     * value.
     * @param keyword The keyword
     * @param value The value
     */
    void setSetting(const QString& keyword, const QString& value);

    /**
     * This function returns the value which is stored in the database
     * (table Settings).
     * @param keyword The keyword for which the value has to be returned.
     * @return The values which belongs to the keyword.
     */
    QString getSetting(const QString& keyword);

    /**
     * This is simple function to put a new Item in the database,
     * without checking if it already exists, but since albumID+name
     * has to be unique, it will simply replace the datetime and comment
     * for an already existing item. 
     * @param albumID The albumID where the file is located.
     * @param name The filename
     * @param datetime The datetime to be stored. Should try to let that be
     * the exif-datetime, but if not available the modification date.
     * @param comment The user comment as found in the exif-headers of the 
     * file.
     * @return the id of item added or -1 if it fails
     */
    Q_LLONG addItem(int albumID, const QString& name,
                    const QDateTime& datetime,
                    const QString& comment);
    
    /**
     * Update the date of a item to supplied date
     * @param imageID The ID of the item
     * @param datetime The datetime to be stored. Should try to let that be
     * the exif-datetime, but if not available the modification date.
     * @return It will always return true. Maybe that will change.
     */
    bool setItemDate(Q_LLONG imageID, const QDateTime& datetime);

    /**
     * Update the date of a item to supplied date
     * @param albumID The albumID where the file is located.
     * @param name The filename
     * @param datetime The datetime to be stored. Should try to let that be
     * the exif-datetime, but if not available the modification date.
     * @return It will always return true. Maybe that will change.
     */
    bool setItemDate(int albumID, const QString& name,
                     const QDateTime& datetime);
    
    /**
     * Set the caption for the item
     * @param imageID the id of the item
     * @param caption the caption for the item
     */
    void setItemCaption(Q_LLONG imageID, const QString& caption);

    /**
     * Set the caption for the item
     * @param albumID the albumID of the item
     * @param name    the name of the item
     * @param caption the caption for the item
     */
    void setItemCaption(int albumID, const QString& name, const QString& caption);
    
    /**
     * Add a tag for the item
     * @param imageID the ID of the item
     * @param tagID   the tagID for the tag
     */
    void addItemTag(Q_LLONG imageID, int tagID);

    /**
     * Add a tag for the item
     * @param albumID the albumID of the item
     * @param name    the name of the item
     * @param tagID   the tagID for the tag
     */
    void addItemTag(int albumID, const QString& name, int tagID);
    
    /**
     * Get the caption for the item
     * @param imageID the id  of the item
     * @return the caption for the item
     */
    QString getItemCaption(Q_LLONG imageID);

    /**
     * Get the caption for the item
     * @param albumID the albumID of the item
     * @param name    the name of the item
     * @return the caption for the item
     */
    QString getItemCaption(int albumID, const QString& name);
    
    /**
     * Get the datetime for the item
     * @param imageID the ID of the item
     * @return the datetime for the item
     */
    QDateTime getItemDate(Q_LLONG imageID);

    /**
     * Get a list of names of all the tags for the item
     * @param imageID the ID of the item
     * @return the list of names of all tags for the item
     */
    QStringList getItemTagNames(Q_LLONG imageID);

    /**
     * Get a list of IDs of all the tags for the item
     * @param imageID the ID of the item
     * @return the list of IDs of all tags for the item
     */
    IntList     getItemTagIDs(Q_LLONG imageID);

    /**
     * Given a set of items (identified by their IDs),
     * get a list of ID of all common tags
     * @param imageIDList a list of IDs of the items
     * @return the list of common IDs of the given items
     */
    IntList     getItemCommonTagIDs(const LLongList& imageIDList);

    /**
     * Remove a specific tag for the item
     * @param imageID the ID of the item
     * @param tagID   the tagID for the tag
     */
    void removeItemTag(Q_LLONG imageID, int tagID);

    /**
     * Remove all tags for the item
     * @param imageID the ID of the item
     */
    void removeItemAllTags(Q_LLONG imageID);

    /**
     * Deletes an item from the database.
     * @param albumID The id of the album.
     * @param file The filename of the file to delete.
     */
    void deleteItem(int albumID, const QString& file);
    
    /**
     * This can be used to find out the albumID for a given
     * folder. If it does not exist, it will be created and the
     * new albumID will be returned.
     * @param folder The folder for which you want the albumID
     * @return It returns the albumID for that folder.
     */
    int  getOrCreateAlbumId(const QString& folder);
    
    /**
     * Returns all items for a given albumid. This is used to
     * verify if all items on disk are consistent with the database
     * in the scanlib class.
     * @param albumID The albumID for which you want all items.
     * @return It returns a QStringList with the filenames.
     */
    QStringList getItemNamesInAlbum(int albumID);
    
    /**
     * Returns all items in the database without a date. This is used
     * in the scanlib class which tries to find out the date of the 
     * items, so the database holds the date for each item. This was
     * not the case untill the 0.8.0 release.
     * @return The path (starting from albumPath and including the 
     * the filename of all items.
     */
    QStringList getAllItemURLsWithoutDate();

    /**
     * Given a tagid, get a list of the url of all items in the tag
     * @param  tagID the id of the tag
     * @return a list of urls for the items in the tag. The urls are the
     * absolute path of the items
     */
    QStringList getItemURLsInTag(int tagID);

    QDate getAlbumAverageDate(int albumID);

    /**
     * Move the attributes of an item to a different item. Useful when
     * say a file is renamed
     * @param  srcAlbumID the id of the source album
     * @param  dstAlbumID the id of the destination album
     * @param  srcName    the name of the source file
     * @param  dstName    the name of the destination file
     */
    void moveItem(int srcAlbumID, const QString& srcName,
                  int dstAlbumID, const QString& dstName);

    /**
     * Copy the attributes of an item to a different item. Useful when
     * say a file is copied
     * @param  srcAlbumID the id of the source album
     * @param  dstAlbumID the id of the destination album
     * @param  srcName    the name of the source file
     * @param  dstName    the name of the destination file
     */
    void copyItem(int srcAlbumID, const QString& srcName,
                  int dstAlbumID, const QString& dstName);

    /**
     * This will execute a given SQL statement to the database.
     * @param sql The SQL statement
     * @param values This will be filled with the result of the SQL statement
     * @param debug If true, it will output the SQL statement 
     * @return It will return if the execution of the statement was succesfull
     */
    bool execSql(const QString& sql, QStringList* const values = 0, 
                 const bool debug = false);

    bool isValid() const { return m_valid; }
    
private:

    /**
     * Checks the available tables and creates them if they are not 
     * available.
     */
    void initDB();

    /**
     * Escapes text fields. This is needed for all queries to the database
     * which happens with an argument which is a text field. It makes sure
     * a ' is replaced with '', as this is needed for sqlite.
     * @param str String to escape
     * @return The escaped string
     */
    QString escapeString(QString str) const;

    void removeInvalidEntries();
    
    sqleet3* m_db;
    bool     m_valid;
};

#endif /* ALBUMDB_H */
