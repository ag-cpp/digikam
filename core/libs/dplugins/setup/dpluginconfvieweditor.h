/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : configuration view for external editor plugin
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
