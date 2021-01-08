/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-13-08
 * Description : Raw post processing corrections.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RAW_POST_PROCESSING_H
#define DIGIKAM_RAW_POST_PROCESSING_H

// Local includes

#include "dimgthreadedfilter.h"

using namespace Digikam;

namespace DigikamRawImportNativePlugin
{

class RawPostProcessing : public DImgThreadedFilter
{

public:

    explicit RawPostProcessing(DImg* const orgImage,
                               QObject* const parent = nullptr,
                               const DRawDecoding& settings = DRawDecoding());

    /**
     * Constructor for slave mode: execute immediately in current thread with specified master filter
     */
    RawPostProcessing(DImgThreadedFilter* const parentFilter,
                      const DImg& orgImage,
                      const DImg& destImage,
                      int progressBegin = 0,
                      int progressEnd = 100,
                      const DRawDecoding& settings = DRawDecoding());

    ~RawPostProcessing();

private:

    void rawPostProcessing();

    virtual void filterImage()                          override;

    /**
     * This filter is only for preview calculation.
     */
    virtual FilterAction filterAction()                 override
    {
        return FilterAction();
    }

    virtual void readParameters(const FilterAction&)    override
    {
    }

    virtual QString filterIdentifier() const            override
    {
        return QString();
    }

private:

    DRawDecoding m_customRawSettings;
};

} // namespace DigikamRawImportNativePlugin

#endif // DIGIKAM_RAW_POST_PROCESSING_H
