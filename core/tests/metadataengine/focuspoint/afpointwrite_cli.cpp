/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to write metadata with ExifTool
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>
#include <QCoreApplication>
#include <QVariant>

// Local includes

#include "dimg.h"
#include "digikam_debug.h"
#include "focuspoints_writer.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "afpointwrite - CLI tool to write metadata with ExifTool in image";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image to patch>";
        return -1;
    }

    FocusPointsWriter* writer = new FocusPointsWriter(qApp,QString::fromUtf8(argv[1]));

    FocusPoint point = {0.5, 0.5, 0.5, 0.5};

    writer->writeFocusPoint(point);

    delete writer;

    return 0;
}
