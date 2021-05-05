/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-31
 * Description : Auto-Color correction tool.
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

#ifndef DIGIKAM_EDITOR_AUTO_CORRECTION_TOOL_H
#define DIGIKAM_EDITOR_AUTO_CORRECTION_TOOL_H

// Qt includes

#include <QPixmap>

// Local includes

#include "editortool.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamEditorAutoCorrectionToolPlugin
{

class AutoCorrectionTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit AutoCorrectionTool(QObject* const parent);
    ~AutoCorrectionTool()       override;

private Q_SLOTS:

    void slotInit() override;
    void slotResetSettings()    override;

private:

    void writeSettings()        override;
    void readSettings()         override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

    void autoCorrection(DImg* const img, DImg* const ref, int type);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorAutoCorrectionToolPlugin

#endif // DIGIKAM_EDITOR_AUTO_CORRECTION_TOOL_H
