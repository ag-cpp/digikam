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

#ifndef AV_PLAYER_CLICKABLE_MENU_H
#define AV_PLAYER_CLICKABLE_MENU_H

// Qt includes

#include <QMenu>

namespace AVPlayer
{

class ClickableMenu : public QMenu
{
    Q_OBJECT

public:

    explicit ClickableMenu(QWidget* parent = nullptr);
    explicit ClickableMenu(const QString& title, QWidget* parent = nullptr);

protected:

    virtual void mouseReleaseEvent(QMouseEvent*);
};

} // namespace AVPlayer

#endif // AV_PLAYER_CLICKABLE_MENU_H
