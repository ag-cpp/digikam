/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : utilities for digiKam setup
 *
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Album
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Album Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_UTILS_H
#define DIGIKAM_SETUP_UTILS_H

// Qt includes

#include <QStringList>
#include <QString>

namespace Digikam
{

QStringList cleanUserFilterString(QString filterString,
                                  const bool caseSensitive = false,
                                  const bool useSemicolon  = false);

} // namespace Digikam

#endif // DIGIKAM_SETUP_UTILS_H
