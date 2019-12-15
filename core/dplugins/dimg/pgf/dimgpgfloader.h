/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-03
 * Description : A PGF IO file for DImg framework
 *
 * Copyright (C) 2009-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This implementation use LibPGF API <http://www.libpgf.org>
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

#ifndef DIGIKAM_DIMG_PGF_LOADER_H
#define DIGIKAM_DIMG_PGF_LOADER_H

// Local includes

#include "dimg.h"
#include "dimgloader.h"
#include "digikam_export.h"

using namespace Digikam;

namespace Digikam
{

class DIGIKAM_EXPORT DImgPGFLoader : public DImgLoader
{

public:

    explicit DImgPGFLoader(DImg* const image);
    ~DImgPGFLoader();

    bool load(const QString& filePath, DImgLoaderObserver* const observer) override;
    bool save(const QString& filePath, DImgLoaderObserver* const observer) override;

    bool hasAlpha()   const override;
    bool sixteenBit() const override;
    bool isReadOnly() const override;

private:

    bool progressCallback(double percent, bool escapeAllowed);

    static bool CallbackForLibPGF(double percent, bool escapeAllowed, void* data);

private:

    bool                m_sixteenBit;
    bool                m_hasAlpha;

    DImgLoaderObserver* m_observer;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_PGF_LOADER_H
