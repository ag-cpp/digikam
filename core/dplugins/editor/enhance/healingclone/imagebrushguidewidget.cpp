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

 ImageBrushGuideWidget::ImageBrushGuideWidget(QWidget* const parent):
     ImageRegionWidget(parent)
{

    activateState(HealingCloneState::SELECT_SOURCE);
    initializeSourceCursor();
}

 void ImageBrushGuideWidget::setDefaults()
 {
     this->default_h = this->height();
     this->default_w = this->width();
     this->float_h = default_h;
     this->float_w = default_w;
    setFocus();
  //  this->resetPixels();

 }

 double ImageBrushGuideWidget::getScaleRatio()
 {

     return this->width()/this->default_w;

 }


 void ImageBrushGuideWidget::mousePressEvent(QMouseEvent* e)
 {

      oldPos = e->globalPos() ;

      if(!this->amIFocused && this->currentState == HealingCloneState::PAINT)
      {
          this->amIFocused = true;
          return;
      }
      else if(!this->amIFocused)
      {
          this->amIFocused = true;
      }

      if( (this->currentState == HealingCloneState::PAINT || this->currentState == HealingCloneState::LASSO_CLONE))
      {
          if(this->cloneVectorChanged)
          {
              this->setCloneVectorChanged(false);
              qCDebug(DIGIKAM_DIMG_LOG()) << "emitting push to stack";
              emit signalPushToUndoStack();
          }
          else {
              qCDebug(DIGIKAM_DIMG_LOG()) << "clone vec didn't change - not pushing to undo stack";
          }
      }

      if (this->currentState == HealingCloneState::MOVE_IMAGE && (e->buttons() & Qt::LeftButton))
      {
          setCursor(Qt::ClosedHandCursor);
      }
     else if (srcSet)
     {
         ImageRegionWidget::mousePressEvent(e);
     }
     else if (this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY && (e->buttons() & Qt::LeftButton))
      {

         QPoint dst = QPoint(e->x(),e->y());
         emit signalLasso(mapToImageCoordinates(dst));
      }
     else
     {
         if (e->button() == Qt::LeftButton)
         {


             dst = mapToImageCoordinates(e->pos());
             qCDebug(DIGIKAM_DIMG_LOG()) <<"SRC AND DST :: "<< src << dst ;
             emit signalClone(src, dst);

         }

     }




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
    else if ( this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY && (e->buttons() & Qt::LeftButton))
     {
        QPoint dst = QPoint(e->x(),e->y());
        emit signalLasso(mapToImageCoordinates(dst));
     }
    else if ((e->buttons() & Qt::LeftButton) && !srcSet)
    {


        QPoint currentDst   =   mapToImageCoordinates(e->pos());
        QPoint currentSrc   =   src;
        QPoint orgDst       =   dst;
        currentSrc          =   QPoint(currentSrc.x() + currentDst.x() - orgDst.x(), currentSrc.y() + currentDst.y() - orgDst.y());

        // Source Cursor Update
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(src)); // sceneCoordinates
        tempCursorPosition = QPoint(tempCursorPosition.x() + currentDst.x() - orgDst.x(), tempCursorPosition.y() + currentDst.y() - orgDst.y());
        setSourceCursorPosition(tempCursorPosition);
        //

        emit signalClone(currentSrc, currentDst);

    }

    if (srcSet)
    {
        ImageRegionWidget::mouseMoveEvent(e);

    }


}

void ImageBrushGuideWidget::mouseReleaseEvent(QMouseEvent* e)
{


    ImageRegionWidget::mouseReleaseEvent(e);
    if (this->currentState == HealingCloneState::MOVE_IMAGE)
    {
        setCursor(Qt::OpenHandCursor);
    }

    else if (srcSet)
    {
        src = mapToImageCoordinates(e->pos());
        setSourceCursorPosition(mapToScene(e->pos()));

        undoSlotSetSourcePoint();

    }
    else
    {
        QPointF tempCursorPosition = mapToScene(mapFromImageCoordinates(src));
        setSourceCursorPosition(tempCursorPosition);

    }


}

void ImageBrushGuideWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
    {
        if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
        {
            slotLassoSelect();
        }
    }
}

void ImageBrushGuideWidget :: keyPressEvent(QKeyEvent *e)
{

    if(e->key() == Qt :: Key_M)
    {

        slotMoveImage();
    }

    else if(e->key() == Qt :: Key_L)
    {
        slotLassoSelect();
    }


    if(e->key() == Qt :: Key_Plus)
    {
        zoomPlus();

    }

    if(e->key() == Qt :: Key_Minus)
    {
        zoomMinus();

    }

    if(e->key() == Qt :: Key_BracketLeft)
    {
     emit signalDecreaseBrushRadius();
    }

    if(e->key() == Qt :: Key_BracketRight)
    {
        emit signalIncreaseBrushRadius();
    }

    if(e->matches(QKeySequence::Undo))
    {
       emit signalUndoClone();
    }

    if(e->matches(QKeySequence::Redo))
    {
       emit signalRedoClone();
    }

    QWidget::keyPressEvent(e);

}

