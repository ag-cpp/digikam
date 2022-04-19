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

#ifndef QTAV_PLAYER_SUBTITLE_H
#define QTAV_PLAYER_SUBTITLE_H

// Qt includes

#include <QObject>
#include <QVariant>
#include <QVector>

// Local includes

#include "QtAV_Global.h"
#include "Packet.h"

namespace QtAV
{

class AVPlayer;
class Subtitle;

/*!
 * \brief The PlayerSubtitle class
 * Bind Subtitle to AVPlayer. Used by SubtitleFilter and QuickSubtitle.
 * Subtitle load priority: user specified file (setFile(...)) > auto load external (autoLoad() must be true) > embedded subtitle
 */
class Q_AV_PRIVATE_EXPORT PlayerSubtitle : public QObject
{
    Q_OBJECT

public:

    PlayerSubtitle(QObject *parent = 0);
    void setPlayer(AVPlayer* player);
    Subtitle* subtitle();

    /*!
     * \brief setFile
     * Load user selected subtitle. The subtitle will not change unless you manually setFile(QString()).
     */
    void setFile(const QString& file);
    QString file() const;

    /*!
     * \brief autoLoad
     * Auto find and load a suitable external subtitle if file() is not empty.
     */
    void setAutoLoad(bool value);
    bool autoLoad() const;

Q_SIGNALS:

    void autoLoadChanged(bool value);
    void fileChanged();

public Q_SLOTS:

    void onEnabledChanged(bool value);

private Q_SLOTS:

    void onPlayerSourceChanged();
    void onPlayerPositionChanged();
    void onPlayerStart();
    void tryReload();
    void tryReloadInternalSub();
    void updateInternalSubtitleTracks(const QVariantList& tracks);
    void processInternalSubtitlePacket(int track, const QtAV::Packet& packet);
    void processInternalSubtitleHeader(const QByteArray &codec, const QByteArray& data); // TODO: remove

private:

    void connectSignals();
    void disconnectSignals();
    void tryReload(int flag);   ///< 1: internal, 2: external, 3: internal+external

private:

    bool            m_auto;
    bool            m_enabled;  // TODO: m_enable_external
    AVPlayer*       m_player;
    Subtitle*       m_sub;
    QString         m_file;
    QVariantList    m_tracks;
    QVector<Packet> m_current_pkt;
};

}
#endif // QTAV_PLAYER_SUBTITLE_H
