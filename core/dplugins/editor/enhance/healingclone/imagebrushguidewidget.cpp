/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-15
 * Description : a brush for use with tool to replace part of the image using another
 *
 * Copyright (C) 2017      by Shaza Ismail Kaoud <shaza dot ismail dot k at gmail dot com>
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

#include "imagebrushguidewidget.h"
#include "overlaywidget.h"

// Local includes

#include "digikam_debug.h"

// KDE includes
#include <klocalizedstring.h>

namespace DigikamEditorHealingCloneToolPlugin
{

 ImageBrushGuideWidget::ImageBrushGuideWidget(QWidget* const parent ,
                          bool spotVisible,
                          int guideMode ,
                          const QColor& guideColor ,
                          int guideSize ,
                          bool blink ,
                          ImageIface::PreviewType type):
     ImageGuideWidget( parent, spotVisible, guideMode , guideColor,  guideSize , blink, type)
{

}

 void ImageBrushGuideWidget::setDefaults()
 {
     this->default_h = this->height();
     this->default_w = this->width();
     this->float_h = default_h;
     this->float_w = default_w;
    setFocus();

 }

 double ImageBrushGuideWidget::getScaleRatio()
 {
     if(first_time)
     {
         setDefaults();
         first_time = false;
     }

     return this->width()/this->default_w;

 }
void ImageBrushGuideWidget::mouseMoveEvent(QMouseEvent* e)
{


    if( this->currentState != HealingCloneState::MOVE_IMAGE)
        oldPos = e->globalPos() ;

    if ( this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
    {
        const QPoint delta = e->globalPos() - oldPos;
        move(x()+delta.x(), y()+delta.y());
        oldPos = e->globalPos();

    }
    else if ( this->currentState == HealingCloneState::LASSO_SELECT && (e->buttons() & Qt::LeftButton))
     {
        QPoint dst = QPoint(e->x(),e->y());
        //qCDebug(DIGIKAM_GENERAL_LOG()) << "Emitting Signal Lasso";
        emit signalLasso(translateItemPosition(dst, false));
     }
    else if ((e->buttons() & Qt::LeftButton) && !srcSet)
    {


        QPoint currentDst = QPoint(e->x(), e->y());

        currentDst        = translateItemPosition(currentDst, false);
        QPoint currentSrc = translateItemPosition(src, true);
        QPoint orgDst     = translateItemPosition(dst, false);
        currentSrc        = QPoint(currentSrc.x() + currentDst.x() - orgDst.x(), currentSrc.y() + currentDst.y() - orgDst.y());

        setSpotPosition(currentSrc);

        emit signalClone(currentSrc, currentDst);
    }

    if (srcSet)
    {
        ImageGuideWidget::mouseMoveEvent(e);

    }


}

void ImageBrushGuideWidget::mouseReleaseEvent(QMouseEvent* e)
{


    ImageGuideWidget::mouseReleaseEvent(e);
    if (this->currentState == HealingCloneState::MOVE_IMAGE)
    {
        setCursor(Qt::OpenHandCursor);
    }

    else if (srcSet)
    {
        src   = getSpotPosition();
        undoSlotSetSourcePoint();

    }
    else
    {
        QPoint p = translatePointPosition(src);
        setSpotPosition(p);

    }
}

void ImageBrushGuideWidget::mousePressEvent(QMouseEvent* e)
{

     oldPos = e->globalPos() ;

     if(!this->amIFocused)
     {
         this->amIFocused = true;
     }
     else if (this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
     {
         setCursor(Qt::ClosedHandCursor);
     }
    else if (srcSet)
    {
        ImageGuideWidget::mousePressEvent(e);
    }
    else if (this->currentState == HealingCloneState::LASSO_SELECT && (e->buttons() & Qt::LeftButton))
     {

        QPoint dst = QPoint(e->x(),e->y());
        emit signalLasso(translateItemPosition(dst, false));
     }
    else
    {
        if (e->button() == Qt::LeftButton)
        {
            dst = QPoint(e->x(), e->y());

            QPoint currentSrc = translateItemPosition(src, true);
            QPoint currentDst = translateItemPosition(dst, false);

            emit signalClone(currentSrc, currentDst);
        }

    }

}

void ImageBrushGuideWidget :: keyPressEvent(QKeyEvent *e)
{
    QWidget::keyPressEvent(e);

    if(e->key() == Qt :: Key_M)
    {

        if(this->currentState == HealingCloneState::MOVE_IMAGE)
        {
            this->currentState = HealingCloneState::PAINT;
            changeCursorShape(Qt::blue);
        }
        else
        {
            this->currentState = HealingCloneState::MOVE_IMAGE;
            setCursor(Qt::OpenHandCursor);
        }
    }

    else if(e->key() == Qt :: Key_L)
    {
        if(this->currentState != HealingCloneState::LASSO_SELECT)
        {
            this->currentState = HealingCloneState :: LASSO_SELECT;
            changeCursorShape(Qt::yellow);
            emit signalResetLassoPoint();
            this->resetPixels();
        }
        else {
            this->currentState = HealingCloneState :: PAINT;
            changeCursorShape(Qt::blue);
            emit signalContinuePolygon();
        }
    }


    if(e->key() == Qt :: Key_Plus)
    {
        zoomPlus();
        emit signalResetLassoPoint();
        emit signalContinuePolygon();
    }

    if(e->key() == Qt :: Key_Minus)
    {
        zoomMinus();
        emit signalResetLassoPoint();
        emit signalContinuePolygon();
    }

    if(e->key() == Qt :: Key_BracketLeft)
    {
     emit signalDecreaseBrushRadius();
    }

    if(e->key() == Qt :: Key_BracketRight)
    {
        emit signalIncreaseBrushRadius();
    }



}

void ImageBrushGuideWidget::  keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt :: Key_S)
    {
        if(this->currentState == HealingCloneState::SELECT_SOURCE)
        {
            undoSlotSetSourcePoint();
        }
        else
        {
            slotSetSourcePoint();
        }
    }
}

void ImageBrushGuideWidget:: wheelEvent(QWheelEvent *e)
{

    if(e->angleDelta().y() > 0)
        zoomPlus();
    else if (e->angleDelta().y() <0)
        zoomMinus();
}

void ImageBrushGuideWidget::focusOutEvent(QFocusEvent *event)
{
    this->amIFocused = false;
}

void ImageBrushGuideWidget::focusInEvent(QFocusEvent *event)
{

}
void ImageBrushGuideWidget::slotSetSourcePoint()
{
    srcSet = true;
    this->currentState = HealingCloneState :: SELECT_SOURCE;
    changeCursorShape(QColor(Qt::red));
}

void ImageBrushGuideWidget::undoSlotSetSourcePoint()
{
    srcSet = false;
    this->currentState = HealingCloneState :: PAINT;
    changeCursorShape(Qt::blue);
}
void ImageBrushGuideWidget::changeCursorShape(QColor color)
{
    int radius =this->brushRadius;
    int size = radius * 2;
    this->brushColor = color;
    int penSize = 2;
    QPixmap pix(size,size);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    p.setPen(QPen(color,penSize));
    p.setRenderHint(QPainter::Antialiasing, true);

    p.drawEllipse(1, 1, size-2, size-2);


    setCursor(QCursor(pix));
}

void ImageBrushGuideWidget :: updateCursor()
{
    changeCursorShape(this->brushColor);
}
void ImageBrushGuideWidget::setBrushRadius(int value)
{
    this->brushRadius = value;
    updateCursor();
}

void ImageBrushGuideWidget::zoomImage(int zoomPercent)
{

    if(first_time)
    {
        setDefaults();
        first_time = false;
    }
    this->float_h = this->default_h * zoomPercent/100.0;
    this->float_w = this->default_w  * zoomPercent/100.0;
    this->resize((int)this->float_w, (int)this->float_h);
    qCDebug(DIGIKAM_DPLUGIN_EDITOR_LOG) << "zooom " << zoomPercent << this->float_h << this->float_w;


}


void ImageBrushGuideWidget::resizeEvent(QResizeEvent* e)
{
    ImageGuideWidget::resizeEvent(e);
    emit signalReclone();

}

void ImageBrushGuideWidget::zoomPlus()
{
    if(first_time)
    {
        setDefaults();
        first_time = false;
    }
    this->float_h += .1 * this->default_h;
    this->float_w += .1 * this->default_w;
    this->resize((int)this->float_w, (int)this->float_h);
}

void ImageBrushGuideWidget::zoomMinus()
{
    if(first_time)
    {
        setDefaults();
        first_time = false;
    }
    this->float_h -= .1 * this->default_h;
    this->float_w -= .1 * this->default_w;
    this->resize((int)this->float_w, (int)this->float_h);
}

void ImageBrushGuideWidget::resetPixels()
{

    int w = (int)this->float_w;
    int h = (int) this->float_h;
    // This is a workaround. I am mainly using this to restore all lasso-colored pixels to the original image colors.
    // I am forcing a resize event with the same width and height, as a resizeEvent function in ImageGuideWidget already
    // does this resetting for me.
    QResizeEvent event(QSize(w,h),QSize(w,h));
    ImageGuideWidget::resizeEvent(&event);

    emit(signalReclone());
}

} // namespace DigikamEditorHealingCloneToolPlugin
