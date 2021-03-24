/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-01-18
 * Description : a widget class to edit perspective.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_EDITOR_PERSPECTIVE_WIDGET_H
#define DIGIKAM_EDITOR_PERSPECTIVE_WIDGET_H

// Qt includes

#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>
#include <QColor>

// Local includes

#include "dimg.h"
#include "perspectivematrix.h"
#include "imageiface.h"

using namespace Digikam;

namespace DigikamEditorPerspectiveToolPlugin
{

class PerspectiveWidget : public QWidget
{
    Q_OBJECT

public:

    PerspectiveWidget(int width, int height, QWidget* const parent = nullptr);
    ~PerspectiveWidget() override;

    QRect  getTargetSize()        const;
    QPoint getTopLeftCorner()     const;
    QPoint getTopRightCorner()    const;
    QPoint getBottomLeftCorner()  const;
    QPoint getBottomRightCorner() const;

    float getAngleTopLeft()       const;
    float getAngleTopRight()      const;
    float getAngleBottomLeft()    const;
    float getAngleBottomRight()   const;

    ImageIface* imageIface()      const;

    void  setBackgroundColor(const QColor& bg);
    void  applyPerspectiveAdjustment();
    void  reset();

public Q_SLOTS:

    void slotToggleAntiAliasing(bool a);
    void slotToggleDrawWhileMoving(bool draw);
    void slotToggleDrawGrid(bool grid);

    void slotChangeGuideColor(const QColor& color);
    void slotChangeGuideSize(int size);
    void slotInverseTransformationChanged(bool isEnabled);

Q_SIGNALS:

    void signalPerspectiveChanged(const QRect& newSize, float topLeftAngle, float topRightAngle,
                                  float bottomLeftAngle, float bottomRightAngle, bool valid);

protected:

    void paintEvent(QPaintEvent*)           override;
    void resizeEvent(QResizeEvent*)         override;
    void mousePressEvent(QMouseEvent*)      override;
    void mouseReleaseEvent(QMouseEvent*)    override;
    void mouseMoveEvent(QMouseEvent*)       override;

private:  // Widget methods.

    void   updatePixmap();

    void   transformAffine(DImg* const orgImage, DImg* const destImage,
                           const PerspectiveMatrix& matrix, const DColor& background);

    QPoint buildPerspective(const QPoint& orignTopLeft, const QPoint& orignBottomRight,
                            const QPoint& transTopLeft, const QPoint& transTopRight,
                            const QPoint& transBottomLeft, const QPoint& transBottomRight,
                            DImg* const orgImage=nullptr, DImg* const destImage=nullptr,
                            const DColor& background=DColor());

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorPerspectiveToolPlugin

#endif // DIGIKAM_EDITOR_PERSPECTIVE_WIDGET_H
