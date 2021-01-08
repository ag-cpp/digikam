/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-22
 * Description : a widget to filter album contents by type mime
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_MIME_FILTER_H
#define DIGIKAM_MIME_FILTER_H

// Qt includes

#include <QComboBox>

namespace Digikam
{

class MimeFilter : public QComboBox
{
    Q_OBJECT

public:

    enum TypeMimeFilter
    {
        AllFiles = 0,
        ImageFiles,
        NoRAWFiles,
        JPGFiles,
        PNGFiles,
        TIFFiles,
        PGFFiles,
        HEIFFiles,           // HEVC H265 compression based containers.
        DNGFiles,
        RAWFiles,            // All Raw file formats such as nef, cr2, arw, pef, etc..
        MoviesFiles,
        AudioFiles,
        RasterGraphics       // PSD, XCF, etc...
    };

public:

    explicit MimeFilter(QWidget* const parent);
    ~MimeFilter() override;

    void setMimeFilter(int filter);
    int mimeFilter();
};

} // namespace Digikam

#endif // DIGIKAM_MIME_FILTER_H
