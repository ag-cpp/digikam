/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-05-16
 * Description : Autodetects pano_modify binary program and version
 *
 * Copyright (C) 2013-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef DIGIKAM_PANO_MODIFY_BINARY_H
#define DIGIKAM_PANO_MODIFY_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class PanoModifyBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit PanoModifyBinary(QObject* const parent = nullptr);
    ~PanoModifyBinary() override;
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_PANO_MODIFY_BINARY_H
