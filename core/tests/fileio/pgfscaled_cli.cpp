/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : a command line tool to test PGF scaled to QImage
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QFile>
#include <QIODevice>
#include <QImage>

// Local includes

#include "digikam_debug.h"
#include "pgfutils.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "pgfscaled - Load scaled version of PGF image and save to PNG";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <pgffile>";
        return -1;
    }

    qCDebug(DIGIKAM_TESTS_LOG) << "Using LibPGF version: " << PGFUtils::libPGFVersion();
    QImage img;

    // Write PGF file.

    QString fname = QString::fromUtf8(argv[1]);
    QFile   file(fname);

    if (!file.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Cannot open PGF file to read...";
        return -1;
    }

    // PGF => QImage conversion

    if (!PGFUtils::loadPGFScaled(img, file.fileName(), 1280))
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "loadPGFScaled failed...";
        return -1;
    }

    img.save(file.fileName() + QString::fromUtf8("-scaled.png"), "PNG");

    return 0;
}
