/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-07-18
 * Description : DNG Raw Converter batch tool.
 *
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "converttodng.h"

// Qt includes

#include <QFileInfo>
#include <QWidget>

// KDE includes

#include <klocalizedstring.h>

namespace DigikamBqmConvertToDngPlugin
{

ConvertToDNG::ConvertToDNG(QObject* const parent)
    : BatchTool(QLatin1String("ConvertToDNG"), ConvertTool, parent)
{
    m_changeSettings = true;
}

ConvertToDNG::~ConvertToDNG()
{
}

void ConvertToDNG::registerSettingsWidget()
{
    DNGSettings* const DNGBox = new DNGSettings;

    connect(DNGBox, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    m_settingsWidget = DNGBox;

    BatchTool::registerSettingsWidget();
}

BatchToolSettings ConvertToDNG::defaultSettings()
{
    BatchToolSettings settings;
    settings.insert(QLatin1String("CompressLossLess"),      true);
    settings.insert(QLatin1String("PreviewMode"),           DNGWriter::MEDIUM);
    settings.insert(QLatin1String("BackupOriginalRawFile"), false);

    return settings;
}

void ConvertToDNG::slotAssignSettings2Widget()
{
    m_changeSettings          = false;
    DNGSettings* const DNGBox = dynamic_cast<DNGSettings*>(m_settingsWidget);

    if (DNGBox)
    {
        DNGBox->setCompressLossLess(settings()[QLatin1String("CompressLossLess")].toBool());
        DNGBox->setPreviewMode(settings()[QLatin1String("PreviewMode")].toInt());
        DNGBox->setBackupOriginalRawFile(settings()[QLatin1String("BackupOriginalRawFile")].toBool());
    }

    m_changeSettings          = true;
}

void ConvertToDNG::slotSettingsChanged()
{
    if (m_changeSettings)
    {
        BatchToolSettings settings;
        DNGSettings* const DNGBox = dynamic_cast<DNGSettings*>(m_settingsWidget);

        if (DNGBox)
        {
            settings.insert(QLatin1String("CompressLossLess"),      DNGBox->compressLossLess());
            settings.insert(QLatin1String("PreviewMode"),           DNGBox->previewMode());
            settings.insert(QLatin1String("BackupOriginalRawFile"), DNGBox->backupOriginalRawFile());
            BatchTool::slotSettingsChanged(settings);
        }
    }
}

QString ConvertToDNG::outputSuffix() const
{
    return QLatin1String("dng");
}

void ConvertToDNG::cancel()
{
    m_dngProcessor.cancel();
    BatchTool::cancel();
}

bool ConvertToDNG::toolOperations()
{
    if (!isRawFile(inputUrl()))
        return false;

    m_dngProcessor.reset();
    m_dngProcessor.setInputFile(inputUrl().toLocalFile());
    m_dngProcessor.setOutputFile(outputUrl().toLocalFile());
    m_dngProcessor.setBackupOriginalRawFile(settings()[QLatin1String("BackupOriginalRawFile")].toBool());
    m_dngProcessor.setCompressLossLess(settings()[QLatin1String("CompressLossLess")].toBool());
    m_dngProcessor.setPreviewMode(settings()[QLatin1String("PreviewMode")].toInt());

    int ret = m_dngProcessor.convert();

    return (ret == DNGWriter::PROCESSCOMPLETE);
}

} // namespace DigikamBqmConvertToDngPlugin
