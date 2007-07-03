/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-28
 * Description : a color gradient widget
 * 
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef COLORGRADIENTWIDGET_H
#define COLORGRADIENTWIDGET_H

// KDE includes.

#include <Q3Frame>
#include <QColor>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ColorGradientWidgetPriv;

class DIGIKAM_EXPORT ColorGradientWidget : public Q3Frame
{
Q_OBJECT

public:

    enum Orientation
    {
        Horizontal=0,
        Vertical
    };

public:

    ColorGradientWidget( int o, int size, QWidget *parent=0 );
    ~ColorGradientWidget();

    void setColors( const QColor &col1, const QColor &col2 );

protected:

    void drawContents(QPainter *);

private:

    ColorGradientWidgetPriv* d;
};

}   // namespace Digikam

#endif /* COLORGRADIENTWIDGET_H */
