/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-28
 * Description : a digiKam image editor tool to process image
 *               free rotation.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2010 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_EDITOR_FREE_ROTATION_TOOL_H
#define DIGIKAM_EDITOR_FREE_ROTATION_TOOL_H

// Local includes

#include "editortool.h"

class QPixmap;
class QString;

using namespace Digikam;

namespace DigikamEditorFreeRotationToolPlugin
{

class FreeRotationTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit FreeRotationTool(QObject* const parent);
    ~FreeRotationTool()         override;

public Q_SLOTS:

    void slotAutoAdjustP1Clicked();
    void slotAutoAdjustP2Clicked();
    void slotAutoAdjustClicked();

private Q_SLOTS:

    void slotResetSettings()    override;
    void slotColorGuideChanged();

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

    QString centerString(const QString& str, int maxLength = -1)         const;
    QString repeatString(const QString& str, int times)                  const;
    QPixmap generateBtnPixmap(const QString& label, const QColor& color) const;
    QString generateButtonLabel(const QPoint& p)                         const;
    double  calculateAutoAngle()                                         const;
    bool    pointIsValid(const QPoint& p)                                const;
    void    setPointInvalid(QPoint& p);
    void    updatePoints();
    void    resetPoints();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorFreeRotationToolPlugin

#endif // DIGIKAM_EDITOR_FREE_ROTATION_TOOL_H
