/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : configuration view for external editor plugin
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DPLUGIN_CONF_VIEW_EDITOR_H
#define DIGIKAM_DPLUGIN_CONF_VIEW_EDITOR_H

// Qt includes

#include <QString>

// Local includes

#include "dpluginconfview.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPluginConfViewEditor : public DPluginConfView
{
    Q_OBJECT

public:

    explicit DPluginConfViewEditor(QWidget* const parent = nullptr);
    ~DPluginConfViewEditor() override;

    void loadPlugins()       override;
};

} // namespace Digikam

#endif // DIGIKAM_DPLUGIN_CONF_VIEW_EDITOR_H
