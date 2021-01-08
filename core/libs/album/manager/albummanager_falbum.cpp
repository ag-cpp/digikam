/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-06-15
 * Description : Albums manager interface - Face Album helpers.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "albummanager_p.h"

namespace Digikam
{

QMap<int, int> AlbumManager::getFaceCount() const
{
    return d->fAlbumsCount;
}

QMap<int, int> AlbumManager::getUnconfirmedFaceCount() const
{
    return d->uAlbumsCount;
}

void AlbumManager::personItemsCount()
{
    if (d->personListJob)
    {
        d->personListJob->cancel();
        d->personListJob = nullptr;
    }

    TagsDBJobInfo jInfo;
    jInfo.setFaceFoldersJob();

    d->personListJob = DBJobsManager::instance()->startTagsJobThread(jInfo);

    connect(d->personListJob, SIGNAL(finished()),
            this, SLOT(slotPeopleJobResult()));

    connect(d->personListJob, SIGNAL(faceFoldersData(QMap<QString,QMap<int,int> >)),    // krazy:exclude=normalize
            this, SLOT(slotPeopleJobData(QMap<QString,QMap<int,int> >)));               // krazy:exclude=normalize
}

void AlbumManager::slotPeopleJobResult()
{
    if (!d->personListJob)
    {
        return;
    }

    if (d->personListJob->hasErrors())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to list face tags";

        // Pop-up a message about the error.
        DNotificationWrapper(QString(), d->personListJob->errorsList().first(),
                             nullptr, i18n("digiKam"));
    }

    d->personListJob = nullptr;
}

void AlbumManager::slotPeopleJobData(const QMap<QString, QMap<int, int> >& facesStatMap)
{
    if (facesStatMap.isEmpty())
    {
        return;
    }

    d->uAlbumsCount.clear();
    //I think this is a bug
    //Why autodetectedFace have all autodetected tags?
    //They should be in autodetectedPerson
    if (facesStatMap.contains(ImageTagPropertyName::autodetectedFace())) //autodetectedPerson
        d->uAlbumsCount = *facesStatMap.find(ImageTagPropertyName::autodetectedFace()); //autodetectedPerson

    d->fAlbumsCount.clear();

    if (ApplicationSettings::instance()->getShowFolderTreeViewItemsCount())
    {
        typedef QMap<int, int> IntIntMap;

        foreach (const IntIntMap& counts, facesStatMap)
        {
            QMap<int, int>::const_iterator it;

            for (it = counts.begin() ; it != counts.end() ; ++it)
            {
                d->fAlbumsCount[it.key()] += it.value();
            }
        }
    }

    emit signalFaceCountsDirty(d->fAlbumsCount, d->uAlbumsCount, d->toUpdatedFaces);

    d->toUpdatedFaces.clear();
}

} // namespace Digikam
