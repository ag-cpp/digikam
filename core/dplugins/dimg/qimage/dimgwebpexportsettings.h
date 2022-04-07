/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-04-04
 * Description : WEBP image export settings widget.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIMG_WEBP_EXPORT_SETTINGS_H
#define DIGIKAM_DIMG_WEBP_EXPORT_SETTINGS_H

// Local includes

#include "dimgloadersettings.h"

namespace Digikam
{

class DImgWEBPExportSettings : public DImgLoaderSettings
{
    Q_OBJECT

public:

    explicit DImgWEBPExportSettings(QWidget* const parent = nullptr);
    ~DImgWEBPExportSettings()                   override;

    /**
     * This widget manage 2 parameters for the image encoding:
     * "quality"  as integer [1 - 99].
     * "lossless" as boolean.
     */
    void setSettings(const DImgLoaderPrms& set) override;
    DImgLoaderPrms settings() const             override;

private Q_SLOTS:

    void slotToggleWEBPLossLess(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_WEBP_EXPORT_SETTINGS_H
