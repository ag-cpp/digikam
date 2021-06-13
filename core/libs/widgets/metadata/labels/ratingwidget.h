/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-08-15
 * Description : a widget to draw stars rating
 *
 * Copyright (C) 2005      by Owen Hirst <n8rider@sbcglobal.net>
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

#ifndef DIGIKAM_RATING_WIDGET_H
#define DIGIKAM_RATING_WIDGET_H

// Qt includes

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPolygon>
#include <QIcon>
#include <QMenu>

// Local includes

#include "dlayoutbox.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT RatingWidget : public QWidget
{
    Q_OBJECT

public:

    explicit RatingWidget(QWidget* const parent);
    ~RatingWidget() override;

    void setRating(int val);
    int  rating() const;

    void setTracking(bool tracking);
    bool hasTracking() const;

    void setFading(bool fading);
    bool hasFading() const;

    void startFading();
    void stopFading();
    void setVisibleImmediately();

    void setVisible(bool visible)                   override;
    int  maximumVisibleWidth() const;

    /**
     * Pre-computed star polygon for a 15x15 pixmap.
     */
    static QPolygon starPolygon();
    static QIcon buildIcon(int rate, int size);

Q_SIGNALS:

    void signalRatingModified(int);    ///< Not managed by tracking properties
    void signalRatingChanged(int);

protected:

    int regPixmapWidth()         const;

    QPixmap starPixmap()         const;
    QPixmap starPixmapFilled()   const;
    QPixmap starPixmapDisabled() const;

    void regeneratePixmaps();
    void setupTimeLine();
    void applyFading(QPixmap& pix);

    void mousePressEvent(QMouseEvent*)      override;
    void mouseMoveEvent(QMouseEvent*)       override;
    void mouseReleaseEvent(QMouseEvent*)    override;
    void paintEvent(QPaintEvent*)           override;

protected Q_SLOTS:

    void setFadingValue(int value);

private Q_SLOTS:

    void slotThemeChanged();

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------------------------

class DIGIKAM_EXPORT RatingBox : public DVBox
{
    Q_OBJECT

public:

    explicit RatingBox(QWidget* const parent);
    ~RatingBox() override;

Q_SIGNALS:

    void signalRatingChanged(int);

private Q_SLOTS:

    void slotUpdateDescription(int);

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------------------------

class DIGIKAM_EXPORT RatingMenuAction : public QMenu
{
    Q_OBJECT

public:

    explicit RatingMenuAction(QMenu* const parent = nullptr);
    ~RatingMenuAction() override;

Q_SIGNALS:

    void signalRatingChanged(int);
};

} // namespace Digikam

#endif // DIGIKAM_RATING_WIDGET_H
