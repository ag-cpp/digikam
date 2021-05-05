/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : a digiKam image tool to reduce
 *               vignetting on an image.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H
#define DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorAntivignettingToolPlugin
{

class AntiVignettingTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit AntiVignettingTool(QObject* const parent);
    ~AntiVignettingTool()       override;

private Q_SLOTS:

    void slotResetSettings()    override;

private:

    void writeSettings()        override;
    void readSettings()         override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorAntivignettingToolPlugin

#endif // DIGIKAM_EDITOR_ANTI_VIGNETTING_TOOL_H
