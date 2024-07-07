/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "ViewParams.h"

namespace Marble
{

class Q_DECL_HIDDEN ViewParamsPrivate
{
public:

    ViewParamsPrivate();
    ~ViewParamsPrivate();

    MapQuality  m_stillQuality;
    MapQuality  m_animationQuality;

    // The context that is active now.
    ViewContext m_viewContext;

    // The quality that we are painting right now.
    MapQuality  m_mapQuality;


    // Parameters that determine the painting
    // Show/don't show options

    bool        m_showAtmosphere;

    bool        m_showClouds;
};

ViewParamsPrivate::ViewParamsPrivate()
    : m_stillQuality(HighQuality),
      m_animationQuality(LowQuality),
      m_viewContext(Still),
      m_mapQuality(m_stillQuality),
      // Show / don't show parameters
      m_showAtmosphere(true),
      m_showClouds(false)
{
}

ViewParamsPrivate::~ViewParamsPrivate()
{
}


ViewParams::ViewParams()
    : d(new ViewParamsPrivate)
{
}

ViewParams::~ViewParams()
{
    delete d;
}

MapQuality ViewParams::mapQuality(ViewContext viewContext) const
{
    if (viewContext == Still)
    {
        return d->m_stillQuality;
    }

    Q_ASSERT(viewContext == Animation);
    return d->m_animationQuality;
}

MapQuality ViewParams::mapQuality() const
{
    return d->m_mapQuality;
}

void ViewParams::setMapQualityForViewContext(MapQuality quality, ViewContext viewContext)
{
    if (viewContext == Still)
    {
        d->m_stillQuality = quality;
    }

    else if (viewContext == Animation)
    {
        d->m_animationQuality = quality;
    }

    if (d->m_viewContext == viewContext)
    {
        d->m_mapQuality = quality;
    }
}

ViewContext ViewParams::viewContext() const
{
    return d->m_viewContext;
}

void ViewParams::setViewContext(ViewContext viewContext)
{
    d->m_viewContext = viewContext;

    if (viewContext == Still)
    {
        d->m_mapQuality = d->m_stillQuality;
    }

    if (viewContext == Animation)
    {
        d->m_mapQuality = d->m_animationQuality;
    }
}

bool ViewParams::showAtmosphere() const
{
    return d->m_showAtmosphere;
}

void ViewParams::setShowAtmosphere(bool showAtmosphere)
{
    d->m_showAtmosphere = showAtmosphere;
}

bool ViewParams::showClouds() const
{
    return d->m_showClouds;
}

void ViewParams::setShowClouds(bool const showClouds)
{
    d->m_showClouds = showClouds;
}

} // namespace Marble
