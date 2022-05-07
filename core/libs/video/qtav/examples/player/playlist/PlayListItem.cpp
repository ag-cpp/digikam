/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include "PlayListItem.h"

// Qt includes

#include <QTime>
#include <QDataStream>

namespace QtAVPlayer
{

QDataStream& operator>> (QDataStream& s, PlayListItem& p)
{
    int stars;
    qint64 duration, last_time;
    QString url, title;
    s >> url >> title >> duration >> last_time >> stars;
    p.setTitle(title);
    p.setUrl(url);
    p.setStars(stars);
    p.setDuration(duration);
    p.setLastTime(last_time);

    return s;
}

QDataStream& operator<< (QDataStream& s, const PlayListItem& p)
{
    s << p.url() << p.title() << p.duration() << p.lastTime() << p.stars();

    return s;
}

PlayListItem::PlayListItem()
    : mStars(0),
      mLastTime(0),
      mDuration(0)
{
}

void PlayListItem::setTitle(const QString &title)
{
    mTitle = title;
}

QString PlayListItem::title() const
{
    return mTitle;
}

void PlayListItem::setUrl(const QString &url)
{
    mUrl = url;
}

QString PlayListItem::url() const
{
    return mUrl;
}

void PlayListItem::setStars(int s)
{
    mStars = s;
}

int PlayListItem::stars() const
{
    return mStars;
}

void PlayListItem::setLastTime(qint64 ms)
{
    mLastTime  = ms;
    mLastTimeS = QTime(0, 0, 0, 0).addMSecs(ms).toString(QString::fromLatin1("HH:mm:ss"));
}

qint64 PlayListItem::lastTime() const
{
    return mLastTime;
}

QString PlayListItem::lastTimeString() const
{
    return mLastTimeS;
}

void PlayListItem::setDuration(qint64 ms)
{
    mDuration  = ms;
    mDurationS = QTime(0, 0, 0, 0).addMSecs(ms).toString(QString::fromLatin1("HH:mm:ss"));
}

qint64 PlayListItem::duration() const
{
    return mDuration;
}

QString PlayListItem::durationString() const
{
    return mDurationS;
}

bool PlayListItem::operator ==(const PlayListItem& other) const
{
    return (url() == other.url());
}

} // namespace QtAVPlayer
