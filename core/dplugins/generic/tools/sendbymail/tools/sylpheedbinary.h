/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect sylpheed binary program
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SYLPHEED_BINARY_H
#define DIGIKAM_SYLPHEED_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericSendByMailPlugin
{

class SylpheedBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit SylpheedBinary(QObject* const parent = nullptr);
    ~SylpheedBinary() override;
};

} // namespace DigikamGenericSendByMailPlugin

#endif // DIGIKAM_SYLPHEED_BINARY_H
