/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-22
 * Description : Building complex database SQL queries from search descriptions
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_QUERY_BUILDER_H
#define DIGIKAM_ITEM_QUERY_BUILDER_H

// Qt includes

#include <QVariant>
#include <QString>

// Local includes

#include "coredbsearchxml.h"
#include "itemqueryposthooks.h"
#include "digikam_export.h"

class QUrl;

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemQueryBuilder
{
public:

    explicit ItemQueryBuilder();

    QString buildQuery(const QString& q, QList<QVariant>* boundValues, ItemQueryPostHooks* const hooks) const;
    QString buildQueryFromUrl(const QUrl& url, QList<QVariant>* boundValues) const;
    QString buildQueryFromXml(const QString& xml, QList<QVariant>* boundValues, ItemQueryPostHooks* const hooks) const;
    QString convertFromUrlToXml(const QUrl& url) const;

    /**
     * Use for special queries where ImageTagProperties table is JOIN'ed.
     * (Default: false)
     */
    void setImageTagPropertiesJoined(bool isJoined);

public:

    static void addSqlOperator(QString& sql, SearchXml::Operator op, bool isFirst);
    static void addSqlRelation(QString& sql, SearchXml::Relation rel);
    static void addNoEffectContent(QString& sql, SearchXml::Operator op);

protected:

    void buildGroup(QString& sql, SearchXmlCachingReader& reader,
                    QList<QVariant>* boundValues, ItemQueryPostHooks* const hooks) const;
    bool buildField(QString& sql, SearchXmlCachingReader& reader, const QString& name,
                    QList<QVariant>* boundValues, ItemQueryPostHooks* const hooks) const;

    QString possibleDate(const QString& str, bool& exact) const;

protected:

    QString m_longMonths[12];
    QString m_shortMonths[12];
    bool    m_imageTagPropertiesJoined;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_QUERY_BUILDER_H
