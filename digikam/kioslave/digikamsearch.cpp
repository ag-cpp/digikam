/* ============================================================
 * File  : digikamsearch.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2005-04-21
 * Description :
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

#include <kglobal.h>
#include <klocale.h>
#include <kinstance.h>
#include <kfilemetainfo.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <kio/global.h>
#include <kio/ioslave_defaults.h>
#include <klargefile.h>

#include <qfile.h>
#include <qdatastream.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qdir.h>
#include <qvariant.h>
#include <qmap.h>

#include <config.h>

extern "C"
{
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <utime.h>
}

#include "digikamsearch.h"

kio_digikamsearch::kio_digikamsearch(const QCString &pool_socket,
                                     const QCString &app_socket)
    : SlaveBase("kio_digikamsearch", pool_socket, app_socket)
{
}

kio_digikamsearch::~kio_digikamsearch()
{
}

static QValueList<QRegExp> makeFilterList( const QString &filter )
{
    QValueList<QRegExp> regExps;
    if ( filter.isEmpty() )
        return regExps;

    QChar sep( ';' );
    int i = filter.find( sep, 0 );
    if ( i == -1 && filter.find( ' ', 0 ) != -1 )
        sep = QChar( ' ' );

    QStringList list = QStringList::split( sep, filter );
    QStringList::Iterator it = list.begin();
    while ( it != list.end() ) {
        regExps << QRegExp( (*it).stripWhiteSpace(), false, true );
        ++it;
    }
    return regExps;
}

static bool matchFilterList( const QValueList<QRegExp>& filters,
                             const QString &fileName )
{
    QValueList<QRegExp>::ConstIterator rit = filters.begin();
    while ( rit != filters.end() ) {
        if ( (*rit).exactMatch(fileName) )
            return true;
        ++rit;
    }
    return false;
}

void kio_digikamsearch::special(const QByteArray& data)
{
    QString libraryPath;
    KURL    url;
    QString filter;
    int     getDimensions;
    int     recurse;
    int     listingType = 0;

    QDataStream ds(data, IO_ReadOnly);
    ds >> libraryPath;
    ds >> url;
    ds >> filter;
    ds >> getDimensions;
    ds >> recurse;
    if (!ds.atEnd())
        ds >> listingType;

    if (m_libraryPath != libraryPath)
    {
        m_libraryPath = libraryPath;
        m_db.closeDB();
        m_db.openDB(libraryPath);
    }

    QValueList<QRegExp> regex = makeFilterList(filter);
    QByteArray ba;

    if (listingType == 0)
    {
        QString sqlQuery;

        // query head
        sqlQuery = "SELECT Images.id, Images.name, Images.dirid, Images.datetime, Albums.url "
                   "FROM Images, Albums "
                   "WHERE ( ";

        // query body
        sqlQuery += buildQuery(url);

        // query tail
        sqlQuery += " ) ";
        sqlQuery += " AND (Albums.id=Images.dirid); ";

        QStringList values;
        QString     errMsg;
        if (!m_db.execSql(sqlQuery, &values))
        {
            error(KIO::ERR_INTERNAL, errMsg);
            return;
        }

        Q_LLONG     imageid;
        QString     name;
        QString     path;
        int         dirid;
        QString     date;
        QString     purl;
        QSize       dims;
        struct stat stbuf;

        int  count = 0;
        QDataStream* os = new QDataStream(ba, IO_WriteOnly);

        for (QStringList::iterator it = values.begin(); it != values.end();)
        {
            imageid = (*it).toLongLong();
            ++it;
            name  = *it;
            ++it;
            dirid = (*it).toInt();
            ++it;
            date  = *it;
            ++it;
            purl  = *it;
            ++it;

            if (!matchFilterList(regex, name))
                continue;

            path = m_libraryPath + purl + "/" + name;
            if (::stat(QFile::encodeName(path), &stbuf) != 0)
                continue;

            dims = QSize();
            if (getDimensions)
            {
                KFileMetaInfo metaInfo(path);
                if (metaInfo.isValid())
                {
                    if (metaInfo.containsGroup("Jpeg EXIF Data"))
                    {
                        dims = metaInfo.group("Jpeg EXIF Data").
                               item("Dimensions").value().toSize();
                    }
                    else if (metaInfo.containsGroup("General"))
                    {
                        dims = metaInfo.group("General").
                               item("Dimensions").value().toSize();
                    }
                    else if (metaInfo.containsGroup("Technical"))
                    {
                        dims = metaInfo.group("Technical").
                               item("Dimensions").value().toSize();
                    }
                }
            }

            *os << imageid;
            *os << dirid;
            *os << name;
            *os << date;
            *os << stbuf.st_size;
            *os << dims;

            count++;

            if (count > 200)
            {
                delete os;
                os = 0;

                SlaveBase::data(ba);
                ba.resize(0);

                count = 0;
                os = new QDataStream(ba, IO_WriteOnly);
            }
        }

        delete os;
    }
    else
    {
        QString sqlQuery;

        // query head
        sqlQuery = "SELECT Albums.url||'/'||Images.name "
                   "FROM Images, Albums "
                   "WHERE ( ";

        // query body
        sqlQuery += buildQuery(url);

        // query tail
        sqlQuery += " ) ";
        sqlQuery += " AND (Albums.id=Images.dirid) ";
        sqlQuery += " LIMIT 500;";

        QStringList values;
        QString     errMsg;
        if (!m_db.execSql(sqlQuery, &values))
        {
            error(KIO::ERR_INTERNAL, errMsg);
            return;
        }

        QDataStream ds(ba, IO_WriteOnly);
        for (QStringList::iterator it = values.begin(); it != values.end(); ++it)
        {
            if (matchFilterList(regex, *it))
            {
                ds << m_libraryPath + *it;
            }
        }
    }

    SlaveBase::data(ba);


    finished();
}

QString kio_digikamsearch::buildQuery(const KURL& url) const
{
    int  count = url.queryItem("count").toInt();
    if (count <= 0)
        return QString();

    QMap<int, RuleType> rulesMap;

    for (int i=1; i<=count; i++)
    {
        RuleType rule;

        QString key = url.queryItem(QString::number(i) + ".key").lower();
        QString op  = url.queryItem(QString::number(i) + ".op").lower();

        if (key == "album")
        {
            rule.key = ALBUM;
        }
        else if (key == "albumname")
        {
            rule.key = ALBUMNAME;
        }
        else if (key == "albumcaption")
        {
            rule.key = ALBUMCAPTION;
        }
        else if (key == "albumcollection")
        {
            rule.key = ALBUMCOLLECTION;
        }
        else if (key == "imagename")
        {
            rule.key = IMAGENAME;
        }
        else if (key == "imagecaption")
        {
            rule.key = IMAGECAPTION;
        }
        else if (key == "imagedate")
        {
            rule.key = IMAGEDATE;
        }
        else if (key == "tag")
        {
            rule.key = TAG;
        }
        else if (key == "tagname")
        {
            rule.key = TAGNAME;
        }
        else
        {
            kdWarning() << "Unknown rule type: " << key << " passed to kioslave"
                        << endl;
            continue;
        }

        if (op == "eq")
            rule.op = EQ;
        else if (op == "ne")
            rule.op = NE;
        else if (op == "lt")
            rule.op = LT;
        else if (op == "gt")
            rule.op = GT;
        else if (op == "like")
            rule.op = LIKE;
        else if (op == "nlike")
            rule.op = NLIKE;
        else
        {
            kdWarning() << "Unknown op type: " << op << " passed to kioslave"
                        << endl;
            continue;
        }

        rule.val = url.queryItem(QString::number(i) + ".val");

        rulesMap.insert(i, rule);
    }

    QString sqlQuery;

    QStringList strList = QStringList::split(" ", url.path());
    for ( QStringList::Iterator it = strList.begin(); it != strList.end(); ++it )
    {
        bool ok;
        int  num = (*it).toInt(&ok);
        if (ok)
        {
            RuleType rule = rulesMap[num];
            sqlQuery += subQuery(rule.key, rule.op, rule.val);
        }
        else
        {
            sqlQuery += " " + *it + " ";
        }
    }

    return sqlQuery;
}

QString kio_digikamsearch::subQuery(enum kio_digikamsearch::SKey key,
                                    enum kio_digikamsearch::SOperator op,
                                    const QString& val) const
{
    QString query;

    switch (key)
    {
    case(ALBUM):
    {
        query = " (Images.dirid $$##$$ $$@@$$) ";
        break;
    }
    case(ALBUMNAME):
    {
        query = " (Images.dirid IN "
                "  (SELECT id FROM Albums WHERE url $$##$$ $$@@$$)) ";
        break;
    }
    case(ALBUMCAPTION):
    {
        query = " (Images.dirid IN "
                "  (SELECT id FROM Albums WHERE caption $$##$$ $$@@$$)) ";
        break;
    }
    case(ALBUMCOLLECTION):
    {
        query = " (Images.dirid IN "
                "  (SELECT id FROM Albums WHERE collection $$##$$ $$@@$$)) ";
        break;
    }
    case(TAG):
    {
        query = " (Images.id IN "
                "   (SELECT imageid FROM ImageTags "
                "    WHERE tagid $$##$$ $$@@$$)) ";
        break;
    }
    case(TAGNAME):
    {
        query = " (Images.id IN "
                "  (SELECT imageid FROM ImageTags "
                "   WHERE tagid IN "
                "   (SELECT id FROM Tags WHERE name $$##$$ $$@@$$))) ";
        break;
    }
    case(IMAGENAME):
    {
        query = " (Images.name $$##$$ $$@@$$) ";
        break;
    }
    case(IMAGECAPTION):
    {
        query = " (Images.caption $$##$$ $$@@$$) ";
        break;
    }
    case(IMAGEDATE):
    {
        query = " (Images.datetime $$##$$ $$@@$$) ";
        break;
    }
    }

    switch (op)
    {
    case(EQ):
    {
        query.replace("$$##$$", "=");
        query.replace("$$@@$$", QString::fromLatin1("'") + escapeString(val)
                      + QString::fromLatin1("'"));
        break;
    }
    case(NE):
    {
        query.replace("$$##$$", "<>");
        query.replace("$$@@$$", QString::fromLatin1("'") + escapeString(val)
                      + QString::fromLatin1("'"));
        break;
    }
    case(LT):
    {
        query.replace("$$##$$", "<");
        query.replace("$$@@$$", QString::fromLatin1("'") + escapeString(val)
                      + QString::fromLatin1("'"));
        break;
    }
    case(GT):
    {
        query.replace("$$##$$", ">");
        query.replace("$$@@$$", QString::fromLatin1("'") + escapeString(val)
                      + QString::fromLatin1("'"));
        break;
    }
    case(LIKE):
    {
        query.replace("$$##$$", "LIKE");
        query.replace("$$@@$$", QString::fromLatin1("'%") + escapeString(val)
                      + QString::fromLatin1("%'"));
        break;
    }
    case(NLIKE):
    {
        query.replace("$$##$$", "NOT LIKE");
        query.replace("$$@@$$", QString::fromLatin1("%") + escapeString(val)
                      + QString::fromLatin1("%"));
        break;
    }
    }

    return query;
}

/* KIO slave registration */

extern "C"
{
    int kdemain(int argc, char **argv)
    {
        KLocale::setMainCatalogue("digikam");
        KInstance instance( "kio_digikamsearch" );
        KGlobal::locale();

        if (argc != 4) {
            kdDebug() << "Usage: kio_digikamsearch  protocol domain-socket1 domain-socket2"
                      << endl;
            exit(-1);
        }

        kio_digikamsearch slave(argv[2], argv[3]);
        slave.dispatchLoop();

        return 0;
    }
}

