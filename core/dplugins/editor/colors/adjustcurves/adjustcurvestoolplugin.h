/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to adjust color curves.
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADJUSTCURVESTOOL_PLUGIN_H
#define DIGIKAM_ADJUSTCURVESTOOL_PLUGIN_H

// Local includes

#include "dplugineditor.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.editor.AdjustCurvesTool"

using namespace Digikam;

namespace DigikamEditorAdjustCurvesToolPlugin
{

class AdjustCurvesToolPlugin : public DPluginEditor
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginEditor)

public:

    explicit AdjustCurvesToolPlugin(QObject* const parent = nullptr);
    ~AdjustCurvesToolPlugin()            override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

private Q_SLOTS:

    void slotAdjustCurvesTool();
};

} // namespace DigikamEditorAdjustCurvesToolPlugin

#endif // DIGIKAM_ADJUSTCURVESTOOL_PLUGIN_H
