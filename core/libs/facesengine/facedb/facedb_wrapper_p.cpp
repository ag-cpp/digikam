/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2021-08-07
 * Description : The access point to Face database.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020-2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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

#include "facedb_wrapper_p.h"

#include "digikam_debug.h"
#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "facedbaccess.h"
#include "facedb.h"

namespace Digikam
{

FaceDbWrapper::Private::Private()
    : ref(1),
      mutex (QMutex::Recursive)
{
    DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
    FaceDbAccess::setParameters(params);

    dbAvailable = FaceDbAccess::checkReadyForUse(nullptr);
    if (dbAvailable)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Face database ready for use";

        foreach (const Identity& identity, FaceDbAccess().db()->identities())
        {
            identityCache[identity.id()] = identity;
        }
    }
    else
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Failed to initialize face database";
    }
}

FaceDbWrapper::Private::~Private()
{
}

/**
 * NOTE: Takes care that there may be multiple values of attribute in identity's attributes
 */
bool FaceDbWrapper::Private::identityContains(const Identity& identity,
                                              const QString&  attribute,
                                              const QString&  value)
{
    const QMultiMap<QString, QString> map     = identity.attributesMap();
    QMap<QString, QString>::const_iterator it = map.constFind(attribute);

    for ( ; (it != map.constEnd()) && (it.key() == attribute) ; ++it)
    {
        if (it.value() == value)
        {
            return true;
        }
    }

    return false;
}

Identity FaceDbWrapper::Private::findByAttribute(const QString& attribute,
                                                 const QString& value) const
{
    foreach (const Identity& identity, identityCache)
    {
        if (identityContains(identity, attribute, value))
        {
            return identity;
        }
    }

    return Identity();
}

/**
 * NOTE: Takes care that there may be multiple values of attribute in valueMap
 */
Identity FaceDbWrapper::Private::findByAttributes(const QString& attribute,
                                                  const QMultiMap<QString, QString>& valueMap) const
{
    QMap<QString, QString>::const_iterator it = valueMap.find(attribute);

    for ( ; (it != valueMap.end()) && (it.key() == attribute) ; ++it)
    {
        foreach (const Identity& identity, identityCache)
        {
            if (identityContains(identity, attribute, it.value()))
            {
                return identity;
            }
        }
    }

    return Identity();
}

} //namespace Digikam