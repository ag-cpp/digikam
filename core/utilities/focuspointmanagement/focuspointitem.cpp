#include "focuspointitem.h"

// Qt includes

#include <QPainter>

// Local includes

#include "digikam_debug.h"
#include "assignnamewidgetstates.h"

namespace Digikam
{

FocusPointItem::FocusPointItem(QGraphicsItem* const parent)
    : RegionFrameItem(parent),
      m_color(QColor::fromRgb(0,0,0,255))
    //   m_widget       (nullptr),
    //   m_changer      (nullptr)
{
}

FocusPointItem::~FocusPointItem()
{
}

void FocusPointItem::setPoint(const FocusPoint& point)
{
    m_point = point;
    // updateCurrentTag();
    setEditable(false);

    switch(m_point.getType())
    {
        case FocusPoint::TypePoint::Inactive : 
            break;
        case FocusPoint::TypePoint::Selected :
            m_color = QColor::fromRgb(200,0,0);
            break;
        case FocusPoint::TypePoint::Infocus :
            m_color = QColor::fromRgb(0,200,0);
            break;
        case FocusPoint::TypePoint::SelectedInFocus :
            m_color = QColor::fromRgb(0,0,200);
            break;
    }
}

FocusPoint FocusPointItem::point() const
{
    return m_point;
}

void FocusPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const QColor borderColor = m_color;

    // will paint to the left and bottom of logical coordinates

    QRectF drawRect          = boundingRect();

    painter->setPen(borderColor);
    painter->drawRect(drawRect);
}

void FocusPointItem::setEditable(bool allowEdit)
{
    changeFlags(GeometryEditable, allowEdit);
}

} // namespace Digikam
