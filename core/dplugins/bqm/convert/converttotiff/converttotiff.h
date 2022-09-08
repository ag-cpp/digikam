/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-28
 * Description : TIFF image Converter batch tool.
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_CONVERT_TO_TIFF_H
#define DIGIKAM_BQM_CONVERT_TO_TIFF_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmConvertToTiffPlugin
{

class ConvertToTIFF : public BatchTool
{
    Q_OBJECT

public:

    explicit ConvertToTIFF(QObject* const parent = nullptr);
    ~ConvertToTIFF()                                        override;

    QString outputSuffix()                            const override;
    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotSettingsChanged()                              override;
    void slotAssignSettings2Widget()                        override;

private:

    bool toolOperations()                                   override;

private:

    bool m_changeSettings;
};

} // namespace DigikamBqmConvertToTiffPlugin

#endif // DIGIKAM_BQM_CONVERT_TO_TIFF_H
