/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Graphics View item for DImg - Private containers
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QPixmapCache>
#include <QQueue>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimg.h"
#include "dimgpreviewitem.h"
#include "imagezoomsettings.h"
#include "previewsettings.h"

namespace Digikam
{

class CachedPixmapKey
{
public:

    CachedPixmapKey() = default;

    QRect             region;
    QPixmapCache::Key key;
};

// -------------------------------------------------------------------------------

class CachedPixmaps
{
public:

    explicit CachedPixmaps(int maxCount = 2);
    ~CachedPixmaps();

    void setMaxCount(int);
    void clear();
    bool find(const QRect& region, QPixmap* const pix, QRect* const source);
    void insert(const QRect& region, const QPixmap& pixmap);

protected:

    int                     maxCount = 2;
    QQueue<CachedPixmapKey> keys;
};

// -------------------------------------------------------------------------------

class DIGIKAM_EXPORT GraphicsDImgItem::GraphicsDImgItemPrivate
{
public:

    GraphicsDImgItemPrivate() = default;

    void init(GraphicsDImgItem* const q);

public:

    DImg                  image;
    ImageZoomSettings     zoomSettings;
    mutable CachedPixmaps cachedPixmaps;
};

// -------------------------------------------------------------------------------

class PreviewLoadThread;
class DImgPreviewItem;

class DIGIKAM_EXPORT DImgPreviewItem::DImgPreviewItemPrivate : public GraphicsDImgItem::GraphicsDImgItemPrivate
{
public:

    DImgPreviewItemPrivate() = default;
    void init(DImgPreviewItem* const q);

public:

    DImgPreviewItem::State state            = DImgPreviewItem::NoImage;
    bool                   exifRotate       = false;
    int                    previewSize      = 1024;

    QString                path;
    PreviewSettings        previewSettings;
    PreviewLoadThread*     previewThread    = nullptr;
    PreviewLoadThread*     preloadThread    = nullptr;
    QStringList            pathsToPreload;
};

} // namespace Digikam
