/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to get ExifTool version string
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QString>
#include <QCoreApplication>
#include <QVariant>

// Local includes

#include "digikam_debug.h"
#include "exiftoolparser.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // Create ExifTool parser instance.

    ExifToolParser* const parser = new ExifToolParser(qApp);
    ExifToolParser::ExifToolData parsed;

    if (parser->version())
    {
        parsed      = parser->currentData();
        QString ver = parsed.find(QLatin1String("VERSION_STRING")).value()[0].toString();
        qCDebug(DIGIKAM_TESTS_LOG) << "ExifTool version:" << ver;
    }

    return 0;
}
