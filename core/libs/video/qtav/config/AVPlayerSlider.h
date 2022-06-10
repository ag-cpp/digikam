/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2006-2010 Ricardo Villalba <rvm at escomposlinux dot org>
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

// TODO: hover support (like youtube and ExMplayer timeline preview)

#ifndef QTAV_WIDGETS_AVPLAYER_SLIDER_H
#define QTAV_WIDGETS_AVPLAYER_SLIDER_H

// Qt includes

#include <QSlider>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class DIGIKAM_EXPORT AVPlayerSlider : public QSlider
{
    Q_OBJECT

public:

    explicit AVPlayerSlider(QWidget* const parent = nullptr);
    ~AVPlayerSlider();

Q_SIGNALS:

    void onEnter();
    void onLeave();
    void onHover(int pos, int value);

protected:

    virtual void enterEvent(QEvent* e);
    virtual void leaveEvent(QEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

    inline int pick(const QPoint& pt)                       const;
    int pixelPosToRangeValue(int pos)                       const;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_AVPLAYER_SLIDER_H