bool ImageBrushGuideWidget::event(QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent && keyEvent->key() == Qt::Key_Escape
                &&
     this->currentState != HealingCloneState::PAINT)
        {
          keyEvent->accept();

          if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
          {
              if(!this->isLassoPointsVectorEmpty)
                slotLassoSelect();
              slotLassoSelect();
          }
          else if(this->currentState == HealingCloneState::LASSO_CLONE)
          {
              slotLassoSelect();
          }

          return true;
        }
        return QWidget::event(event);
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

    ImageRegionWidget::wheelEvent(e);
    return;
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
    activateState(HealingCloneState::SELECT_SOURCE);
}

void ImageBrushGuideWidget :: slotMoveImage()
{
    if(this->currentState == HealingCloneState::MOVE_IMAGE)
    {
        if(this->isLassoPointsVectorEmpty)
        {
            activateState(HealingCloneState::PAINT);
        }
        else
        {
            activateState(HealingCloneState::LASSO_CLONE);
            emit signalContinuePolygon();
        }
    }
    else
    {
        activateState(HealingCloneState::MOVE_IMAGE);
    }
}
void ImageBrushGuideWidget :: slotLassoSelect()
{
    if(this->currentState != HealingCloneState::LASSO_DRAW_BOUNDARY &&
            this->currentState != HealingCloneState::LASSO_CLONE)
    {
        activateState(HealingCloneState::LASSO_DRAW_BOUNDARY);
        emit signalResetLassoPoint();

    }
    else if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY) {

        if(this->isLassoPointsVectorEmpty)
        {
            activateState(HealingCloneState::PAINT);
        }
        else
        {
            activateState(HealingCloneState::LASSO_CLONE);
            emit signalContinuePolygon();
        }
      }
    else if(this->currentState == HealingCloneState::LASSO_CLONE)
    {
        activateState(HealingCloneState::PAINT);
        emit signalResetLassoPoint();


    }


}

void ImageBrushGuideWidget::undoSlotSetSourcePoint()
{
    srcSet = false;

    if(this->isLassoPointsVectorEmpty)
    {
        activateState(HealingCloneState::PAINT);
    }
    else
    {
        activateState(HealingCloneState::LASSO_CLONE);
        emit signalContinuePolygon();
    }
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

    this->sourceCursor->rect().setWidth(size -2);
    this->sourceCursor->rect().setHeight(size-2);
    this->sourceCursor->update();
}

void ImageBrushGuideWidget::changeCursorShape(QPixmap pixMap, float x = 0.5 , float y = 0.5)
{
 setCursor(QCursor(pixMap,x * pixMap.width(),y *pixMap.height()));
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

    this->float_h = this->default_h * zoomPercent/100.0;
    this->float_w = this->default_w  * zoomPercent/100.0;
    this->resize((int)this->float_w, (int)this->float_h);



}


void ImageBrushGuideWidget::resizeEvent(QResizeEvent* e)
{


    ImageRegionWidget::resizeEvent(e);
    return;
    emit signalResetLassoPoint();
    emit signalContinuePolygon();
    if(this->currentState == HealingCloneState::LASSO_CLONE
            || this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY)
    {
        activateState(HealingCloneState::PAINT);
    }

   // this->recenterOnMousePosition();
}


void ImageBrushGuideWidget::showEvent( QShowEvent* event ) {
    ImageRegionWidget::showEvent( event );
    activateState(HealingCloneState::SELECT_SOURCE);
}

