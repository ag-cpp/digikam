/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-26
 * Description : A HEIF IO file for DImg framework
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Other HEIF loader implementions:
 *     https://github.com/KDE/krita/tree/master/plugins/impex/heif
 *     https://github.com/jakar/qt-heif-image-plugin
 *     https://github.com/ImageMagick/ImageMagick/blob/master/coders/heic.c
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

#ifndef DIGIKAM_DIMG_HEIF_LOADER_H
#define DIGIKAM_DIMG_HEIF_LOADER_H

// Local includes

#include "dimg.h"
#include "dimgloader.h"
#include "digikam_export.h"
#include "heif.h"

using namespace Digikam;

namespace Digikam
{

class DIGIKAM_EXPORT DImgHEIFLoader : public DImgLoader
{

public:

    explicit DImgHEIFLoader(DImg* const image);

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    virtual bool hasAlpha()   const override;
    virtual bool sixteenBit() const override;
    virtual bool isReadOnly() const override;

private:

    bool isHeifSuccess(struct heif_error* const error);

    // Read operations
    bool readHEICColorProfile(struct heif_image_handle* const image_handle);
    bool readHEICMetadata(struct heif_image_handle* const image_handle);
    bool readHEICImageByID(struct heif_context* const heif_context,
                           heif_item_id image_id);

    // Save operations
    bool saveHEICColorProfile(struct heif_image* const image);
    bool saveHEICMetadata(struct heif_context* const heif_context,
                          struct heif_image_handle* const image_handle);

private:

    bool                m_sixteenBit;
    bool                m_hasAlpha;

    DImgLoaderObserver* m_observer;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_HEIF_LOADER_H
