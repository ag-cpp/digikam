/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-07-09
 * Description : item tool tip configuration setup tab
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETUPTOOLTIP_H
#define SETUPTOOLTIP_H

// Qt includes

#include <QScrollArea>

namespace ShowFoto
{

class SetupToolTip : public QScrollArea
{
    Q_OBJECT

public:

    SetupToolTip(QWidget* parent=0);
    ~SetupToolTip();

    void applySettings();

private:

    void readSettings();

private:

    class SetupToolTipPriv;
    SetupToolTipPriv* const d;
};

}  // namespace ShowFoto

#endif // SETUPTOOLTIP_H
