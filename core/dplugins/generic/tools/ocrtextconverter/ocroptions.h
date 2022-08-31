/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract options
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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

#ifndef TESSERACT_OCR_OPTIONS_H
#define TESSERACT_OCR_OPTIONS_H

// Qt includes

#include <QMap>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT OcrOptions
{

public:

    enum class Languages
    {
        DEFAULT = 0,
        ENG,
        OSD
    };

    enum class PageSegmentationModes
    {
        OSD_ONLY           = 0,
        AUTO_WITH_OSD      ,
        AUTO_WITH_NO_OSD   ,
        DEFAULT,
        SINGLE_COL_TEXT_OF_VAR_SIZE,
        SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT,
        SINGLE_UNIFORM_BLOCK_TEXT,
        SINGLE_TEXT_LINE,
        SINGLE_WORD,
        SINGLE_WORD_IN_CIRCLE,
        SINGLE_CHARACTER,
        SPARSE_TEXT,
        SPARSE_WITH_OSD,
        RAW_LINE
    };

    enum class EngineModes
    {
        LEGACY_ENGINE_ONLY = 0,
        NEURAL_NETS_LSTM_ONLY,
        LEGACY_LSTM_ENGINES,
        DEFAULT
    };

public:

    explicit OcrOptions();
    ~OcrOptions();

    static QMap<OcrOptions::Languages, QString>               languagesNames();
    static QMap<OcrOptions::PageSegmentationModes, QString>   psmNames();
    static QMap<OcrOptions::EngineModes, QString>             oemNames();

    QString LanguageCodeToValue(OcrOptions::Languages l);
    QString PsmCodeToValue(OcrOptions::PageSegmentationModes psm);
    QString OemCodeToValue(OcrOptions::EngineModes oem);
};

} // namespace Digikam

#endif // TESSERACT_OCR_OPTIONS_H

