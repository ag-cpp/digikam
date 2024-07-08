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

#include "GeoDataData.h"
#include "GeoDataData_p.h"

// Qt includes

#include <QDataStream>

// Local includes

#include "GeoDataTypes.h"

namespace Marble
{

GeoDataData::GeoDataData()
    : GeoDataObject(), d(new GeoDataDataPrivate)
{
}

GeoDataData::GeoDataData(const GeoDataData& other)
    : GeoDataObject(other), d(new GeoDataDataPrivate(*other.d))
{
}

GeoDataData::~GeoDataData()
{
    delete d;
}

GeoDataData& GeoDataData::operator=(const GeoDataData& other)
{
    GeoDataObject::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataData::operator==(const GeoDataData& other) const
{
    return equals(other) &&
           d->m_name == other.d->m_name &&
           d->m_value == other.d->m_value &&
           d->m_displayName == other.d->m_displayName;
}

bool GeoDataData::operator!=(const GeoDataData& other) const
{
    return !this->operator==(other);
}

GeoDataData::GeoDataData(const QString& name, const QVariant& value)
    : d(new GeoDataDataPrivate)
{
    d->m_name = name;
    d->m_value = value;
}

const char* GeoDataData::nodeType() const
{
    return GeoDataTypes::GeoDataDataType;
}

QVariant GeoDataData::value() const
{
    return d->m_value;
}

QVariant& GeoDataData::valueRef()
{
    return d->m_value;
}

const QVariant& GeoDataData::valueRef() const
{
    return d->m_value;
}

void GeoDataData::setValue(const QVariant& value)
{
    d->m_value = value;
}

QString GeoDataData::name() const
{
    return d->m_name;
}

void GeoDataData::setName(const QString& name)
{
    d->m_name = name;
}

QString GeoDataData::displayName() const
{
    return d->m_displayName;
}

void GeoDataData::setDisplayName(const QString& displayName)
{
    d->m_displayName = displayName;
}

void GeoDataData::pack(QDataStream& stream) const
{
    GeoDataObject::pack(stream);

    stream << d->m_value;
    stream << d->m_displayName;
}

void GeoDataData::unpack(QDataStream& stream)
{
    GeoDataObject::unpack(stream);

    stream >> d->m_value;
    stream >> d->m_displayName;
}

} // namespace Marble
