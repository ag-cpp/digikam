/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to reduce vignetting
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ANTIVIGNETTINGTOOL_PLUGIN_H
#define DIGIKAM_ANTIVIGNETTINGTOOL_PLUGIN_H

// Local includes

#include "dplugineditor.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.editor.AntiVignettingTool"

using namespace Digikam;

namespace DigikamEditorAntivignettingToolPlugin
{

class AntiVignettingToolPlugin : public DPluginEditor
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginEditor)

public:

    explicit AntiVignettingToolPlugin(QObject* const parent = nullptr);
    ~AntiVignettingToolPlugin()          override = default;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;
    QString handbookSection()      const override;
    QString handbookChapter()      const override;
    QString handbookReference()    const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotAntiVignetting();
};

} // namespace DigikamEditorAntivignettingToolPlugin

#endif // DIGIKAM_ANTIVIGNETTINGTOOL_PLUGIN_H
