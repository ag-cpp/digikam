/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-04-08
 * Description : a command line tool to test DImg equalize filter
 *
 * Copyright (C) 2016-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include <QFileInfo>

// Local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "equalizefilter.h"
#include "dimg.h"
#include "dimgthreadedfilter.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "testequalizefilter - test DImg equalize algorithm";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();

    QFileInfo input(QString::fromUtf8(argv[1]));
    QString   outFilePath(input.baseName() + QLatin1String(".out.png"));

    DRawDecoderSettings settings;
    settings.halfSizeColorImage    = false;
    settings.sixteenBitsImage      = false;
    settings.RGBInterpolate4Colors = false;
    settings.RAWQuality            = DRawDecoderSettings::BILINEAR;

    DImg img(input.filePath(), nullptr, DRawDecoding(settings));
    EqualizeFilter filter(&img, &img);
    filter.startFilterDirectly();
    img.putImageData(filter.getTargetImage().bits());
    img.save(outFilePath, QLatin1String("PNG"));

    return 0;
}
