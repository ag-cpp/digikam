/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-17
 * Description : low level files management interface - Finder classes
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2018      by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "diofinders.h"

// Qt includes

#include <QFileInfo>
#include <QSet>

// Local includes

#include "digikam_debug.h"
#include "iteminfo.h"
#include "dmetadata.h"
#include "metaenginesettings.h"

namespace Digikam
{

SidecarFinder::SidecarFinder(const QList<QUrl>& files)
{
    // First, the sidecar urls will be added so that they are first copied or renamed.

    foreach (const QUrl& url, files)
    {
        QString path(url.toLocalFile());

        if (!path.endsWith(QLatin1String(".xmp")) && DMetadata::hasSidecar(path))
        {
            localFiles << DMetadata::sidecarUrl(url);
            localFileSuffixes << QLatin1String(".xmp");
            qCDebug(DIGIKAM_DATABASE_LOG) << "Detected a sidecar" << localFiles.last();
        }

        foreach (const QString& ext, MetaEngineSettings::instance()->settings().sidecarExtensions)
        {
            QString suffix(QLatin1Char('.') + ext);
            QString sidecarPath(url.toLocalFile() + suffix);
            QUrl    sidecarUrl(QUrl::fromLocalFile(sidecarPath));

            if (QFileInfo::exists(sidecarPath) && !localFiles.contains(sidecarUrl))
            {
                localFiles        << sidecarUrl;
                localFileSuffixes << suffix;
                qCDebug(DIGIKAM_DATABASE_LOG) << "Detected a sidecar" << localFiles.last();
            }
        }
    }

    // Now the files, if the user has selected sidecars, these are ignored.

    foreach (const QUrl& url, files)
    {
        if (!localFiles.contains(url))
        {
            localFiles        << url;
            localFileSuffixes << QString();
        }
    }
}

// ------------------------------------------------------------------------------------------------

/**
 * TODO: Groups should not be resolved in dio, it should be handled in views.
 * This is already done for most things except for drag&drop, which is hard :)
 */
GroupedImagesFinder::GroupedImagesFinder(const QList<ItemInfo>& source)
{
    QSet<qlonglong> ids;

    foreach (const ItemInfo& info, source)
    {
        ids << info.id();
    }

    infos.reserve(source.size());

    foreach (const ItemInfo& info, source)
    {
        infos << info;

        if (info.hasGroupedImages())
        {
            foreach (const ItemInfo& groupedImage, info.groupedImages())
            {
                if (ids.contains(groupedImage.id()))
                {
                    continue;
                }

                infos << groupedImage;
                ids   << groupedImage.id();
            }
        }
    }
}

} // namespace Digikam
