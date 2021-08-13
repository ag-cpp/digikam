/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-16
 * Description : Database interface for face recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2021 by Nghia Duong    <minhnghiaduong997 at gmail dot com>
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


#ifndef DIGIKAM_DATABASE_INTERFACE_H
#define DIGIKAM_DATABASE_INTERFACE_H

#include "actionthreadbase.h"
#include "iteminfo.h"

#include <QVector>

namespace Digikam
{

class FaceEmbeddingData;

class Q_DECL_HIDDEN DatabaseInterface: public ActionJob
{
    Q_OBJECT
public: 
    DatabaseInterface(QObject* parent = nullptr);
    ~DatabaseInterface();

    QVector<FaceEmbeddingData> faceEmbeddings() const;

private:

    class Private;
    Private* d;
};

} // namespace Digikam

#endif // DIGIKAM_DATABASE_INTERFACE_H