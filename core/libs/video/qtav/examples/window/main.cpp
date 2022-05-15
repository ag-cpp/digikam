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

// Qt includes

#include <QGuiApplication>
#include <QWindow>
#include <QKeyEvent>

// Local includes

#include "QtAV.h"
#include "digikam_debug.h"

using namespace QtAV;

class PlayerWindow : public OpenGLWindowRenderer
{
    Q_OBJECT

public:

    PlayerWindow(QWindow* const parent = nullptr)
        : OpenGLWindowRenderer(NoPartialUpdate, parent)
    {
        player.setRenderer(this);
    }

    void play(const QString& file)
    {
        setTitle(file);
        player.setFile(file);
        player.play();
    }

protected:

    virtual void keyPressEvent(QKeyEvent* e)
    {
        int key = e->key();

        if      (key == Qt::Key_Space)
        {
            player.togglePause();
        }
        else if (key == Qt::Key_Left)
        {
            player.seekBackward();
        }
        else if (key == Qt::Key_Right)
        {
            player.seekForward();
        }
    }

private:

    AVPlayerCore player;
};

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    if (a.arguments().size() < 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote() << QString::asprintf("./window file");

        return 0;
    }

    PlayerWindow win;
    win.resize(600, 400);
    win.show();
    win.play(a.arguments().at(1));

    return a.exec();
}

#include "main.moc"
