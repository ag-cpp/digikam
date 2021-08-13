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

#ifndef DIGIKAM_RECOGNITION_PIPELINE_H
#define DIGIKAM_RECOGNITION_PIPELINE_H

#include "iteminfo.h"
#include "actionthreadbase.h"

namespace Digikam
{

class RecognitionPipeline: public ActionThreadBase
{
    Q_OBJECT
public:

    enum FilterMode
    {
        ReadUnconfirmedFaces = 0,
        ReadFacesForTraining,
        ReadConfirmedFaces
    };

public:
    explicit RecognitionPipeline(QObject* parent);
    ~RecognitionPipeline();

private:

    QList<FaceTagsIface> getFaces(const ItemInfo& info) const;

private:

    class Private;
    Private* d;

};

}

#endif // DIGIKAM_RECOGNITION_PIPELINE_H