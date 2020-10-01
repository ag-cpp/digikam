/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-22
 * Description : Slideshow video viewer
 *
 * Copyright (C) 2014-2020 Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SLIDE_VIDEO_H
#define DIGIKAM_SLIDE_VIDEO_H

// Qt includes

#include <QWidget>
#include <QEvent>
#include <QUrl>

// QtAV includes

#include <QtAV/AVError.h>    // krazy:exclude=includes
#include <QtAV/AVPlayer.h>   // krazy:exclude=includes

// Local includes

#include "dinfointerface.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SlideVideo : public QWidget
{
    Q_OBJECT

public:

    explicit SlideVideo(QWidget* const parent);
    ~SlideVideo() override;

    void setInfoInterface(DInfoInterface* const iface);
    void setCurrentUrl(const QUrl& url);
    void showIndicator(bool);
    void pause(bool);
    void stop();

Q_SIGNALS:

    void signalVideoLoaded(bool);
    void signalVideoFinished();

    void signalVideoPosition(qint64);
    void signalVideoDuration(qint64);

private Q_SLOTS:

    void slotPlayerStateChanged(QtAV::AVPlayer::State state);
    void slotMediaStatusChanged(QtAV::MediaStatus status);
    void slotHandlePlayerError(const QtAV::AVError& err);

    void slotPositionChanged(qint64 position);
    void slotDurationChanged(qint64 duration);
    void slotVolumeChanged(int volume);
    void slotPosition(int position);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SLIDE_VIDEO_H
