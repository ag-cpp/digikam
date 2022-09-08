/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to convert 8 bits color depth to 16.
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_CONVERT8TO16TOOL_PLUGIN_H
#define DIGIKAM_CONVERT8TO16TOOL_PLUGIN_H

// Local includes

#include "dplugineditor.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.editor.Convert8To16Tool"

using namespace Digikam;

namespace DigikamEditorConvert8To16ToolPlugin
{

class Convert8To16ToolPlugin : public DPluginEditor
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginEditor)

public:

    explicit Convert8To16ToolPlugin(QObject* const parent = nullptr);
    ~Convert8To16ToolPlugin()            override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotConvert8To16();
};

} // namespace DigikamEditorConvert8To16ToolPlugin

#endif // DIGIKAM_CONVERT8TO16TOOL_PLUGIN_H
