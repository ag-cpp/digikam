/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-09
 * Description : digiKam release ID header.
 *
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "digikam_version.h"

// Qt includes

#include <QDate>
#include <QTime>
#include <QLocale>
#include <QSysInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_gitversion.h"
#include "digikam_builddate.h"
#include "digikam_globals.h"
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

    return asDateTimeUTC(dt);
}

const QString additionalInformation()
{
    QString gitRev       = QLatin1String(GITVERSION);
    QString gitBra       = QLatin1String(GITBRANCH);

    QString ret = QString::fromUtf8("<table>");

    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("IRC: "))
                  .arg(QString::fromLatin1("<a href='https://libera.chat/'>irc.libera.chat</a> - #digikam"));
    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Feedback: "))
                  .arg(QString::fromLatin1("<a href='https://mail.kde.org/mailman/listinfo/digikam-users'>digikam-user@kde.org</a>"));
    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Documentation: "))
                  .arg(QString::fromLatin1("<a href='https://docs.digikam.org/en/index.html'>docs.digikam.org</a>"));
    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Build Date: "))
                  .arg(QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat));
    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Build Target: "))
                  .arg(QLatin1String(digikam_build_type));
    ret        += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Build Architecture: "))
                  .arg(QSysInfo::buildCpuArchitecture());

    if (
        !gitRev.isEmpty()                           &&
        !gitBra.isEmpty()                           &&
        !gitRev.startsWith(QLatin1String("unknow")) &&
        !gitRev.startsWith(QLatin1String("export")) &&
        !gitBra.startsWith(QLatin1String("unknow"))
       )
    {
        const int maxStringLength = 10;
        QString gitVer            = gitRev.left(maxStringLength / 2 - 2) +
                                    QLatin1String("...")                 +
                                    gitRev.right(maxStringLength / 2 - 1);

        ret    += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Git Revision: "))
                  .arg(QString::fromLatin1("<a href='https://invent.kde.org/graphics/digikam/commit/%1'>%2</a>").arg(gitRev).arg(gitVer));
        ret    += QString::fromUtf8("<tr><td>%1</td><td>%2</td></tr>")
                  .arg(i18n("Git Branch: "))
                  .arg(gitBra);
    }

    ret        += QString::fromUtf8("</table>");

    return ret;
}

} // namespace Digikam
