/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : TextureFilter threaded image filter.
 *
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_TEXTURE_FILTER_H
#define DIGIKAM_TEXTURE_FILTER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "texturecontainer.h"

namespace Digikam
{

class DIGIKAM_EXPORT TextureFilter : public DImgThreadedFilter
{

public:

    explicit TextureFilter(QObject* const parent = nullptr);
    explicit TextureFilter(DImg* const orgImage,
                           QObject* const parent=nullptr,
                           int blendGain=200,
                           const QString& texturePath=QString());

    ~TextureFilter();

    static QString          FilterIdentifier()
    {
        return QLatin1String("digikam:TextureFilter");
    }

    static QString          DisplayableName();

    static QList<int>       SupportedVersions()
    {
        return QList<int>() << 1;
    }

    static int              CurrentVersion()
    {
        return 1;
    }

    virtual QString         filterIdentifier()                          const override
    {
        return FilterIdentifier();
    }

    virtual FilterAction    filterAction()                                    override;
    void                    readParameters(const FilterAction& action)        override;

private:

    void filterImage()                                                        override;

private:

    TextureContainer m_settings;
};

} // namespace Digikam

#endif // DIGIKAM_TEXTURE_FILTER_H
