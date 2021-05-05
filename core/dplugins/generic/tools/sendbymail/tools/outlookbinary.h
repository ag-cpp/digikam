/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-06
 * Description : Autodetect outlook binary program
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_OUTLOOK_BINARY_H
#define DIGIKAM_OUTLOOK_BINARY_H

// Local includes

#include "dbinaryiface.h"

namespace Digikam
{

class OutlookBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit OutlookBinary(QObject* const parent = nullptr);
    ~OutlookBinary() override;
};

} // namespace Digikam

#endif // DIGIKAM_OUTLOOK_BINARY_H
