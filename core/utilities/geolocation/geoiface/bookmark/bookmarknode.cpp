/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-15
 * Description : a node container for GPS bookmarks
 *
 * Copyright (C) 2017-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "bookmarknode.h"

// Qt includes

#include <QFile>
#include <QDateTime>
#include <QSharedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN BookmarkNode::Private
{
public:

    explicit Private()
      : parent(nullptr),
        type(BookmarkNode::Root)
    {
    }

    BookmarkNode*        parent;
    Type                 type;
    QList<BookmarkNode*> children;
};

BookmarkNode::BookmarkNode(BookmarkNode::Type type, BookmarkNode* const parent)
    : QObject(0),
      d(new Private)
{
    expanded  = false;
    d->parent = parent;
    d->type   = type;

    if (parent)
    {
        parent->add(this);
    }
}

BookmarkNode::~BookmarkNode()
{
    if (d->parent)
    {
        d->parent->remove(this);
    }

    qDeleteAll(d->children);
    d->parent = nullptr;
    d->type   = BookmarkNode::Root;

    delete d;
}

bool BookmarkNode::operator==(const BookmarkNode& other) const
{
    if (url                 != other.url           ||
        title               != other.title         ||
        desc                != other.desc          ||
        expanded            != other.expanded      ||
        dateAdded           != other.dateAdded     ||
        d->type             != other.d->type       ||
        d->children.count() != other.d->children.count())
    {
        return false;
    }

    for (int i = 0 ; i < d->children.count() ; ++i)
    {
        if (!((*(d->children[i])) == (*(other.d->children[i]))))
        {
            return false;
        }
    }

    return true;
}

BookmarkNode::Type BookmarkNode::type() const
{
    return d->type;
}

void BookmarkNode::setType(Type type)
{
    d->type = type;
}

QList<BookmarkNode*> BookmarkNode::children() const
{
    return d->children;
}

BookmarkNode* BookmarkNode::parent() const
{
    return d->parent;
}

void BookmarkNode::add(BookmarkNode* const child, int offset)
{
    Q_ASSERT(child->d->type != Root);

    if (child->d->parent)
    {
        child->d->parent->remove(child);
    }

    child->d->parent = this;

    if (offset == -1)
    {
        offset = d->children.size();
    }

    d->children.insert(offset, child);
}

void BookmarkNode::remove(BookmarkNode* const child)
{
    child->d->parent = nullptr;
    d->children.removeAll(child);
}

// -------------------------------------------------------

XbelReader::XbelReader()
{
}

BookmarkNode* XbelReader::read(const QString& fileName)
{
    QFile file(fileName);

    if (!file.exists() || !file.open(QFile::ReadOnly))
    {
        BookmarkNode* const root   = new BookmarkNode(BookmarkNode::Root);
        BookmarkNode* const folder = new BookmarkNode(BookmarkNode::RootFolder, root);
        folder->title = i18n("Bookmark folder");
        return root;
    }

    return read(&file, true);
}

BookmarkNode* XbelReader::read(QIODevice* const device, bool addRootFolder)
{
    BookmarkNode* const root = new BookmarkNode(BookmarkNode::Root);
    setDevice(device);

    if (readNextStartElement())
    {
        QString version = attributes().value(QLatin1String("version")).toString();

        if ((name() == QLatin1String("xbel")) &&
            (version.isEmpty() || version == QLatin1String("1.0")))
        {
            if (addRootFolder)
            {
                BookmarkNode* const folder = new BookmarkNode(BookmarkNode::RootFolder, root);
                folder->title = i18n("Bookmark folder");
                readXBEL(folder);
            }
            else
            {
                readXBEL(root);
            }
        }
        else
        {
            raiseError(i18n("The file is not an XBEL version 1.0 file."));
        }
    }

    return root;
}

void XbelReader::readXBEL(BookmarkNode* const parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("xbel"));

    while (readNextStartElement())
    {
        if      (name() == QLatin1String("folder"))
        {
            readFolder(parent);
        }
        else if (name() == QLatin1String("bookmark"))
        {
            readBookmarkNode(parent);
        }
        else if (name() == QLatin1String("separator"))
        {
            readSeparator(parent);
        }
        else
        {
            skipCurrentElement();
        }
    }
}

