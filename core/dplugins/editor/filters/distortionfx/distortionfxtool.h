/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-02-11
 * Description : a tool to apply Distortion FX to an image.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * Original Distortion algorithms copyrighted 2004-2005 by
 * Pieter Z. Voloshyn <pieter dot voloshyn at gmail dot com>.
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

#ifndef DIGIKAM_EDITOR_DISTORTION_FX_TOOL_H
#define DIGIKAM_EDITOR_DISTORTION_FX_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorDistortionFxToolPlugin
{

class DistortionFXTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit DistortionFXTool(QObject* const parent);
    ~DistortionFXTool()         override;

private Q_SLOTS:

    void slotEffectTypeChanged(int type);
    void slotResetSettings()    override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;
    void renderingFinished()    override;
    void blockWidgetSignals(bool b);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorDistortionFxToolPlugin

#endif // DIGIKAM_EDITOR_DISTORTION_FX_TOOL_H
