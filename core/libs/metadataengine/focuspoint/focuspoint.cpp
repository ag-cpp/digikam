#include "focuspoint.h"

#include <QDebug>


namespace Digikam
{

FocusPoint::FocusPoint()
{
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height, TypePoint type)
{
    this->x_position    = x_position;
    this->y_position    = y_position;
    this->width         = width;
    this->height        = height;
    this->type          = type;
}

FocusPoint::FocusPoint(float x_position, float y_position, float width, float height)
{
    this->x_position    = x_position;
    this->y_position    = y_position;
    this->width         = width;
    this->height        = height;
}

FocusPoint::FocusPoint(QRectF rectF)
{
    this->x_position = rectF.topLeft().x() + rectF.width() * 0.5;
    this->y_position = rectF.topLeft().y() + rectF.height() * 0.5; 
    this->width      = rectF.width(); 
    this->height     = rectF.height();
}

FocusPoint::~FocusPoint()
{
}

void FocusPoint::SetType(TypePoint type)
{
    this->type          = type;
}

QRect FocusPoint::getRectBySize(const QSize& size) const
{
    return QRect(static_cast<int>((x_position - 0.5*width)*size.width()),
                 static_cast<int>((y_position - 0.5*height)*size.height()),
                 static_cast<int>(width*size.width()),
                 static_cast<int>(height*size.height()));
}

}