void XbelReader::readFolder(BookmarkNode* const parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("folder"));

    QSharedPointer<BookmarkNode> folder = QSharedPointer<BookmarkNode>(new BookmarkNode(BookmarkNode::Folder, parent));
    folder.data()->expanded              = (attributes().value(QLatin1String("folded")) == QLatin1String("no"));

    while (readNextStartElement())
    {
        if      (name() == QLatin1String("title"))
        {
            readTitle(folder.data());
        }
        else if (name() == QLatin1String("desc"))
        {
            readDescription(folder.data());
        }
        else if (name() == QLatin1String("folder"))
        {
            readFolder(folder.data());
        }
        else if (name() == QLatin1String("bookmark"))
        {
            readBookmarkNode(folder.data());
        }
        else if (name() == QLatin1String("separator"))
        {
            readSeparator(folder.data());
        }
        else
        {
            skipCurrentElement();
        }
    }
}

void XbelReader::readTitle(BookmarkNode* const parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("title"));
    parent->title = readElementText();
}

void XbelReader::readDescription(BookmarkNode* const parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("desc"));
    parent->desc = readElementText();
}

void XbelReader::readSeparator(BookmarkNode* const parent)
{
    QSharedPointer<BookmarkNode>(new BookmarkNode(BookmarkNode::Separator, parent));

    // empty elements have a start and end element
    readNext();
}

void XbelReader::readBookmarkNode(BookmarkNode* const parent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("bookmark"));

    QSharedPointer<BookmarkNode> bookmark = QSharedPointer<BookmarkNode>(new BookmarkNode(BookmarkNode::Bookmark, parent));
    bookmark.data()->url                  = attributes().value(QLatin1String("href")).toString();
    QString date                          = attributes().value(QLatin1String("added")).toString();
    bookmark.data()->dateAdded            = QDateTime::fromString(date, Qt::ISODate);

    while (readNextStartElement())
    {
        if (name() == QLatin1String("title"))
        {
            readTitle(bookmark.data());
        }
        else if (name() == QLatin1String("desc"))
        {
            readDescription(bookmark.data());
        }
        else
        {
            skipCurrentElement();
        }
    }

    if (bookmark.data()->title.isEmpty())
    {
        bookmark.data()->title = i18n("Unknown title");
    }
}

// -------------------------------------------------------

XbelWriter::XbelWriter()
{
    setAutoFormatting(true);
}

bool XbelWriter::write(const QString& fileName, const BookmarkNode* const root)
{
    QFile file(fileName);

    if (!root || !file.open(QFile::WriteOnly))
    {
        return false;
    }

    return write(&file, root);
}

bool XbelWriter::write(QIODevice* const device, const BookmarkNode* const root)
{
    setDevice(device);

    writeStartDocument();
    writeDTD(QLatin1String("<!DOCTYPE xbel>"));
    writeStartElement(QLatin1String("xbel"));
    writeAttribute(QLatin1String("version"), QLatin1String("1.0"));

    if (root->type() == BookmarkNode::Root)
    {
        BookmarkNode* const rootFolder = root->children().first();

        for (int i = 0  ; i < rootFolder->children().count() ; ++i)
        {
            writeItem(rootFolder->children().at(i));
        }
    }
    else
    {
        writeItem(root);
    }

    writeEndDocument();

    return true;
}

void XbelWriter::writeItem(const BookmarkNode* const parent)
{
    switch (parent->type())
    {
        case BookmarkNode::Folder:
            writeStartElement(QLatin1String("folder"));
            writeAttribute(QLatin1String("folded"), parent->expanded ? QLatin1String("no") : QLatin1String("yes"));
            writeTextElement(QLatin1String("title"), parent->title);

            for (int i = 0 ; i < parent->children().count() ; ++i)
            {
                writeItem(parent->children().at(i));
            }

            writeEndElement();
            break;

        case BookmarkNode::Bookmark:
            writeStartElement(QLatin1String("bookmark"));

            if (!parent->url.isEmpty())
            {
                writeAttribute(QLatin1String("href"), parent->url);
            }

            if (parent->dateAdded.isValid())
            {
                writeAttribute(QLatin1String("added"), parent->dateAdded.toString(Qt::ISODate));
            }

            if (!parent->desc.isEmpty())
            {
                writeAttribute(QLatin1String("desc"), parent->desc);
            }

            writeTextElement(QLatin1String("title"), parent->title);

            writeEndElement();
            break;

        case BookmarkNode::Separator:
            writeEmptyElement(QLatin1String("separator"));
            break;

        default:
            break;
    }
}

} // namespace Digikam
