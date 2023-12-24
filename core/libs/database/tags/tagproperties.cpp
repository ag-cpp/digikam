/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-05
 * Description : Access to the properties of a tag in the database
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tagproperties.h"

// Qt includes

#include <QSharedData>

// Local includes

#include "digikam_debug.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "tagscache.h"

namespace Digikam
{

typedef QExplicitlySharedDataPointer<TagProperties::TagPropertiesPriv> TagPropertiesPrivSharedPointer;

class Q_DECL_HIDDEN TagProperties::TagPropertiesPriv : public QSharedData
{
public:

    explicit TagPropertiesPriv()
      : tagId(-1)
    {
    }

    static TagPropertiesPrivSharedPointer createGuarded(int tagId);

    bool isNull() const;

public:

    int                         tagId;
    QMultiMap<QString, QString> properties;
};

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN TagPropertiesPrivSharedNull : public TagPropertiesPrivSharedPointer
{
public:

    TagPropertiesPrivSharedNull()
        : TagPropertiesPrivSharedPointer(new TagProperties::TagPropertiesPriv)
    {
    }
};

Q_GLOBAL_STATIC(TagPropertiesPrivSharedNull, tagPropertiesPrivSharedNull)

TagPropertiesPrivSharedPointer TagProperties::TagPropertiesPriv::createGuarded(int tagId)
{
    if (tagId <= 0)
    {
        qCDebug(DIGIKAM_DATABASE_LOG) << "Attempt to create tag properties for tag id" << tagId;

        return *tagPropertiesPrivSharedNull;
    }

    return TagPropertiesPrivSharedPointer(new TagPropertiesPriv);
}

bool TagProperties::TagPropertiesPriv::isNull() const
{
    return (this == tagPropertiesPrivSharedNull->constData());
}

// ------------------------------------------------------------------------------------------------

TagProperties::TagProperties()
    : d(*tagPropertiesPrivSharedNull)
{
}

TagProperties::TagProperties(int tagId)
    : d(TagPropertiesPriv::createGuarded(tagId))
{
    if (d->isNull())
    {
        return;
    }

    d->tagId                      = tagId;
    QList<TagProperty> properties = CoreDbAccess().db()->getTagProperties(tagId);

    Q_FOREACH (const TagProperty& p, properties)
    {
        d->properties.insert(p.property, p.value);
    }
}

TagProperties::~TagProperties()
{
}

TagProperties::TagProperties(const TagProperties& other)
    : d(other.d)
{
}

TagProperties& TagProperties::operator=(const TagProperties& other)
{
    d = other.d;

    return *this;
}

bool TagProperties::isNull() const
{
    return (d == *tagPropertiesPrivSharedNull);
}

TagProperties TagProperties::getOrCreate(const QString& tagPath)
{
    int tagId = TagsCache::instance()->getOrCreateTag(tagPath);

    return TagProperties(tagId);
}

int TagProperties::tagId() const
{
    return d->tagId;
}

bool TagProperties::hasProperty(const QString& key) const
{
    return d->properties.contains(key);
}

bool TagProperties::hasProperty(const QString& key, const QString& value) const
{
    return d->properties.contains(key, value);
}

QString TagProperties::value(const QString& key) const
{
    return d->properties.value(key);
}

QStringList TagProperties::propertyKeys() const
{
    return d->properties.keys();
}

QMultiMap<QString, QString> TagProperties::properties() const
{
    return d->properties;
}

void TagProperties::setProperty(const QString& key, const QString& value)
{
    if (d->isNull())
    {
        return;
    }
    if (d->properties.contains(key, value) && d->properties.count(key) == 1)
    {
        return;
    }

    // for single entries in db, this can of course be optimized using a single UPDATE WHERE

    removeProperties(key);
    d->properties.insert(key, value);
    CoreDbAccess().db()->addTagProperty(d->tagId, key, value);
}

void TagProperties::addProperty(const QString& key, const QString& value)
{
    if (d->isNull() || d->properties.contains(key, value))
    {
        return;
    }

    d->properties.insert(key, value);
    CoreDbAccess().db()->addTagProperty(d->tagId, key, value);
}

void TagProperties::removeProperty(const QString& key, const QString& value)
{
    if (!d->isNull() && d->properties.contains(key, value))
    {
        CoreDbAccess().db()->removeTagProperties(d->tagId, key, value);
        d->properties.remove(key, value);
    }
}

void TagProperties::removeProperties(const QString& key)
{
    if (!d->isNull() && d->properties.contains(key))
    {
        CoreDbAccess().db()->removeTagProperties(d->tagId, key);
        d->properties.remove(key);
    }
}

} // namespace Digikam
