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

#include "GeoDataUpdate.h"

// Qt includes

#include <QString>

// Local includes

#include "GeoDataCreate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "GeoDataTypes.h"

namespace Marble
{

class Q_DECL_HIDDEN GeoDataUpdatePrivate
{
public:

    GeoDataUpdatePrivate();

    GeoDataCreate* m_create = nullptr;
    GeoDataDelete* m_delete = nullptr;
    GeoDataChange* m_change = nullptr;
    QString        m_targetHref;
};

GeoDataUpdatePrivate::GeoDataUpdatePrivate()
    : m_create(nullptr),
      m_delete(nullptr),
      m_change(nullptr)
{
}

GeoDataUpdate::GeoDataUpdate() :
    d(new GeoDataUpdatePrivate)
{
}

GeoDataUpdate::GeoDataUpdate(const Marble::GeoDataUpdate& other)
    : GeoDataObject(),
      d(new GeoDataUpdatePrivate(*other.d))
{
}

GeoDataUpdate& GeoDataUpdate::operator=(const GeoDataUpdate& other)
{
    GeoDataObject::operator =(other);
    *d = *other.d;
    return *this;
}

bool GeoDataUpdate::operator==(const GeoDataUpdate& other) const
{
    bool const changeEmpty = !d->m_change || d->m_change->size() == 0;
    bool const otherChangeEmpty = !other.d->m_change || other.d->m_change->size() == 0;

    if (changeEmpty != otherChangeEmpty)
    {
        return false;
    }

    else if (d->m_change && other.d->m_change && *d->m_change != *other.d->m_change)
    {
        return false;
    }

    return d->m_targetHref == other.d->m_targetHref;
}

bool GeoDataUpdate::operator!=(const GeoDataUpdate& other) const
{
    return !this->operator==(other);
}

GeoDataUpdate::~GeoDataUpdate()
{
    delete d;
}

const char* GeoDataUpdate::nodeType() const
{
    return GeoDataTypes::GeoDataUpdateType;
}

QString GeoDataUpdate::targetHref() const
{
    return d->m_targetHref;
}

void GeoDataUpdate::setTargetHref(const QString& targetHref)
{
    d->m_targetHref = targetHref;
}

const GeoDataChange* GeoDataUpdate::change() const
{
    return d->m_change;
}

GeoDataChange* GeoDataUpdate::change()
{
    return d->m_change;
}

void GeoDataUpdate::setChange(GeoDataChange* change)
{
    delete d->m_change;
    d->m_change = change;

    if (d->m_change)
    {
        d->m_change->setParent(this);
    }
}

const GeoDataCreate* GeoDataUpdate::create() const
{
    return d->m_create;
}

GeoDataCreate* GeoDataUpdate::create()
{
    return d->m_create;
}

void GeoDataUpdate::setCreate(GeoDataCreate* create)
{
    delete d->m_create;
    d->m_create = create;

    if (d->m_create)
    {
        d->m_create->setParent(this);
    }
}

const GeoDataDelete* GeoDataUpdate::getDelete() const
{
    return d->m_delete;
}

GeoDataDelete* GeoDataUpdate::getDelete()
{
    return d->m_delete;
}

void GeoDataUpdate::setDelete(GeoDataDelete* dataDelete)
{
    delete d->m_delete;
    d->m_delete = dataDelete;

    if (d->m_delete)
    {
        d->m_delete->setParent(this);
    }
}

} // namespace Marble
