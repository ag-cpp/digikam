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

#ifndef QTAV_EXAMPLE_PLAYERWINDOW_H
#define QTAV_EXAMPLE_PLAYERWINDOW_H

// Qt includes

#include <QWidget>

// Local includes

#include "QtAV.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
class QLabel;
class QCheckBox;
QT_END_NAMESPACE

class PlayerWindow : public QWidget
{
    Q_OBJECT

public:

    explicit PlayerWindow(QWidget* const parent = nullptr);

public Q_SLOTS:

    void openMedia();
    void seek(int);
    void capture();

private Q_SLOTS:

    void updateSlider();
    void updatePreview(const QImage& image);
    void onCaptureSaved(const QString& path);
    void onCaptureError();

private:

    QtAV::VideoOutput*  m_vo;
    QtAV::AVPlayerCore*     m_player;
    QSlider*            m_slider;
    QPushButton*        m_openBtn;
    QPushButton*        m_captureBtn;
    QLabel*             m_preview;
};

#endif // QTAV_EXAMPLE_PLAYERWINDOW_H
