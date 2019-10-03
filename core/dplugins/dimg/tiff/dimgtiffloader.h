/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-17
 * Description : A TIFF IO file for DImg framework
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Specifications & references:
 * - TIFF 6.0  : http://partners.adobe.com/public/developer/en/tiff/TIFF6.pdf
 * - TIFF/EP   : http://www.map.tu.chiba-u.ac.jp/IEC/100/TA2/recdoc/N4378.pdf
 * - TIFF/Tags : http://www.awaresystems.be/imaging/tiff/tifftags.html
 * - DNG       : http://www.adobe.com/products/dng/pdfs/dng_spec.pdf
 *
 * Others Linux Tiff Loader implementation using libtiff:
 * - http://websvn.kde.org/trunk/koffice/filters/krita/tiff/kis_tiff_converter.cc
 * - http://artis.inrialpes.fr/Software/TiffIO/
 * - http://cvs.graphicsmagick.org/cgi-bin/cvsweb.cgi/GraphicsMagick/coders/tiff.c
 * - http://freeimage.cvs.sourceforge.net/freeimage/FreeImage/Source/FreeImage/PluginTIFF.cpp
 * - http://freeimage.cvs.sourceforge.net/freeimage/FreeImage/Source/Metadata/XTIFF.cpp
 * - https://subversion.imagemagick.org/subversion/ImageMagick/trunk/coders/tiff.c
 *
 * Test images repository:
 * - http://www.remotesensing.org/libtiff/images.html
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

#ifndef DIGIKAM_DIMG_TIFF_LOADER_H
#define DIGIKAM_DIMG_TIFF_LOADER_H

// C ANSI includes

extern "C"
{
#include <tiffio.h>
#include <tiff.h>
}

// Local includes

#include "dimg.h"
#include "dimgloader.h"
#include "dmetadata.h"
#include "digikam_export.h"

using namespace Digikam;

namespace DigikamTIFFDImgPlugin
{

class DIGIKAM_EXPORT DImgTIFFLoader : public DImgLoader
{
public:

    explicit DImgTIFFLoader(DImg* const image);
    ~DImgTIFFLoader();

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    bool hasAlpha()   const override;
    bool sixteenBit() const override;
    bool isReadOnly() const override;

private:

    void tiffSetExifAsciiTag(TIFF* const tif, ttag_t tiffTag, const DMetadata& metaData, const char* const exifTagName);

    // cppcheck-suppress unusedPrivateFunction
    void tiffSetExifDataTag(TIFF* const tif, ttag_t tiffTag, const DMetadata& metaData, const char* const exifTagName);

    static void dimg_tiff_warning(const char* module, const char* format, va_list warnings);
    static void dimg_tiff_error(const char* module, const char* format, va_list errors);

private:

    bool m_sixteenBit;
    bool m_hasAlpha;
};

} // namespace DigikamTIFFDImgPlugin

#endif // DIGIKAM_DIMG_TIFF_LOADER_H
