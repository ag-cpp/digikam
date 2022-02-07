/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to rotate an image.
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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
    ~FreeRotationToolPlugin()            override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

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
