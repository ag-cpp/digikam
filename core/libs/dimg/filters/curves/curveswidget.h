/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-12-01
 * Description : a widget to draw histogram curves
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_CURVES_WIDGET_H
#define DIGIKAM_CURVES_WIDGET_H

// Qt includes

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>

// Local includes

#include "dcolor.h"
#include "digikam_export.h"
#include "digikam_globals.h"

class KConfigGroup;

namespace Digikam
{

class ImageCurves;
class DImg;

class DIGIKAM_EXPORT CurvesWidget : public QWidget
{
    Q_OBJECT

public:

    explicit CurvesWidget(int w, int h, QWidget* const parent, bool readOnly=false);
    ~CurvesWidget() override;

    void setup(int w, int h, bool readOnly);

    /**
     * Saves the currently created curve to the given group with prefix as a
     * prefix for the curve point config entries.
     *
     * @param group the group to save the curve to
     * @param prefix the prefix prepended to the point numbers in the config
     */
    void saveCurve(KConfigGroup& group, const QString& prefix);

    /**
     * Restores the curve tfrom the given group with prefix as a
     * prefix for the curve point config entries.
     *
     * @param group the group to restore the curve from
     * @param prefix the prefix prepended to the point numbers in the config
     */
    void restoreCurve(KConfigGroup& group, const QString& prefix);

    /**
     * Updates the image data the curve should be used for.
     *
     * @param img image data
     */
    void updateData(const DImg& img);

    /**
     * Stop current histogram computations.
     */
    void stopHistogramComputation();

    void setDataLoading();
    void setLoadingFailed();

    /**
     * Resets the ui including the user specified curve.
     */
    void reset();

    /**
     * Resets only the ui and keeps the curve.
     */
    void resetUI();
    void curveTypeChanged();
    void setCurveGuide(const DColor& color);

    ImageCurves* curves() const;
    bool isSixteenBits()  const;

Q_SIGNALS:

    void signalMouseMoved(int x, int y);
    void signalCurvesChanged();
    void signalHistogramComputationDone();
    void signalHistogramComputationFailed();

public Q_SLOTS:

    void setChannelType(ChannelType channel);
    void setScaleType(HistogramScale scale);

protected Q_SLOTS:

    void slotProgressTimerDone();
    void slotCalculationStarted();
    void slotCalculationFinished(bool success);

protected:

    void paintEvent(QPaintEvent*)           override;
    void mousePressEvent(QMouseEvent*)      override;
    void mouseReleaseEvent(QMouseEvent*)    override;
    void mouseMoveEvent(QMouseEvent*)       override;
    void leaveEvent(QEvent*)                override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_CURVES_WIDGET_H
