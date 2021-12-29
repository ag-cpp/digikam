/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect thunderbird binary program
 *
 * Copyright (C) 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "thunderbirdbinary.h"

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericSendByMailPlugin
{

ThunderbirdBinary::ThunderbirdBinary(QObject* const)
   : DBinaryIface(
                  QLatin1String("thunderbird"),
                  QLatin1String("Thunderbird"),
                  QLatin1String("https://www.mozilla.org/en-US/thunderbird/"),
                  QLatin1String("SendByMail"),
                  QStringList(QLatin1String("-v")),
                  i18n("Mozilla Foundation Mail Client.")
                 )
{
    setup();
}

ThunderbirdBinary::~ThunderbirdBinary()
{
}

} // namespace DigikamGenericSendByMailPlugin
