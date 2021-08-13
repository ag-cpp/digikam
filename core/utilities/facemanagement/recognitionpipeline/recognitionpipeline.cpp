/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-13
 * Description : Async face recognition pipeline
 *
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

#include "recognitionpipeline.h"

#include "faceutils.h"

namespace Digikam
{
class Q_DECL_PRIVATE RecognitionPipeline::Private
{
public:
    explicit Private(FilterMode mode)
        : mode(mode)
    {
    }

    ~Private()
    {
    }

public: 
    FilterMode mode;
}

QList<FaceTagsIface> RecognitionPipeline::getFaces(const ItemInfo& info) const
{
    FaceUtils utils;
    QList<FaceTagsIface> databaseFaces;

    if      (d->mode == ReadUnconfirmedFaces)
    {
        databaseFaces = utils.unconfirmedFaceTagsIfaces(info.id());
    }
    else if (d->mode == ReadFacesForTraining)
    {
        databaseFaces = utils.databaseFacesForTraining(info.id());
    }
    else
    {
        databaseFaces = utils.confirmedFaceTagsIfaces(info.id());
    }

    return databaseFaces;
}

}