/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : Test ImageMagick loader to QImage.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QString>
#include <QDebug>
#include <QApplication>

// ImageMagick includes

#include <Magick++.h>

using namespace Magick;
using namespace MagickCore;

int main(int argc, char** argv)
{
    MagickCoreGenesis((char*)nullptr, MagickFalse);

    const int msize                   = 20;
    MagickCore::ImageInfo* image_info = nullptr;
    ExceptionInfo ex                  = *AcquireExceptionInfo();

    QApplication app(argc, argv);

    try
    {
        char** metadata     = new char*[msize];

        for (int i = 0 ; i < msize; ++i)
        {
            metadata[i]     = new char[256];
            metadata[i][0]  = '\0';
        }

        image_info          = CloneImageInfo((ImageInfo*)nullptr);
        strcpy(image_info->filename, argv[1]);
        image_info->ping    = MagickTrue;
        image_info->verbose = MagickTrue;
        image_info->debug   = MagickTrue;

        int identargc       = argc + 2;

        char** identargv    = new char*[identargc];
        identargv[0] = argv[0];
        identargv[1] = (char*)"-format";
        identargv[2] = (char*)"%[*:*]";
        identargv[3] = argv[1];

        IdentifyImageCommand(image_info, identargc, identargv, metadata, &ex);

        for (int i = 0 ; i < msize; ++i)
        {
            if (metadata[i][0] != '\0')
            {
                printf("%s\n", metadata[i]);
            }
        }

        DestroyImageInfo(image_info);

        for (int i = 0 ; i < msize; ++i)
        {
            delete metadata[i];
        }

        delete [] metadata;
        delete [] identargv;
    }
    catch (Exception& error_)
    {
        qWarning() << "Cannot read [" << argv[1] << "] due to ImageMagick exception:" << error_.what();
    }

    MagickCoreTerminus();

    return 0;
}
