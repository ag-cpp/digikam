/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-04-27
 * Description : image preview item for image editor.
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMAGE_PREVIEW_ITEM_H
#define DIGIKAM_IMAGE_PREVIEW_ITEM_H

// Local includes

#include "graphicsdimgitem.h"
#include "digikam_export.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QGraphicsSceneContextMenuEvent;

namespace Digikam
{

class DIGIKAM_EXPORT ImagePreviewItem : public GraphicsDImgItem
{
    Q_OBJECT

public:

    explicit ImagePreviewItem();
    ~ImagePreviewItem()         override;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:

    // Disable
    explicit ImagePreviewItem(QObject*) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_PREVIEW_ITEM_H
