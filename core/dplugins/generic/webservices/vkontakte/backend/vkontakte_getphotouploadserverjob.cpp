/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "vkontakte_getphotouploadserverjob.h"
#include "vkontakte_uploadphotosjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>

namespace Vkontakte
{

GetPhotoUploadServerJob::GetPhotoUploadServerJob(const QString &accessToken, Vkontakte::UploadPhotosJob::Dest dest)
    : VkontakteJob(accessToken, getMethod(dest))
{
    m_dest = dest;

    m_aid = -1;
    m_gid = -1;
    m_uid = -1;
}

void GetPhotoUploadServerJob::initUploadAlbum(int aid, int gid)
{
    m_aid = aid;
    m_gid = gid;
}

// static
QString GetPhotoUploadServerJob::getMethod(Vkontakte::UploadPhotosJob::Dest dest)
{
    switch (dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
            return QLatin1String("photos.getUploadServer");
        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
            return QLatin1String("photos.getProfileUploadServer");
        case Vkontakte::UploadPhotosJob::DEST_WALL:
            return QLatin1String("photos.getWallUploadServer");
        default:
            return QLatin1String("");
    }
}

void GetPhotoUploadServerJob::prepareQueryItems()
{
    switch (m_dest)
    {
        case Vkontakte::UploadPhotosJob::DEST_ALBUM:
            if (m_aid == -1)
            {
                setError(KJob::UserDefinedError);
                setErrorText(QStringLiteral("m_aid not set."));
                qWarning() << "m_aid not set.";
            }

            addQueryItem(QStringLiteral("aid"), QString::number(m_aid));
            if (m_gid != -1)
                addQueryItem(QStringLiteral("gid"), QString::number(m_gid));
            break;

        case Vkontakte::UploadPhotosJob::DEST_PROFILE:
            // photos.getProfileUploadServer has not parameters
            break;

        case Vkontakte::UploadPhotosJob::DEST_WALL:
            if (m_uid != -1 && m_gid != -1)
            {
                setError(KJob::UserDefinedError);
                setErrorText(QStringLiteral("Only one parameter m_uid or m_gid should be set."));
                qWarning() << "Only one parameter m_uid or m_gid should be set.";
            }

            if (m_uid != -1)
                addQueryItem(QStringLiteral("uid"), QString::number(m_uid));
            if (m_gid != -1)
                addQueryItem(QStringLiteral("gid"), QString::number(m_gid));
            break;

        default:
            setError(KJob::UserDefinedError);
            setErrorText(QStringLiteral("Unsupported m_dest."));
            qWarning() << "Unsupported m_dest.";
            break;
    }
}

void GetPhotoUploadServerJob::handleData(const QJsonValue &data)
{
    if (!data.isObject())
    {
        return;
    }

    // TODO: simplify this code
    QJsonObject object = data.toObject();
    if (object.contains(QStringLiteral("upload_url")) &&
        object.value(QStringLiteral("upload_url")).isString())
    {
        m_uploadUrl = QUrl(object.value(QStringLiteral("upload_url")).toString());
    }
}

QUrl GetPhotoUploadServerJob::uploadUrl() const
{
    return m_uploadUrl;
}

} /* namespace Vkontakte */
