/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-09-19
 * Description : Access to comments of an image in the database
 *
 * Copyright (C) 2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include <QSharedData>

// KDE includes

#include <klocale.h>

// Local includes

#include "albumdb.h"
#include "imagecomments.h"

namespace Digikam
{

class ImageCommentsPriv : public QSharedData
{
public:
    ImageCommentsPriv()
    {
        id = -1;
        unique = ImageComments::UniquePerLanguage;
    }

    qlonglong id;
    QList<CommentInfo> infos;
    QSet<int> dirty;
    ImageComments::UniqueBehavior unique;

    void languageMatch(const QString &fullCode, const QString &langCode,
                       int &fullCodeMatch, int &langCodeMatch, int &defaultCodeMatch, int &firstMatch) const
    {
        fullCodeMatch = -1;
        langCodeMatch = -1;
        defaultCodeMatch = -1;
        firstMatch = -1;

        if (infos.isEmpty())
        {
            return;
        }
        else
            firstMatch = 0; // index of first entry - at least we have one

        // First we search for a full match
        // Second for a match of the language code
        // Third for the default code
        // Fourth we return the first comment

        QString defaultCode("x-default");

        for (int i=0; i<infos.size(); i++)
        {
            const CommentInfo &info = infos[i];
            if (info.type == DatabaseComment::Comment)
            {
                if (info.language == fullCode)
                {
                    fullCodeMatch = i;
                    break;
                }
                else if (info.language.startsWith(langCode) && langCodeMatch == -1)
                    langCodeMatch = i;
                else if (info.language == defaultCode)
                    defaultCodeMatch = i;
            }
        }
    }
};

ImageComments::ImageComments(qlonglong imageid)
{
    d = new ImageCommentsPriv;
    d->id = imageid;
    DatabaseAccess access;
    d->infos = access.db()->getImageComments(imageid);
}

ImageComments::ImageComments(DatabaseAccess &access, qlonglong imageid)
{
    d = new ImageCommentsPriv;
    d->id = imageid;
    d->infos = access.db()->getImageComments(imageid);
}

ImageComments::~ImageComments()
{
    apply();
}

QString ImageComments::defaultComment(int *index) const
{
    KLocale *locale = KGlobal::locale();
    QString langCode = locale->language().toLower() + '-';
    QString fullCode = langCode + locale->country().toLower();

    int fullCodeMatch, langCodeMatch, defaultCodeMatch, firstMatch;

    d->languageMatch(fullCode, langCode, fullCodeMatch, langCodeMatch, defaultCodeMatch, firstMatch);

    int chosen = fullCodeMatch;
    if (chosen == -1)
        chosen = langCodeMatch;
    if (chosen == -1)
        chosen = defaultCodeMatch;
    if (chosen == -1)
        chosen = firstMatch;

    if (index)
        *index = chosen;

    if (chosen == -1)
        return QString();
    else
        return d->infos[chosen].comment;
}

QString ImageComments::commentForLanguage(const QString &languageCode, int *index, LanguageChoiceBehavior behavior) const
{
    int fullCodeMatch, langCodeMatch, defaultCodeMatch, firstMatch;

    // en-us => en-
    QString firstPart;
    if (languageCode == "x-default")
        firstPart = languageCode;
    else
        firstPart = languageCode.section('-', 0, 0, QString::SectionIncludeTrailingSep);

    d->languageMatch(languageCode, firstPart, fullCodeMatch, langCodeMatch, defaultCodeMatch, firstMatch);

    int chosen = fullCodeMatch;
    if (chosen == -1)
        chosen = langCodeMatch;
    if (chosen == -1 && behavior > ReturnMatchingLanguageOnly)
    {
        chosen = defaultCodeMatch;
        if (chosen == -1 && behavior == ReturnMatchingDefaultOrFirstLanguage)
            chosen = firstMatch;
    }

    if (index)
        *index = chosen;

    if (chosen == -1)
        return QString();
    else
        return d->infos[chosen].comment;
}

int ImageComments::numberOfComments() const
{
    return d->infos.size();
}



DatabaseComment::Type ImageComments::type(int index) const
{
    return d->infos[index].type;
}

QString ImageComments::language(int index) const
{
    return d->infos[index].language;
}

QString ImageComments::author(int index) const
{
    return d->infos[index].author;
}

QDateTime ImageComments::date(int index) const
{
    return d->infos[index].date;
}

QString ImageComments::comment(int index) const
{
    return d->infos[index].comment;
}


void ImageComments::setUniqueBehavior(UniqueBehavior behavior)
{
    d->unique = behavior;
}

void ImageComments::addComment(const QString &comment, const QString &lang, const QString &author,
                               const QDateTime &date, DatabaseComment::Type type)
{
    bool multipleCommentsPerLanguage = (d->unique == UniquePerLanguageAndAuthor);
    QString language = lang;
    if (language.isNull())
        language = "x-default";

    for (int i=0; i<d->infos.size(); i++)
    {
        CommentInfo &info = d->infos[i];
        if (info.type == DatabaseComment::Comment && info.language == language)
        {
            if ( !multipleCommentsPerLanguage
                 || (multipleCommentsPerLanguage && info.author == author) )
            {
                changeComment(i, comment);
                changeAuthor(i, author);
                apply();
                return;
            }
        }
    }
    return addCommentDirect(comment, language, author, type, date);
}

void ImageComments::addHeadline(const QString &comment, const QString &lang,
                                const QString &author, const QDateTime &date)
{
    return addComment(comment, lang, author, date, DatabaseComment::Headline);
}

void ImageComments::addTitle(const QString &comment, const QString &lang,
                             const QString &author, const QDateTime &date)
{
    return addComment(comment, lang, author, date, DatabaseComment::Title);
}

void ImageComments::addCommentDirect(const QString &comment, const QString &language, const QString &author,
                                     DatabaseComment::Type type, const QDateTime &date)
{
    CommentInfo info;
    info.comment = comment;
    info.language = language;
    info.author = author;
    info.type = type;
    info.date = date;

    DatabaseAccess access;
    info.id = access.db()->setImageComment(d->id, comment, type, language, author, date);

    if (info.id != -1)
    {
        // overwrite or add
        for (int i=0; i<d->infos.size(); i++)
        {
            if (d->infos[i].id == info.id)
            {
                d->infos[i] = info;
                return;
            }
        }

        d->infos << info;
    }
}

void ImageComments::changeComment(int index, const QString &comment)
{
    d->infos[index].comment = comment;
    d->dirty << index;
}

void ImageComments::changeLanguage(int index, const QString &language)
{
    d->infos[index].language = language;
    d->dirty << index;
}

void ImageComments::changeAuthor(int index, const QString &author)
{
    d->infos[index].author = author;
    d->dirty << index;
}

void ImageComments::changeDate(int index, const QDateTime &date)
{
    d->infos[index].date = date;
    d->dirty << index;
}

void ImageComments::changeType(int index, DatabaseComment::Type type)
{
    d->infos[index].type = type;
    d->dirty << index;
}

void ImageComments::apply()
{
    DatabaseAccess access;
    apply(access);
}

void ImageComments::apply(DatabaseAccess &access)
{
     foreach(int index, d->dirty)
     {
         QVariantList values;
         CommentInfo &info = d->infos[index];
         values << (int)info.type << info.language << info.author << info.date << info.comment;
         access.db()->changeImageComment(info.id, values);
     }
     d->dirty.clear();
}

} // namespace Digikam

