/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-30-08
 * Description : batch thumbnails generator
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_THUMBS_GENERATOR_H
#define DIGIKAM_THUMBS_GENERATOR_H

// Qt includes

#include <QObject>
#include <QImage>

// Local includes

#include "album.h"
#include "maintenancetool.h"

namespace Digikam
{

class ThumbsGenerator : public MaintenanceTool
{
    Q_OBJECT

public:

    /** Constructor using Album Id as argument. If Id = -1, whole Albums collection is processed.
     */
    explicit ThumbsGenerator(const bool rebuildAll, int albumId, ProgressItem* const parent = nullptr);

    /** Constructor using AlbumList as argument. If list is empty, whole Albums collection is processed.
     */
    ThumbsGenerator(const bool rebuildAll, const AlbumList& list, ProgressItem* const parent = nullptr);
    ~ThumbsGenerator() override;

    void setUseMultiCoreCPU(bool b) override;

private:

    void init(const bool rebuildAll);

private Q_SLOTS:

    void slotStart()                override;
    void slotCancel()               override;
    void slotAdvance(const QImage&);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_THUMBS_GENERATOR_H
