/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 
 * Description : Image Quality Parser - noise detection
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * References  : 
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

#ifndef DIGIKAM_NOISE_DETECTOR_H
#define DIGIKAM_NOISE_DETECTOR_H

// Local includes

#include "dimg.h"
#include "digikam_opencv.h"

namespace Digikam
{

class NoiseDetector 
{
public:

    explicit NoiseDetector(const DImg& image);
    ~NoiseDetector();

    float detect();

private:

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_NOISE_DETECTOR_H