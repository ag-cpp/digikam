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

#ifndef QTAV_AVFilterSubtitle_H
#define QTAV_AVFilterSubtitle_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include "LibAVFilter.h"
#include "AVPlayer.h"
using namespace QtAV;
class AVFilterSubtitle : public LibAVFilterVideo
{
    Q_OBJECT
    Q_PROPERTY(bool autoLoad READ autoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
public:
    explicit AVFilterSubtitle(QObject *parent = 0);
    void setPlayer(AVPlayer* player);
    QString setContent(const QString& doc); // return utf8 subtitle path
    bool setFile(const QString& filePath);
    QString file() const;
    bool autoLoad() const;

signals:
    void loaded();
    void loadError();
    void fileChanged(const QString& path);
    void autoLoadChanged(bool value);
public slots:
    // TODO: enable changed & autoload=> load
    void setAutoLoad(bool value);
    void findAndSetFile(const QString& path);
    void onPlayerStart();
private slots:
    void onStatusChanged();
private:
    bool m_auto;
    AVPlayer *m_player;
    QString m_file;
    // convert to utf8 to ensure ffmpeg can open it.
    QHash<QString,QString> m_u8_files;
};

#endif // QTAV_AVFilterSubtitle_H
