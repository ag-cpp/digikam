/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-03
 * Description : blur image batch tool.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_BLUR_H
#define DIGIKAM_BQM_BLUR_H

#include "batchtool.h"
#include "dnuminput.h"

using namespace Digikam;

namespace DigikamBqmBlurPlugin
{

class Blur : public BatchTool
{
    Q_OBJECT

public:

    explicit Blur(QObject* const parent = nullptr);
    ~Blur() override;

    BatchToolSettings defaultSettings() override;

    BatchTool* clone(QObject* const parent=nullptr) const override { return new Blur(parent); };

    void registerSettingsWidget() override;

private:

    bool toolOperations() override;

private Q_SLOTS:

    void slotAssignSettings2Widget() override;
    void slotSettingsChanged() override;

private:

    DIntNumInput* m_radiusInput;
    bool          m_changeSettings;
};

} // namespace DigikamBqmBlurPlugin

#endif // DIGIKAM_BQM_BLUR_H