void ImageBrushGuideWidget::recenterOnMousePosition()
{
   // QPoint diff = this->parentWidget()->mapFromGlobal(QCursor::pos()) -  this->pos();
   // QPoint newPos = this->parentWidget()->mapFromGlobal(QCursor::pos()) - diff;
/*    qCDebug(DIGIKAM_DIMG_LOG()) << "****************************";
    qCDebug(DIGIKAM_GENERAL_LOG()) <<  "Cursor[regular,translate,map] " << QCursor::pos() << ImageRegionWidget::translatePointPosition(QCursor::pos())
                                  << this->parentWidget()->mapFromGlobal(QCursor::pos());
    qCDebug(DIGIKAM_GENERAL_LOG()) << "this->pos [regular,translate,map] " <<this->pos() << ImageRegionWidget::translatePointPosition(this->pos())
                                   << this->parentWidget()->mapFromGlobal(this->pos());
    qCDebug(DIGIKAM_GENERAL_LOG()) << "****************************";
*/
    double parentWidthHalf = this->parentWidget()->width()/2.0;
    double parentHeightHalf = this->parentWidget()->height()/2.0;
    QPoint pos = this->pos(); // position of upper-left corner of the imagewidget, relative
    // to the upper-left corner of the screen (parent)
    QPoint cursorRelativePosition = this->parentWidget()->mapFromGlobal(QCursor::pos());
    QPoint centerPosition = QPoint(parentWidthHalf,parentHeightHalf);

    bool inside = false;

    if(cursorRelativePosition.x() > pos.x() && cursorRelativePosition.x() < pos.x()+this->width()
            && cursorRelativePosition.y() > pos.y() && cursorRelativePosition.y() < pos.y()+this->height())
    {
        inside = true;
    }
    QPoint diff = centerPosition - cursorRelativePosition;
    if(!inside)
    {
        diff = 0 * diff;
    }
    QPoint relPos = pos+diff;
    qCDebug(DIGIKAM_GENERAL_LOG()) << "pos,diff,rel, Inside" << pos << diff<< relPos<<   inside;
    this->move(relPos);
    //QCursor::setPos(centerPosition);

}
void ImageBrushGuideWidget::zoomPlus()
{

  //  this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->scale(1.2,1.2);
    return;
    this->float_h += .1 * this->default_h;
    this->float_w += .1 * this->default_w;
    int zoomPercent = ceil((this->float_h/this->default_h) * 100);
    emit signalZoomPercentChanged(zoomPercent);
    recenterOnMousePosition();

}

void ImageBrushGuideWidget::zoomMinus()
{
 //   this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->scale(.8,.8);
    return;

    this->float_h -= .1 * this->default_h;
    this->float_w -= .1 * this->default_w;
    int zoomPercent = floor((this->float_h/this->default_h) * 100);
    emit signalZoomPercentChanged(zoomPercent);
    recenterOnMousePosition();
}




void ImageBrushGuideWidget::setIsLassoPointsVectorEmpty(bool isEmpty)
{
    this->isLassoPointsVectorEmpty = isEmpty;
}
void ImageBrushGuideWidget :: activateState(HealingCloneState state)
{

    if(this->currentState == HealingCloneState::LASSO_DRAW_BOUNDARY &&
            state != HealingCloneState::LASSO_CLONE)
    {
        emit signalContinuePolygon();
    }
    this->currentState = state;
    if(state == HealingCloneState::PAINT)
    {
        changeCursorShape(Qt::blue);
    }
    else if(state == HealingCloneState::MOVE_IMAGE)
    {
        setCursor(Qt::OpenHandCursor);
    }
    else if(state == HealingCloneState::LASSO_DRAW_BOUNDARY)
    {
        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_LASSO_PEN.png")));
        changeCursorShape(pix,0,1);
    }
    else if(state == HealingCloneState::LASSO_CLONE)
    {
        changeCursorShape(Qt::blue);
    }
    else if(state == HealingCloneState::SELECT_SOURCE)
    {
  //      this->setSpotVisibleNoUpdate(true);
        QPixmap pix = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                     QLatin1String("digikam/data/healing_clone_SRC.png")));
        changeCursorShape(pix,0.5,0.5);

    }
}

void ImageBrushGuideWidget::setCloneVectorChanged(bool changed)
{
    this->cloneVectorChanged = changed;
}

QPoint ImageBrushGuideWidget::mapToImageCoordinates(QPoint point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    QPointF temp = item->zoomSettings()->mapZoomToImage(mapToScene(point)) ;
    return QPoint((int) temp.x(), (int) temp.y());
}

QPoint ImageBrushGuideWidget::mapFromImageCoordinates(QPoint point)
{

    ImageRegionItem* item = (ImageRegionItem*)this->item();
    return mapFromScene(item->zoomSettings()->mapImageToZoom(point));
}

void ImageBrushGuideWidget::initializeSourceCursor()
{
    const int DIAMETER = 10;
    this->sourceCursor = new QGraphicsEllipseItem(0, 0, DIAMETER, DIAMETER);
    this->sourceCursor->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    this->sourceCursor->setBrush(Qt::green);
    this->sourceCursor->setOpacity(.2);
    this->scene()->addItem(this->sourceCursor);
}

void ImageBrushGuideWidget::setSourceCursorPosition(QPointF topLeftPos)
{
    double dx = this->sourceCursor->rect().width()/2.0;
    double dy = this->sourceCursor->rect().width()/2.0;
    QPointF shiftedPos = QPointF(topLeftPos.x()-dx, topLeftPos.y()-dy);
    this->sourceCursor->setPos(shiftedPos);

    // check if source is inside scene

    if(topLeftPos.x() < 0 || topLeftPos.x() + dx > scene()->width() ||
            topLeftPos.y()<0 || topLeftPos.y() + dy > scene()->height())
    {
        this->sourceCursor->setVisible(false);
    }
    else {
        this->sourceCursor->setVisible(true);
    }


}
} // namespace DigikamEditorHealingCloneToolPlugin
