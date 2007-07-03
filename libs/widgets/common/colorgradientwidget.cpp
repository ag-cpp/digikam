/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 * 
 * Date        : 2004-07-28
 * Description : a color gradient widget
 * 
 * Copyright (C) 2004-2007 by Gilles Caulier<caulier dot gilles at gmail dot com>
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

// Qt includes. 

#include <QImage>
#include <QPainter>
#include <QPixmap>

// KDE includes.

#include <kimageeffect.h>

// Local includes.

#include "colorgradientwidget.h"
#include "colorgradientwidget.moc"

namespace Digikam
{

class ColorGradientWidgetPriv
{

public:

    ColorGradientWidgetPriv(){}

    int    orientation;

    QColor color1;
    QColor color2;
};

ColorGradientWidget::ColorGradientWidget(int o, int size, QWidget *parent)
                   : Q3Frame(parent)
{
    d = new ColorGradientWidgetPriv;
    d->orientation = o;

    setFrameStyle(Q3Frame::Box|Q3Frame::Plain);
    setLineWidth(1);
    setAttribute(Qt::WA_DeleteOnClose);

    if ( d->orientation == Qt::Horizontal )
        setFixedHeight( size );
    else
        setFixedWidth( size );

    d->color1.setRgb( 0, 0, 0 );
    d->color2.setRgb( 255, 255, 255 );
}

ColorGradientWidget::~ColorGradientWidget()
{
    delete d;
}

void ColorGradientWidget::setColors( const QColor &col1, const QColor &col2 )
{
    d->color1 = col1;
    d->color2 = col2;
    update();
}

void ColorGradientWidget::drawContents(QPainter *p)
{
    QImage image(contentsRect().width(), contentsRect().height(), QImage::Format_ARGB32);

    QColor col, color1, color2;
    float scale;

    // Widget is disable : drawing grayed frame.
    if ( !isEnabled() )
    {
       color1 = palette().color(QPalette::Disabled, QPalette::Foreground);
       color2 = palette().color(QPalette::Disabled, QPalette::Background);
    }
    else 
    {
       color1 = d->color1;
       color2 = d->color2;
    }

    int redDiff   = color2.red()   - color1.red();
    int greenDiff = color2.green() - color1.green();
    int blueDiff  = color2.blue()  - color1.blue();

    if ( d->orientation == Qt::Vertical )
    {
        for ( int y = 0; y < image.height(); y++ )
        {
            scale = 1.0 * y / image.height();
            col.setRgb( color1.red()   + int(redDiff   * scale),
                        color1.green() + int(greenDiff * scale),
                        color1.blue()  + int(blueDiff  * scale) );

            unsigned int *p = (uint *) image.scanLine( y );

            for ( int x = 0; x < image.width(); x++ )
                *p++ = col.rgb();
        }
    }
    else
    {
        unsigned int *p = (uint *) image.scanLine( 0 );

        for ( int x = 0; x < image.width(); x++ )
        {
            scale = 1.0 * x / image.width();
            col.setRgb( color1.red()   + int(redDiff   * scale),
                        color1.green() + int(greenDiff * scale),
                        color1.blue()  + int(blueDiff  * scale) );
            *p++ = col.rgb();
        }

        for ( int y = 1; y < image.height(); y++ )
        {
            memcpy( image.scanLine( y ), image.scanLine( y - 1),
                    sizeof( unsigned int ) * image.width() );
        }
    }

    const int psize = 256;
    QColor ditherPalette[psize];

    for ( int s = 0; s < psize; s++ )
    {
        ditherPalette[s].setRgb( color1.red()   + redDiff   * s / psize,
                                 color1.green() + greenDiff * s / psize,
                                 color1.blue()  + blueDiff  * s / psize );
    }

    KImageEffect::dither(image, ditherPalette, psize);

    QPixmap pm = QPixmap::fromImage(image);
    p->drawPixmap(contentsRect(), pm);
}

}  // namespace Digikam
