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

#include "AVFilterSubtitle.h"

// Qt includes

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

// Local includes

#include "digikam_debug.h"

namespace QtAVPlayer
{

AVFilterSubtitle::AVFilterSubtitle(QObject* parent)
    : LibAVFilterVideo(parent),
      m_auto(true),
      m_player(0)
{
/*
    connect(this, SIGNAL(statusChanged()),
            this, SLOT(onStatusChanged()));
*/
}

void AVFilterSubtitle::setPlayer(AVPlayer *player)
{
    if (m_player == player)
        return;

    uninstall();

    if (m_player)
    {
        disconnect(this);
    }

    m_player = player;

    if (!player)
        return;

    player->installFilter(this);

    if (m_auto)
    {
/*
        connect(player, SIGNAL(fileChanged(QString)),
                this, SLOT(findAndSetFile(QString)));
*/
        connect(player, SIGNAL(started()),
                this, SLOT(onPlayerStart()));
    }
}

bool AVFilterSubtitle::setFile(const QString &filePath)
{
    setOptions(QString());

    if (m_file != filePath)
    {
        emit fileChanged(filePath);

        // DO NOT return here because option is null now
    }

    m_file    = filePath;
    QString u = m_u8_files[filePath];

    if (!u.isEmpty() && !QFile(u).exists())
        u = QString();

    if (u.isEmpty())
    {
        QFile f(filePath);

        if (!f.open(QIODevice::ReadOnly))
        {
            qCWarning(DIGIKAM_QTAVPLAYER_LOG).noquote()
                << QString::asprintf("open '%s' error: %s",
                    filePath.toUtf8().constData(), f.errorString().toUtf8().constData());

            return false;
        }

        QTextStream ts(&f);
        ts.setAutoDetectUnicode(true);
        QString s   = ts.readAll();
        QString tmp = setContent(s);

        if (!tmp.isEmpty())
        {
            u                    = tmp;
            m_u8_files[filePath] = u;
        }
        else
        {
            // read the origin file

            qCWarning(DIGIKAM_QTAVPLAYER_LOG).noquote()
                << QString::asprintf("open cache file '%s' error, originanl subtitle file will be used",
                    filePath.toUtf8().constData());
        }
    }

    if (u.isEmpty())
        u = filePath;

    // filter_name=argument. use ' to quote the argument, use \ to escaping chars within quoted text. on windows, path can be C:/a/b/c, ":" must be escaped

    u.replace(QLatin1String(":"), QLatin1String("\\:"));
    setOptions(QString::fromLatin1("subtitles='%1'").arg(u));

    qCDebug(DIGIKAM_QTAVPLAYER_LOG).noquote()
        << QString::asprintf("subtitle loaded: %s", filePath.toUtf8().constData());

    return true;
}

QString AVFilterSubtitle::file() const
{
    return m_file;
}

QString AVFilterSubtitle::setContent(const QString &doc)
{
    QString name = QFileInfo(m_file).fileName();

    if (name.isEmpty())
        name = QString::fromLatin1("QtAV_u8_sub_cache");

    QFile w(QDir::temp().absoluteFilePath(name));

    if (w.open(QIODevice::WriteOnly))
    {
        w.write(doc.toUtf8());
        w.close();
    }
    else
    {
        return QString();
    }

    return w.fileName();
}

void AVFilterSubtitle::setAutoLoad(bool value)
{
    if (m_auto == value)
        return;

    m_auto = value;

    emit autoLoadChanged(value);

    if (!m_player || !m_auto)
        return;

    connect(m_player, SIGNAL(started()),
            this, SLOT(onPlayerStart()));
}

bool AVFilterSubtitle::autoLoad() const
{
    return m_auto;
}

void AVFilterSubtitle::findAndSetFile(const QString &path)
{
    QFileInfo fi(path);
    QDir dir(fi.dir());
    QString name     = fi.completeBaseName(); // video suffix has only 1 dot
    QStringList list = dir.entryList(QStringList() << name + QString::fromLatin1("*.ass") << name + QString::fromLatin1("*.ssa"), QDir::Files);
    list.append(dir.entryList(QStringList() << QString::fromLatin1("*.srt"), QDir::Files));

    foreach (const QString& f, list)
    {
        // why it happens?

        if (!f.startsWith(name))
            continue;

        if (setFile(dir.absoluteFilePath(f)))
            break;
    }
}

void AVFilterSubtitle::onPlayerStart()
{
    setOptions(QString());

    if (!autoLoad())
        return;

    findAndSetFile(m_player->file());
}

void AVFilterSubtitle::onStatusChanged()
{
    if      (status() == ConfigureOk)
    {
        emit loaded();
    }
    else if (status() == ConfigureFailed)
    {
        if (options().isEmpty())
            return;

        emit loadError();
    }
}

} // namespace QtAVPlayer
