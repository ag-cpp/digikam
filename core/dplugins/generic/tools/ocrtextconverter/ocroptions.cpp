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

#include "ocroptions.h"

namespace Digikam
{

OcrOptions::OcrOptions()
{
}

OcrOptions::~OcrOptions()
{
}

QMap<OcrOptions::Languages, QString> OcrOptions::languagesNames()
{
    QMap<OcrOptions::Languages, QString> languages;

    languages[OcrOptions::Languages::DEFAULT]                                         = QString::fromLatin1("Default");
    languages[OcrOptions::Languages::ENG]                                             = QString::fromLatin1("english");
    languages[OcrOptions::Languages::OSD]                                             = QString::fromLatin1("osd");

    return languages;
}


QMap<OcrOptions::PageSegmentationModes, QString> OcrOptions::psmNames()
{
    QMap<PageSegmentationModes, QString> psm;

    psm[OcrOptions::PageSegmentationModes::OSD_ONLY]                                  = QLatin1String("OSD only.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_OSD]                             = QLatin1String("Automatic with OSD.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_NO_OSD]                          = QLatin1String("Automatic no OSD.");
    psm[OcrOptions::PageSegmentationModes::DEFAULT]                                   = QLatin1String("Default.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_COL_TEXT_OF_VAR_SIZE]               = QLatin1String("Column of text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT]   = QLatin1String("Block of vertically aligned text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_TEXT]                 = QLatin1String("Block of text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_TEXT_LINE]                          = QLatin1String("Single text line.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD]                               = QLatin1String("Single word.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD_IN_CIRCLE]                     = QLatin1String("Single word in a circle.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_CHARACTER]                          = QLatin1String("Single character.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_TEXT]                               = QLatin1String("Sparse text.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_WITH_OSD]                           = QLatin1String("Sparse text with OSD.");
    psm[OcrOptions::PageSegmentationModes::RAW_LINE]                                  = QLatin1String("Raw line.");

    return psm;
}



QMap<OcrOptions::EngineModes, QString> OcrOptions::oemNames()
{
    QMap<EngineModes, QString> oem;

    oem[OcrOptions::EngineModes::LEGACY_ENGINE_ONLY]                                 = QLatin1String("Legacy engine only.");
    oem[OcrOptions::EngineModes::NEURAL_NETS_LSTM_ONLY]                              = QLatin1String("LSTM engine only.");
    oem[OcrOptions::EngineModes::LEGACY_LSTM_ENGINES]                                = QLatin1String("Legacy + LSTM .");
    oem[OcrOptions::EngineModes::DEFAULT]                                            = QLatin1String("Default.");

    return oem;
}


QString OcrOptions::LanguageCodeToValue(OcrOptions::Languages l)
{
    switch (l)
    {
        case OcrOptions::Languages::ENG:
        {
            return QLatin1String("eng");
        }

        case OcrOptions::Languages::OSD:
        {
            return QLatin1String("osd");
        }

        case OcrOptions::Languages::DEFAULT:
        {
            return QLatin1String("");
        }

        default:
        {
            return QLatin1String("");
        }
    }
}


QString OcrOptions::PsmCodeToValue(OcrOptions::PageSegmentationModes psm)
{
   return QString::fromLatin1("%1").arg((int)psm);  // psm tesseract cli values if range from 0 to 13
}


QString OcrOptions::OemCodeToValue(OcrOptions::EngineModes oem)
{
   return QString::fromLatin1("%1").arg((int)oem); // oem tesseract cli values if range from 0 to 3
}

} //  namespace Digikam