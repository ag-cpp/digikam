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

#ifndef QTAV_EXAMPLE_VIDEOPLAYER_H
#define QTAV_EXAMPLE_VIDEOPLAYER_H

// Qt includes

#include <QWidget>

// Local includes

#include "AVPlayerCore.h"
#include "GraphicsItemRenderer.h"


class QGraphicsView;


class VideoPlayer : public QWidget
{
    Q_OBJECT

public:

    explicit VideoPlayer(QWidget* const parent = nullptr);
    ~VideoPlayer();

    QSize sizeHint() const
    {
        return QSize(720, 640);
    }

    void play(const QString& file);

public Q_SLOTS:

    void setOpenGL(bool o = true);

private Q_SLOTS:

    void setOrientation(int value);
    void rotateVideo(int angle);
    void scaleVideo(int value);
    void open();

private:

    QtAV::AVPlayerCore          mediaPlayer;
    QtAV::GraphicsItemRenderer* videoItem;
    QGraphicsView*              view;
};

#endif // QTAV_EXAMPLE_VIDEOPLAYER_H
