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

#include "GeoDataTimeSpan.h"
#include "GeoDataTimeSpan_p.h"

// Qt includes

#include <QDataStream>
#include <QDateTime>

// Local includes

#include "GeoDataTypes.h"
#include "digikam_debug.h"

namespace Marble
{

GeoDataTimeSpan::GeoDataTimeSpan()
    : GeoDataTimePrimitive(),
      d(new GeoDataTimeSpanPrivate)
{
}

GeoDataTimeSpan::GeoDataTimeSpan(const GeoDataTimeSpan& other)
    : GeoDataTimePrimitive(other),
      d(new GeoDataTimeSpanPrivate(*other.d))
{
}

GeoDataTimeSpan::~GeoDataTimeSpan()
{
    delete d;
}

const char* GeoDataTimeSpan::nodeType() const
{
    return GeoDataTypes::GeoDataTimePrimitiveType;
}

const GeoDataTimeStamp& GeoDataTimeSpan::end() const
{
    return d->m_end;
}

GeoDataTimeStamp& GeoDataTimeSpan::end()
{
    return d->m_end;
}

void GeoDataTimeSpan::setEnd(const GeoDataTimeStamp& end)
{
    d->m_end = end;
}

bool GeoDataTimeSpan::isValid() const
{
    if (d->m_begin.when().isValid() != d->m_end.when().isValid())
    {
        return true;
    }

    return d->m_begin.when().isValid() && d->m_end.when().isValid() && d->m_begin.when() <= d->m_end.when();
}

const GeoDataTimeStamp& GeoDataTimeSpan::begin() const
{
    return d->m_begin;
}

GeoDataTimeStamp& GeoDataTimeSpan::begin()
{
    return d->m_begin;
}

void GeoDataTimeSpan::setBegin(const GeoDataTimeStamp& begin)
{
    d->m_begin = begin;
}

GeoDataTimeSpan& GeoDataTimeSpan::operator=(const GeoDataTimeSpan& other)
{
    GeoDataTimePrimitive::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataTimeSpan::operator==(const GeoDataTimeSpan& other) const
{
    return equals(other) &&
           d->m_begin == other.d->m_begin &&
           d->m_end == other.d->m_end;
}

bool GeoDataTimeSpan::operator!=(const GeoDataTimeSpan& other) const
{
    return !this->operator==(other);
}

void GeoDataTimeSpan::pack(QDataStream& stream) const
{
    GeoDataTimePrimitive::pack(stream);
    d->m_begin.pack(stream);
    d->m_end.pack(stream);
}

void GeoDataTimeSpan::unpack(QDataStream& stream)
{
    GeoDataTimePrimitive::unpack(stream);
    d->m_begin.unpack(stream);
    d->m_end.unpack(stream);
}

} // namespace Marble
