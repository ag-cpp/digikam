/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : utilities for digiKam setup
 *
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Album
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Album Public License for more details.
 *
 * ============================================================ */

#include "setuputils.h"

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

QStringList cleanUserFilterString(QString filterString,
                                  const bool caseSensitive,
                                  const bool useSemicolon)
{
    if (!caseSensitive)
    {
        filterString = filterString.toLower();
    }

    QChar separator;

    if (!useSemicolon)
    {
        separator = QLatin1Char(' ');
        filterString.remove(QLatin1Char('*'));
        filterString.replace(QLatin1Char(';'), QLatin1Char(' '));
        filterString.replace(QLatin1String(" ."), QLatin1String(" "));
        filterString.replace(QLatin1String(" -."), QLatin1String(" -"));
    }
    else
    {
        separator = QLatin1Char(';');
        filterString.replace(QLatin1String(";."), QLatin1String(";"));
        filterString.replace(QLatin1String(";-."), QLatin1String(";-"));
    }

    QStringList filterList;

    Q_FOREACH (const QString& filter, filterString.split(separator, QT_SKIP_EMPTY_PARTS))
    {
        filterList << filter.trimmed();
    }

    return filterList;
}

} // namespace Digikam
