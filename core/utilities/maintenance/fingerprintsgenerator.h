/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-16
 * Description : finger-prints generator
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FINGERPRINTS_GENERATOR_H
#define DIGIKAM_FINGERPRINTS_GENERATOR_H

// Qt includes

#include <QObject>

// Local includes

#include "album.h"
#include "maintenancetool.h"

class QImage;

namespace Digikam
{

class FingerPrintsGenerator : public MaintenanceTool
{
    Q_OBJECT

public:

    /** Constructor using AlbumList as argument. If list is empty, whole Albums collection is processed.
     */
    explicit FingerPrintsGenerator(const bool rebuildAll, const AlbumList& list=AlbumList(), ProgressItem* const parent = nullptr);
    ~FingerPrintsGenerator() override;

    void setUseMultiCoreCPU(bool b) override;

private:

    void processOne();

private Q_SLOTS:

    void slotStart()                override;
    void slotDone()                 override;
    void slotCancel()               override;
    void slotAdvance(const QImage&);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FINGERPRINTS_GENERATOR_H
