/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-05
 * Description : HotPixel settings container.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "hotpixelcontainer.h"

// Local includes

#include "dimg.h"
#include "dimgthreadedfilter.h"

namespace Digikam
{

HotPixelContainer::HotPixelContainer()
    : filterMethod(HotPixelFixer::QUADRATIC_INTERPOLATION)
{
}

bool HotPixelContainer::isDefault() const
{
    return (*this == HotPixelContainer());
}

bool HotPixelContainer::operator==(const HotPixelContainer& other) const
{
    return (
            (blackFramePath    == other.blackFramePath) &&
            (filterMethod      == other.filterMethod)
           );
}

void HotPixelContainer::writeToFilterAction(FilterAction& action, const QString& prefix) const
{
    action.addParameter(prefix + QLatin1String("blackframepath"), blackFramePath);
    action.addParameter(prefix + QLatin1String("filtermethod"),   (int)filterMethod);
}

HotPixelContainer HotPixelContainer::fromFilterAction(const FilterAction& action, const QString& prefix)
{
    HotPixelContainer settings;
    settings.blackFramePath = action.parameter(prefix + QLatin1String("blackframepath"),                                   settings.blackFramePath);
    settings.filterMethod   = (HotPixelFixer::InterpolationMethod)action.parameter(prefix + QLatin1String("filtermethod"), (int)settings.filterMethod);

    return settings;
}

} // namespace Digikam
