/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "GeoDataLod.h"
#include "GeoDataLod_p.h"

// Qt includes

#include <QDataStream>

namespace Marble
{
GeoDataLod::GeoDataLod()
    : GeoDataObject(),
      d(new GeoDataLodPrivate)
{
}

GeoDataLod::GeoDataLod(const GeoDataLod& other)
    : GeoDataObject(other),
      d(new GeoDataLodPrivate(*other.d))
{
}

GeoDataLod::~GeoDataLod()
{
    delete d;
}

bool GeoDataLod::operator==(const GeoDataLod& other) const
{
    return equals(other)
           && d->m_maxFadeExtent == other.d->m_maxFadeExtent
           && d->m_maxLodPixels == other.d->m_maxLodPixels
           && d->m_minFadeExtent == other.d->m_minFadeExtent
           && d->m_minLodPixels == other.d->m_minLodPixels;
}

bool GeoDataLod::operator!=(const GeoDataLod& other) const
{
    return !this->operator==(other);
}

const char* GeoDataLod::nodeType() const
{
    return GeoDataTypes::GeoDataLodType;
}


qreal GeoDataLod::minLodPixels() const
{
    return d->m_minLodPixels;
}


void GeoDataLod::setMinLodPixels(qreal pixels)
{
    d->m_minLodPixels = pixels;
}


qreal GeoDataLod::maxLodPixels() const
{
    return d->m_maxLodPixels;
}


void GeoDataLod::setMaxLodPixels(qreal pixels)
{
    d->m_maxLodPixels = pixels;
}


qreal GeoDataLod::minFadeExtent() const
{
    return d->m_minFadeExtent;
}


void GeoDataLod::setMinFadeExtent(qreal pixels)
{
    d->m_minFadeExtent = pixels;
}


qreal GeoDataLod::maxFadeExtent() const
{
    return d->m_maxFadeExtent;
}


void GeoDataLod::setMaxFadeExtent(qreal pixels)
{
    d->m_maxFadeExtent = pixels;
}


void GeoDataLod::pack(QDataStream& stream) const
{
    GeoDataObject::pack(stream);

    stream << d->m_minLodPixels << d->m_maxLodPixels;
    stream << d->m_minFadeExtent << d->m_maxFadeExtent;
}

void GeoDataLod::unpack(QDataStream& stream)
{
    GeoDataObject::unpack(stream);

    stream >> d->m_minLodPixels >> d->m_maxLodPixels;
    stream >> d->m_minFadeExtent >> d->m_maxFadeExtent;
}

GeoDataLod& GeoDataLod::operator=(const GeoDataLod& other)
{
    *d = *other.d;
    return *this;
}

} // namespace Marble
