/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-26
 * Description : a digiKam image editor tool for add film
 *               grain on an image.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_EDITOR_FILM_GRAIN_TOOL_H
#define DIGIKAM_EDITOR_FILM_GRAIN_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorFilmGrainToolPlugin
{

class FilmGrainTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit FilmGrainTool(QObject* const parent);
    ~FilmGrainTool() override;

private Q_SLOTS:

    void slotResetSettings() override;

private:

    void readSettings() override;
    void writeSettings() override;
    void preparePreview() override;
    void prepareFinal() override;
    void setPreviewImage() override;
    void setFinalImage() override;
    void renderingFinished() override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorFilmGrainToolPlugin

#endif // DIGIKAM_EDITOR_FILM_GRAIN_TOOL_H
