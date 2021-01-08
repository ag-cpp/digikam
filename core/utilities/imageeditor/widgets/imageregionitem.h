/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-07-25
 * Description : image region widget item for image editor.
 *
 * Copyright (C) 2013-2014 by Yiou Wang <geow812 at gmail dot com>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_REGION_ITEM_H
#define DIGIKAM_IMAGE_REGION_ITEM_H

// Qt includes

#include <QStyleOptionGraphicsItem>

// Local includes

#include "graphicsdimgitem.h"
#include "digikam_export.h"
#include "dimg.h"
#include "imageregionwidget.h"

namespace Digikam
{

class DIGIKAM_EXPORT ImageRegionItem : public GraphicsDImgItem
{
    Q_OBJECT

public:

    explicit ImageRegionItem(ImageRegionWidget* const view, bool paintExtras = true);
    ~ImageRegionItem()                                                                     override;

    void setTargetImage(DImg& img);
    void setHighLightPoints(const QPolygon& pointsList);
    void setRenderingPreviewMode(int mode);

    void  paintExtraData(QPainter* const painter);
    QRect getImageRegion() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent*)                                        override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*)                                        override;

private:

    class Private;
    Private* const dd;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_REGION_ITEM_H
