/* ============================================================
 * File  : imageinfo.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2005-04-21
 * Description : 
 * 
 * Copyright 2005 by Renchi Raju

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

/** @file imageinfo.cpp */

#include <qfile.h>
#include <kdebug.h>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

#include "album.h"
#include "albumdb.h"
#include "albummanager.h"
#include "dio.h"
#include "imageinfo.h"

AlbumManager* ImageInfo::m_man = 0;

ImageInfo::ImageInfo(Q_LLONG ID, int albumID, const QString& name,
                     const QDateTime& datetime, size_t size,
                     const QSize& dims)
    : m_ID(ID), m_albumID(albumID), m_name(name), m_datetime(datetime),
      m_size(size), m_dims(dims), m_viewitem(0)
{
    if (!m_man)
    {
        m_man = AlbumManager::instance();
    }
}

ImageInfo::~ImageInfo()
{
}

QString ImageInfo::name() const
{
    return m_name;
}

bool ImageInfo::setName(const QString& newName)
{
    KURL src = kurlForKIO();
    KURL dst = src.upURL();
    dst.addPath(newName);

    if (!DIO::renameFile(src, dst))
        return false;

    PAlbum* a = album();
    if (!a)
    {
        kdWarning() << "No album found for ID: " << m_albumID << endl;
        return false;
    }
    
    if (a->icon() == m_name)
    {
        QString err;
        AlbumManager::instance()->updatePAlbumIcon( a, newName,
                                                    false, err );
    }
    
    m_name = newName;
    return true;
}

size_t ImageInfo::fileSize() const
{
    return m_size;    
}

QDateTime ImageInfo::dateTime() const
{
    return m_datetime;
}

QSize ImageInfo::dimensions() const
{
    return m_dims;
}

Q_LLONG ImageInfo::id() const
{
    return m_ID;    
}

int ImageInfo::albumID() const
{
    return m_albumID;
}

PAlbum* ImageInfo::album() const
{
    return m_man->findPAlbum(m_albumID);
}

KURL ImageInfo::kurl() const
{
    PAlbum* a = album();
    if (!a)
    {
        kdWarning() << "No album found for ID: " << m_albumID << endl;
        return KURL();
    }
    
    KURL u(m_man->getLibraryPath());
    u.addPath(a->url());
    u.addPath(m_name);
    return u;
}

QString ImageInfo::filePath() const
{
    PAlbum* a = album();
    if (!a)
    {
        kdWarning() << "No album found for ID: " << m_albumID << endl;
        return QString();
    }

    QString path = m_man->getLibraryPath();
    path += a->url() + "/" + m_name;
    return path;
}

KURL ImageInfo::kurlForKIO() const
{
    PAlbum* a = album();
    if (!a)
    {
        kdWarning() << "No album found for ID: " << m_albumID << endl;
        return KURL();
    }

    KURL u(a->kurl());
    u.addPath(m_name);
    return u;
}

void ImageInfo::setViewItem(void *d)
{
    m_viewitem = d;
}

void* ImageInfo::getViewItem() const
{
    return m_viewitem;
}

void ImageInfo::setCaption(const QString& caption)
{
    AlbumDB* db  = m_man->albumDB();
    return db->setItemCaption(m_ID, caption);
}

QString ImageInfo::caption() const
{
    AlbumDB* db  = m_man->albumDB();
    return db->getItemCaption(m_ID);
}

QStringList ImageInfo::tagNames() const
{
    AlbumDB* db  = m_man->albumDB();
    return db->getItemTagNames(m_ID);
}

QStringList ImageInfo::tagPaths() const
{
    QStringList tagPaths;
    
    AlbumDB* db  = m_man->albumDB();
    IntList tagIDs = db->getItemTagIDs(m_ID);
    for (IntList::iterator it = tagIDs.begin(); it != tagIDs.end(); ++it)
    {
        TAlbum* ta = m_man->findTAlbum(*it);
        if (ta)
        {
            tagPaths.append(ta->url());
        }
    }

    return tagPaths;
}

QValueList<int> ImageInfo::tagIDs() const
{
    AlbumDB* db  = m_man->albumDB();
    return db->getItemTagIDs(m_ID);
}

void ImageInfo::setTag(int tagID)
{
    AlbumDB* db  = m_man->albumDB();
    db->addItemTag(m_ID, tagID);
}

void ImageInfo::removeTag(int tagID)
{
    AlbumDB* db  = m_man->albumDB();
    db->removeItemTag(m_ID, tagID);
}

void ImageInfo::removeAllTags()
{
    AlbumDB *db = m_man->albumDB();
    db->removeItemAllTags(m_ID);
}

void ImageInfo::refresh()
{
    m_datetime = m_man->albumDB()->getItemDate(m_ID);

    struct stat stbuf;
    stat(QFile::encodeName(filePath()), &stbuf);
    m_size = stbuf.st_size;
}

