/* ============================================================
*
* This file is a part of digiKam project
* https://www.digikam.org
*
* Date        : 2013-09-13
* Description : rubber item for Canvas
*
* Copyright (C) 2013-2014 by Yiou Wang <geow812 at gmail dot com>
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

#ifndef DIGIKAM_RUBBER_ITEM_H
#define DIGIKAM_RUBBER_ITEM_H

// Qt includes

#include <QFlags>

// Local includes

#include "canvas.h"
#include "imagepreviewitem.h"
#include "regionframeitem.h"
#include "digikam_export.h"

class QWidget;

namespace Digikam
{

class DIGIKAM_EXPORT RubberItem : public RegionFrameItem
{
    Q_OBJECT

public:

    explicit RubberItem(ImagePreviewItem* const item);
    ~RubberItem() override;

    void setCanvas(Canvas* const canvas);

protected:

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event)    override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_RUBBER_ITEM_H
