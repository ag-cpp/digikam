/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-04-03
 * Description : AVIF image Converter batch tool.
 *
 * SPDX-FileCopyrightText: 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_CONVERT_TO_AVIF_H
#define DIGIKAM_BQM_CONVERT_TO_AVIF_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmConvertToAvifPlugin
{

class ConvertToAVIF : public BatchTool
{
    Q_OBJECT

public:

    explicit ConvertToAVIF(QObject* const parent = nullptr);
    ~ConvertToAVIF()                                        override = default;

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

    bool m_changeSettings = true;
};

} // namespace DigikamBqmConvertToAvifPlugin

#endif // DIGIKAM_BQM_CONVERT_TO_AVIF_H
