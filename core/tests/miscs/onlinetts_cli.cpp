/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : an unit test to check text online-translators.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QCoreApplication>
#include <QTest>
#include <QList>
#include <QObject>
#include <QUrl>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"

using namespace Digikam;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    DOnlineTts tts;
    tts.generateUrls(QLatin1String("Hello World!"), DOnlineTranslator::Google, DOnlineTranslator::English);
    QList<QUrl> urls = tts.media();

    qCDebug(DIGIKAM_TESTS_LOG) << urls;

    return 0;
}
