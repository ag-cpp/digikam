/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-17
 * Description : resize image batch tool.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_RESIZE_H
#define DIGIKAM_BQM_RESIZE_H

// Local includes

#include "batchtool.h"

using namespace Digikam;

namespace DigikamBqmResizePlugin
{

class Resize : public BatchTool
{
    Q_OBJECT

public:

    explicit Resize(QObject* const parent = nullptr);
    ~Resize()                                               override;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;
    void slotPercentChanged();

private:

    bool toolOperations()                                   override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamBqmResizePlugin

#endif // DIGIKAM_BQM_RESIZE_H
