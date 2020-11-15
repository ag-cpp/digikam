/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-01
 * Description : Qt model view for Showfoto item - the delegate
 *
 * Copyright (C) 2013 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef ITEM_VIEW_SHOW_FOTO_DELEGATE_P_H
#define ITEM_VIEW_SHOW_FOTO_DELEGATE_P_H

#include "itemviewshowfotodelegate.h"

// Qt includes

#include <QPainter>
#include <QApplication>
#include <QFont>
#include <QModelIndex>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "thememanager.h"
#include "itempropertiestab.h"
#include "showfotoiteminfo.h"
#include "colorlabelwidget.h"
#include "digikam_debug.h"
#include "thumbnailsize.h"
#include "itemdelegateoverlay.h"

using namespace Digikam;

namespace ShowFoto
{

class ItemViewShowfotoDelegate;

class Q_DECL_HIDDEN ItemViewShowfotoDelegatePrivate
{
public:

    ItemViewShowfotoDelegatePrivate();
    virtual ~ItemViewShowfotoDelegatePrivate() = default;

    void init(ItemViewShowfotoDelegate* const _q);

    /// Resets cached rects. Remember to reimplement in subclass for added rects.
    virtual void clearRects();

public:

    int                       spacing;
    QSize                     gridSize;

    QRect                     rect;

    QPixmap                   regPixmap;
    QPixmap                   selPixmap;
    QVector<QPixmap>          ratingPixmaps;

    QFont                     font;
    QFont                     fontReg;
    QFont                     fontCom;
    QFont                     fontXtra;

    ThumbnailSize             thumbSize;

    ItemViewShowfotoDelegate* q;

    QRect                     oneRowRegRect;
    QRect                     oneRowComRect;
    QRect                     oneRowXtraRect;

    /// constant values for drawing
    int                       radius;
    int                       margin;

private:

    Q_DISABLE_COPY(ItemViewShowfotoDelegatePrivate)
};

} // namespace ShowFoto

#endif // ITEM_VIEW_SHOW_FOTO_DELEGATE_P_H
