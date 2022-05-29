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

#ifndef QTAV_EXAMPLE_WIDGET_H
#define QTAV_EXAMPLE_WIDGET_H

// Qt includes

#include <QWidget>
#include <QPushButton>

namespace QtAV
{
class GLWidgetRenderer2;
class AVPlayerCore;
class VideoOutput;
}

class Widget : public QWidget
{
    Q_OBJECT

public:

    explicit Widget(QWidget* const parent = nullptr);
    ~Widget();

public Q_SLOTS:

    void setVideo();
    void playVideo();
    void testRTSP();

private:

    QtAV::VideoOutput*  renderer;
    QtAV::AVPlayerCore* player[2];
    QPushButton*        play_btn[2];
    QPushButton*        file_btn[2];
};

#endif // QTAV_EXAMPLE_WIDGET_H
