/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-02-14
 * Description : a digiKam image tool for to apply a color
 *               effect to an image.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_COLOR_FX_TOOL_H
#define DIGIKAM_EDITOR_COLOR_FX_TOOL_H

// Local includes

#include "editortool.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamEditorColorFxToolPlugin
{

class ColorFxTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit ColorFxTool(QObject* const parent);
    ~ColorFxTool()              override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private Q_SLOTS:

    void slotInit()             override;
    void slotResetSettings()    override;
    void slotColorSelectedFromTarget(const Digikam::DColor& color);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorColorFxToolPlugin

#endif // DIGIKAM_EDITOR_COLOR_FX_TOOL_H
