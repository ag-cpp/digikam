/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-13
 * Description : ExifTool thread for the ExifTool process
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2022      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "exiftoolthread.h"

// Local includes

#include "digikam_debug.h"
#include "exiftoolprocess.h"

namespace Digikam
{

ExifToolThread::ExifToolThread(QObject* const parent)
    : QThread(parent)
{
}

ExifToolThread::~ExifToolThread()
{
    quit();
    wait();
}

void ExifToolThread::run()
{
    ExifToolProcess* const proc = ExifToolProcess::instance();
    proc->moveToThread(this);
    proc->initExifTool();

    exec();

    if (ExifToolProcess::isCreated())
    {
        delete ExifToolProcess::internalPtr;
    }
}

} // namespace Digikam
