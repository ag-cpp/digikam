/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-05-08
 * Description : Service menu operation methods
 *
 * Copyright (C) 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DSERVICE_MENU_H
#define DIGIKAM_DSERVICE_MENU_H

// Qt includes

#include <QUrl>
#include <QString>
#include <QStringList>

// KDE includes

#include <kservice.h>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DServiceMenu
{
public:

    /**
     * Linux only: open file urls with the service.
     */
    static bool runFiles(KService* const service, const QList<QUrl>& urls);

    /**
     * Linux only: open file urls with the application command.
     */
    static bool runFiles(const QString& appCmd,
                         const QList<QUrl>& urls,
                         KService* const service = nullptr);

    /**
     * Linux only: return list of service available on desktop to open files.
     */
    static KService::List servicesForOpenWith(const QList<QUrl>& urls);

#ifdef Q_OS_MAC

    static QList<QUrl> MacApplicationForFileExtension(const QString& suffix);
    static bool        MacOpenFileWithApplication(const QUrl& fileUrl, const QUrl& appUrl);

#endif

};

} // namespace Digikam

#endif // DIGIKAM_DSERVICE_MENU_H
