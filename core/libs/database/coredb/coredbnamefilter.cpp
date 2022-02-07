/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-04-02
 * Description : Core database file name filters based on file suffixes.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "coredbnamefilter.h"

// Qt includes

#include <QStringList>

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

CoreDbNameFilter::CoreDbNameFilter(const QString& filter)
{
    if ( filter.isEmpty() )
    {
        return;
    }

    QLatin1Char sep(';');
    int i = filter.indexOf( sep );

    if ( i == -1 && filter.indexOf(QLatin1Char(' ')) != -1)
    {
        sep = QLatin1Char(' ');
    }

    QStringList list               = filter.split(sep, QT_SKIP_EMPTY_PARTS);
    QStringList::const_iterator it = list.constBegin();

    while ( it != list.constEnd() )
    {
        QString pattern = QRegularExpression::anchoredPattern((*it).trimmed());
        QRegularExpression wildcard(QRegularExpression::wildcardToRegularExpression(pattern));
        wildcard.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        m_filterList << wildcard;
        ++it;
    }
}

bool CoreDbNameFilter::matches(const QString& name)
{
    QList<QRegularExpression>::const_iterator rit = m_filterList.constBegin();

    while ( rit != m_filterList.constEnd() )
    {
        if ( (*rit).match(name).hasMatch() )
        {
            return true;
        }

        ++rit;
    }

    return false;
}

} // namespace Digikam
