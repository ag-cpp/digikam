/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-08
 * Description : Async face detection pipeline
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

#ifndef DIGIKAM_DETECTION_PIPELINE_H
#define DIGIKAM_DETECTION_PIPELINE_H

#include "iteminfo.h"

namespace Digikam
{

class DetectionPipeline: public QObject
{
    Q_OBJECT
public:

    explicit DetectionPipeline(bool scanAll, bool overWrite);
    ~DetectionPipeline();

public:

    void process(const QList<ItemInfo>& info);
    void cancel();

Q_SIGNALS:

    void processed();

private:

    class Private;
    Private* d;
};

}

#endif // DIGIKAM_DETECTION_PIPELINE_H
