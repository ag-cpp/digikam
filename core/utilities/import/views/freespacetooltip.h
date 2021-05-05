/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-01-19
 * Description : free space widget tool tip
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

#ifndef DIGIKAM_FREE_SPACE_TOOL_TIP_H
#define DIGIKAM_FREE_SPACE_TOOL_TIP_H

// Local includes

#include "ditemtooltip.h"

namespace Digikam
{

class FreeSpaceToolTip : public DItemToolTip
{
    Q_OBJECT

public:

    explicit FreeSpaceToolTip(QWidget* const parent);
    ~FreeSpaceToolTip()         override;

    void setToolTip(const QString& tip);

    void show();

protected:

    QRect   repositionRect()    override;
    QString tipContents()       override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FREE_SPACE_TOOL_TIP_H
