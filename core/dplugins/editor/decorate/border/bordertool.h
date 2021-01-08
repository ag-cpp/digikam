/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-01-20
 * Description : a digiKam image tool to add a border
 *               around an image.
 *
 * Copyright 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_EDITOR_BORDER_TOOL_H
#define DIGIKAM_EDITOR_BORDER_TOOL_H

// Qt includes

#include <QString>

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorBorderToolPlugin
{

class BorderTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit BorderTool(QObject* const parent);
    ~BorderTool() override;

private Q_SLOTS:

    void slotResetSettings() override;

private:

    void readSettings() override;
    void writeSettings() override;
    void preparePreview() override;
    void prepareFinal() override;
    void setPreviewImage() override;
    void setFinalImage() override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorBorderToolPlugin

#endif // DIGIKAM_EDITOR_BORDER_TOOL_H
