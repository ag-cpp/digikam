/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-09
 * Description : digiKam release ID header.
 *
 * Copyright (C) 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_version.h"

// Qt includes

#include <QDate>
#include <QTime>
#include <QLocale>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_gitversion.h"
#include "digikam_builddate.h"
#include "daboutdata.h"

namespace Digikam
{

int digiKamMakeIntegerVersion(int major, int minor, int patch)
{
    return (((major) << 16) | ((minor) << 8) | (patch));
}

const QString digiKamVersion()
{
    return QLatin1String(digikam_version);
}

const QDateTime digiKamBuildDate()
{
    QDateTime dt = QDateTime::fromString(QLatin1String(BUILD_DATE), QLatin1String("yyyyMMddTHHmmss"));
    dt.setTimeSpec(Qt::UTC);

    return dt;
}

const QString additionalInformation()
{
    QString gitVer       = QLatin1String(GITVERSION);
    QString gitBra       = QLatin1String(GITBRANCH);
    QString ret          = i18n("IRC: %1 - #digikam\n"
                                "Feedback: %2\n\n"
                                "Build date: %3 (target: %4)",
                                QString::fromLatin1("<a href='https://libera.chat/'>irc.libera.chat</a>"),
                                QString::fromLatin1("<a href='https://mail.kde.org/mailman/listinfo/digikam-users'>digikam-user@kde.org</a>"),
                                QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat),
                                QLatin1String(digikam_build_type));

    if (
        !gitVer.isEmpty()                           &&
        !gitBra.isEmpty()                           &&
        !gitVer.startsWith(QLatin1String("unknow")) &&
        !gitVer.startsWith(QLatin1String("export")) &&
        !gitBra.startsWith(QLatin1String("unknow"))
       )
    {
        ret = i18n("IRC: %1 - #digikam\n"
                   "Feedback: %2\n\n"
                   "Build date: %3 (target: %4)\n"
                   "Revision: %5",
                   "Branch: %6"
                   QString::fromLatin1("<a href='https://libera.chat/'>irc.libera.chat</a>"),
                   QString::fromLatin1("<a href='https://mail.kde.org/mailman/listinfo/digikam-users'>digikam-user@kde.org</a>"),
                   QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat),
                   QLatin1String(digikam_build_type),
                   QString::fromLatin1("<a href='https://invent.kde.org/graphics/digikam/commit/%1'>%2</a>").arg(gitVer).arg(gitVer),
                   gitBra);
    }

    return ret;
}

} // namespace Digikam
