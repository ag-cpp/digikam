/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-27
 * Description : Interface for info stored about a face tag in the database
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_FACE_TAGS_IFACE_H
#define DIGIKAM_FACE_TAGS_IFACE_H

// Qt includes

#include <QFlags>
#include <QVariant>

// Local includes

#include "tagregion.h"
#include "digikam_export.h"

class QDebug;

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT FaceTagsIface
{
public:

    enum Type
    {
        InvalidFace      = 0,
        UnknownName      = 1 << 0,
        UnconfirmedName  = 1 << 1,
        IgnoredName      = 1 << 2,
        ConfirmedName    = 1 << 3,
        FaceForTraining  = 1 << 4,

        UnconfirmedTypes = UnknownName | UnconfirmedName,
        NormalFaces      = UnknownName | UnconfirmedName | IgnoredName | ConfirmedName,
        AllTypes         = UnknownName | UnconfirmedName | IgnoredName | ConfirmedName | FaceForTraining,
        TypeFirst        = UnknownName,
        TypeLast         = FaceForTraining
    };
    Q_DECLARE_FLAGS(TypeFlags, Type)

public:

    FaceTagsIface();
    FaceTagsIface(Type type, qlonglong imageId, int tagId, const TagRegion& region);
    FaceTagsIface(const QString& attribute, qlonglong imageId, int tagId, const TagRegion& region);

    bool      isNull()                          const;

    Type      type()                            const;
    qlonglong imageId()                         const;
    int       tagId()                           const;
    TagRegion region()                          const;

    bool      isInvalidFace()                   const
    {
        return (type() == InvalidFace);
    }

    bool      isUnknownName()                   const
    {
        return (type() == UnknownName);
    }

    bool      isUnconfirmedName()               const
    {
        return (type() == UnconfirmedName);
    }

    bool      isUnconfirmedType()               const
    {
        return (type() & UnconfirmedTypes);
    }

    bool      isIgnoredName()                   const
    {
        return (type() == IgnoredName);
    }

    bool      isConfirmedName()                 const
    {
        return (type() == ConfirmedName);
    }

    bool      isForTraining()                   const
    {
        return (type() == FaceForTraining);
    }

    void setType(Type type);
    void setTagId(int tagId);
    void setRegion(const TagRegion& region);

    bool operator==(const FaceTagsIface& other) const;

    /**
     * Returns a list of all image tag properties for which flags are set
     */
    static QStringList attributesForFlags(TypeFlags flags);

    /**
     * Return the corresponding image tag property for the given type
     */
    static QString attributeForType(Type type);

    /**
     * Returns the Face Type corresponding to
     * the given TagId.
     */
    static Type typeForId(int tagId);

    /**
     * Return the Type for the given attribute. To distinguish between UnknownName
     * and UnconfirmedName, the tagId must be given.
     */
    static Type typeForAttribute(const QString& attribute, int tagId = 0);

    /**
     * Returns the string tagId + ',' + unconfirmedFace + ',' + regionXml
     */
    QString getAutodetectedPersonString()       const;

    /**
     * Writes the contents of this face - in a compact way - in the QVariant.
     * Only native QVariant types are used, that is, the QVariant will not have a custom type,
     * thus it can be compared by value by operator==.
     */
    static FaceTagsIface fromVariant(const QVariant& var);
    QVariant toVariant()                        const;

    /**
     * Create a FaceTagsIface from the extraValues returned from ItemLister.
     */
    static FaceTagsIface fromListing(qlonglong imageid, const QList<QVariant>& values);

protected:

    Type      m_type;
    qlonglong m_imageId;
    int       m_tagId;
    TagRegion m_region;
};

DIGIKAM_DATABASE_EXPORT QDebug operator<<(QDebug dbg, const FaceTagsIface& f);

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FaceTagsIface::TypeFlags)

#endif // DIGIKAM_FACE_TAGS_IFACE_H
