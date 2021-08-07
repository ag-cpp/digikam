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

#ifndef FACEDB_WRAPPER_P_H
#define FACEDB_WRAPPER_P_H

#include "facedb_wrapper.h"

#include <QMutex>

namespace Digikam
{

class Q_DECL_HIDDEN FaceDbWrapper::Private
{
public:

    explicit Private();
    ~Private();

public:
    // --- Identity management (facesengine_interface_identity.cpp) -----------------------------------------

    static bool identityContains(const Identity& identity,
                                 const QString& attribute,
                                 const QString& value);

    Identity findByAttribute(const QString& attribute,
                             const QString& value)                         const;

    Identity findByAttributes(const QString& attribute,
                              const QMultiMap<QString, QString>& valueMap) const;
public:

    bool                        dbAvailable;
    int                         ref;
    mutable QMutex              mutex;
    QHash<int, Identity>        identityCache;
};

} // namespace Digikam

#endif // FACEDB_WRAPPER_P_H