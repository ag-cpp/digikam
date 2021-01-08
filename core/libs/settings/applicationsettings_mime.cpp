/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-16-10
 * Description : application settings interface
 *
 * Copyright (C) 2003-2004 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2007      by Arnd Baecker <arnd dot baecker at web dot de>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2014      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#include "applicationsettings_p.h"

namespace Digikam
{

QString ApplicationSettings::getImageFileFilter() const
{
    QStringList imageSettings;
    CoreDbAccess().db()->getFilterSettings(&imageSettings, nullptr, nullptr);
    QStringList wildcards;

    foreach (const QString& suffix, imageSettings)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    return wildcards.join(QLatin1Char(' '));
}

QString ApplicationSettings::getMovieFileFilter() const
{
    QStringList movieSettings;
    CoreDbAccess().db()->getFilterSettings(nullptr, &movieSettings, nullptr);
    QStringList wildcards;

    foreach (const QString& suffix, movieSettings)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    return wildcards.join(QLatin1Char(' '));
}

QString ApplicationSettings::getAudioFileFilter() const
{
    QStringList audioSettings;
    CoreDbAccess().db()->getFilterSettings(nullptr, nullptr, &audioSettings);
    QStringList wildcards;

    foreach (const QString& suffix, audioSettings)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    return wildcards.join(QLatin1Char(' '));
}

QString ApplicationSettings::getRawFileFilter() const
{
    QStringList supportedRaws = DRawDecoder::rawFilesList();
    QStringList imageSettings;
    CoreDbAccess().db()->getFilterSettings(&imageSettings, nullptr, nullptr);

    // form intersection: those extensions that are supported as RAW as well in the list of allowed extensions

    for (QStringList::iterator it = supportedRaws.begin() ; it != supportedRaws.end() ; )
    {
        if (imageSettings.contains(*it))
        {
            ++it;
        }
        else
        {
            it = supportedRaws.erase(it);
        }
    }

    QStringList wildcards;

    foreach (const QString& suffix, supportedRaws)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    return wildcards.join(QLatin1Char(' '));
}

QString ApplicationSettings::getAllFileFilter() const
{
    QStringList imageFilter, audioFilter, videoFilter;
    CoreDbAccess().db()->getFilterSettings(&imageFilter, &videoFilter, &audioFilter);
    QStringList wildcards;

    foreach (const QString& suffix, imageFilter)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    foreach (const QString& suffix, audioFilter)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    foreach (const QString& suffix, videoFilter)
    {
        wildcards << QLatin1String("*.") + suffix;
    }

    return wildcards.join(QLatin1Char(' '));
}

} // namespace Digikam
