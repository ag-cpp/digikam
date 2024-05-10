/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to rotate an image.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FREEROTATIONTOOL_PLUGIN_H
#define DIGIKAM_FREEROTATIONTOOL_PLUGIN_H

// Local includes

#include "dplugineditor.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.editor.FreeRotationTool"

using namespace Digikam;

namespace DigikamEditorFreeRotationToolPlugin
{

class FreeRotationToolPlugin : public DPluginEditor
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginEditor)

public:

    explicit FreeRotationToolPlugin(QObject* const parent = nullptr);
    ~FreeRotationToolPlugin()            override = default;

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

Q_SIGNALS:

    void signalPoint1Action();
    void signalPoint2Action();
    void signalAutoAdjustAction();

private Q_SLOTS:

    void slotFreeRotation();
};

} // namespace DigikamEditorFreeRotationToolPlugin

#endif // DIGIKAM_FREEROTATIONTOOL_PLUGIN_H
