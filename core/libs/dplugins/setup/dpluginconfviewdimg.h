/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-09-22
 * Description : configuration view for external DImg plugin
 *
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>

// Local includes

#include "dpluginconfview.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPluginConfViewDImg : public DPluginConfView
{
    Q_OBJECT

public:

    explicit DPluginConfViewDImg(QWidget* const parent = nullptr);
    ~DPluginConfViewDImg() override = default;

    void loadPlugins()     override;
};

} // namespace Digikam
