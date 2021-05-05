/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * Copyright (C) 2008      by Adrian Schroeter <adrian at suse dot de>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H
#define DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorLensAutoFixToolPlugin
{

class LensAutoFixTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit LensAutoFixTool(QObject* const parent);
    ~LensAutoFixTool()          override;

private Q_SLOTS:

    void slotLensChanged();
    void slotResetSettings()    override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorLensAutoFixToolPlugin

#endif // DIGIKAM_EDITOR_LENS_AUTO_FIX_TOOL_H
