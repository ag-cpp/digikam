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

#include "playerwindow.h"

// Qt includes

#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QFileDialog>

// Local includes

#include "QtAVWidgets.h"

using namespace QtAV;

PlayerWindow::PlayerWindow(QWidget* const parent)
    : QWidget(parent)
{
    QtAV::Widgets::registerRenderers();
    setWindowTitle(QString::fromLatin1("QtAV simple player example"));
    m_player        = new AVPlayer(this);
    QVBoxLayout* vl = new QVBoxLayout();
    setLayout(vl);
    m_vo = new VideoOutput(this);

    if (!m_vo->widget())
    {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), QString::fromLatin1("Can not create video renderer"));

        return;
    }

    m_player->setRenderer(m_vo);
    vl->addWidget(m_vo->widget());
    m_slider = new QSlider();
    m_slider->setOrientation(Qt::Horizontal);

    connect(m_slider, SIGNAL(sliderMoved(int)),
            this, SLOT(seek(int)));

    connect(m_player, SIGNAL(positionChanged(qint64)),
            this, SLOT(updateSlider()));

    connect(m_player, SIGNAL(started()),
            this, SLOT(updateSlider()));

    vl->addWidget(m_slider);
    QHBoxLayout* hb = new QHBoxLayout();
    vl->addLayout(hb);
    m_openBtn       = new QPushButton(tr("Open"));
    m_captureBtn    = new QPushButton(tr("Capture video frame"));
    hb->addWidget(m_openBtn);
    hb->addWidget(m_captureBtn);

    m_preview       = new QLabel(tr("Capture preview"));
    m_preview->setFixedSize(200, 150);
    hb->addWidget(m_preview);

    connect(m_openBtn, SIGNAL(clicked()),
            this, SLOT(openMedia()));

    connect(m_captureBtn, SIGNAL(clicked()),
            this, SLOT(capture()));

    connect(m_player->videoCapture(), SIGNAL(imageCaptured(QImage)),
            this, SLOT(updatePreview(QImage)));

    connect(m_player->videoCapture(), SIGNAL(saved(QString)),
            this, SLOT(onCaptureSaved(QString)));

    connect(m_player->videoCapture(), SIGNAL(failed()),
            this, SLOT(onCaptureError()));
}

void PlayerWindow::openMedia()
{
    QString file = QFileDialog::getOpenFileName(0, tr( "Open a video"));

    if (file.isEmpty())
        return;

    m_player->play(file);
}

void PlayerWindow::seek(int pos)
{
    if (!m_player->isPlaying())
        return;

    m_player->seek(pos*1000LL); // to msecs
}

void PlayerWindow::updateSlider()
{
    m_slider->setRange(0, int(m_player->duration()/1000LL));
    m_slider->setValue(int(m_player->position()/1000LL));
}

void PlayerWindow::updatePreview(const QImage &image)
{
    m_preview->setPixmap(QPixmap::fromImage(image).scaled(m_preview->size()));
}

void PlayerWindow::capture()
{
    //m_player->captureVideo();

    m_player->videoCapture()->capture();
}

void PlayerWindow::onCaptureSaved(const QString &path)
{
    setWindowTitle(tr("saved to: ") + path);
}

void PlayerWindow::onCaptureError()
{
    QMessageBox::warning(0, QString::fromLatin1("QtAV video capture"), tr("Failed to capture video frame"));
}
