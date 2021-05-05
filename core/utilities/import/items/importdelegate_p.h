/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-07-08
 * Description : Qt item view to import items - the delegate (private container)
 *
 * Copyright (C) 2012      by Islam Wazery <wazery at ubuntu dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMPORT_DELEGATE_P_H
#define DIGIKAM_IMPORT_DELEGATE_P_H

#include "importdelegate.h"

// Qt includes

#include <QRect>
#include <QCache>

// Local includes

#include "importcategorizedview.h"
#include "itemviewimportdelegate_p.h"

namespace Digikam
{

class ImportCategoryDrawer;

class Q_DECL_HIDDEN ImportDelegate::ImportDelegatePrivate : public ItemViewImportDelegatePrivate
{
public:

    explicit ImportDelegatePrivate()
      : contentWidth        (0),
        drawImageFormat     (false),
        drawCoordinates     (false),
        drawFocusFrame      (true),
        drawMouseOverFrame  (true),
        ratingOverThumbnail (false),
        categoryDrawer      (nullptr),
        currentView         (nullptr),
        currentModel        (nullptr)
    {
        actualPixmapRectCache.setMaxCost(250);
    }

    int                    contentWidth;

    QRect                  dateRect;
    QRect                  pixmapRect;
    QRect                  nameRect;
/*
    QRect                  titleRect;
    QRect                  commentsRect;
*/
    QRect                  resolutionRect;
    QRect                  sizeRect;
    QRect                  downloadRect;
    QRect                  lockRect;
    QRect                  coordinatesRect;
    QRect                  tagRect;
    QRect                  imageInformationRect;
    QRect                  pickLabelRect;
    QRect                  groupRect;

    bool                   drawImageFormat;
    bool                   drawCoordinates;
    bool                   drawFocusFrame;
    bool                   drawMouseOverFrame;
    bool                   ratingOverThumbnail;

    QCache<int, QRect>     actualPixmapRectCache;
    ImportCategoryDrawer*  categoryDrawer;

    ImportCategorizedView* currentView;
    QAbstractItemModel*    currentModel;

public:

    void clearRects() override;
};

// --- ImportThumbnailDelegate ----------------------------------------------------

class Q_DECL_HIDDEN ImportThumbnailDelegatePrivate : public ImportDelegate::ImportDelegatePrivate
{
public:

    explicit ImportThumbnailDelegatePrivate()
       : flow(QListView::LeftToRight)
    {
        drawFocusFrame      = false;       // switch off drawing of frames
        drawMouseOverFrame  = false;
        ratingOverThumbnail = true;        // switch off composing rating over background
    }

    void init(ImportThumbnailDelegate* const q);

public:

    QListView::Flow flow;
    QRect           viewSize;
};

// --- ImportNormalDelegate ----------------------------------------------------

class Q_DECL_HIDDEN ImportNormalDelegatePrivate : public ImportDelegate::ImportDelegatePrivate
{
public:

    explicit ImportNormalDelegatePrivate() {};
    ~ImportNormalDelegatePrivate() override;

    void init(ImportNormalDelegate* const q, ImportCategorizedView* const parent);
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_DELEGATE_P_H
