/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-02-05
 * Description : film color negative inverter tool
 *
 * Copyright (C) 2012 by Matthias Welwarsky <matthias at welwarsky dot de>
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

#ifndef DIGIKAM_EDITOR_FILM_TOOL_H
#define DIGIKAM_EDITOR_FILM_TOOL_H

// Local includes

#include "editortool.h"
#include "dcolor.h"

class QListWidgetItem;

using namespace Digikam;

namespace DigikamEditorFilmToolPlugin
{

class FilmProfile;

class FilmTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit FilmTool(QObject* const parent);
    ~FilmTool() override;

private Q_SLOTS:

    void slotResetSettings() override;
    void slotScaleChanged() override;
    void slotChannelChanged() override;
    void slotAdjustSliders();
    void slotFilmItemActivated(QListWidgetItem* item);
    void slotExposureChanged(double val);
    void slotGammaInputChanged(double val);
    void slotColorSelectedFromTarget(const Digikam::DColor& color, const QPoint& p);
    void slotPickerColorButtonActived(bool checked);
    void slotResetWhitePoint();
    void slotColorBalanceStateChanged(int);
    void slotAutoWhitePoint(void);

private:

    void readSettings() override;
    void writeSettings() override;
    void preparePreview() override;
    void prepareFinal() override;
    void setPreviewImage() override;
    void setFinalImage() override;

    void gammaInputChanged(double val);
    void setLevelsFromFilm();
    bool eventFilter(QObject*, QEvent*) override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorFilmToolPlugin

#endif // DIGIKAM_EDITOR_FILM_TOOL_H
