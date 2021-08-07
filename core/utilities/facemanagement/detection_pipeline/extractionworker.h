/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-08
 * Description : Integrated, multithread face embedding extraction
 *
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

#ifndef DIGIKAM_EXTRACTION_WORKER_H
#define DIGIKAM_EXTRACTION_WORKER_H

// Local includes
#include "actionthreadbase.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExtractionWorker : public ActionJob
{
    Q_OBJECT

public:

    explicit ExtractionWorker();
    ~ExtractionWorker() override;

public Q_SLOTS:

    void process();

private:

    class Private;
    Private* d;

    // Disable
    ExtractionWorker(const ExtractionWorker&)            = delete;
    ExtractionWorker& operator=(const ExtractionWorker&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_EXTRACTION_WORKER_H
