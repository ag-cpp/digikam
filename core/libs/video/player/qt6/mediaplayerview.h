/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to embed QtMultimedia media player.
 *
 * Copyright (C) 2006-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef MEDIAPLAYERVIEW_H
#define MEDIAPLAYERVIEW_H

// Qt includes

#include <QStackedWidget>
#include <QEvent>
#include <QUrl>
#include <QMediaPlayer>

// Local includes

#include "digikam_export.h"
#include "dinfointerface.h"

namespace Digikam
{

class DIGIKAM_EXPORT MediaPlayerView : public QStackedWidget
{
    Q_OBJECT

public:

    explicit MediaPlayerView(QWidget* const parent);
    ~MediaPlayerView();

    void setCurrentItem(const QUrl& url   = QUrl(),
                        bool  hasPrevious = false,
                        bool  hasNext     = false);
    void setInfoInterface(DInfoInterface* const iface);
    void escapePreview();
    void reload();

Q_SIGNALS:

    void signalNextItem();
    void signalPrevItem();
    void signalEscapePreview();
    void signalFinished();

public Q_SLOTS:

    void slotEscapePressed();
    void slotRotateVideo();

private Q_SLOTS:

    void slotPlayerFinished();
    void slotPlayerStateChanged(QMediaPlayer::PlaybackState newState);
    void slotHandlePlayerError(QMediaPlayer::Error, const QString&);
    void slotThemeChanged();

    // Slidebar slots
    void slotPositionChanged(qint64 position);
    void slotDurationChanged(qint64 duration);
    void slotVolumeChanged(int volume);
    void slotLoopToggled(bool loop);
    void slotPosition(int position);
    void slotPausePlay();
    void slotCapture();
    void slotSliderPressed();
    void slotSliderReleased();

private:

    int  previewMode();
    void setPreviewMode(int mode);

private:

    class Private;
    Private* const d;
};

}  // namespace Digikam

#endif // MEDIAPLAYERVIEW_H
