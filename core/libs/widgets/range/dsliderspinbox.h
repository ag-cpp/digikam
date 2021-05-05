/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-11-30
 * Description : Save space slider widget
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2010 by Justin Noel <justin at ics dot com>
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

#ifndef DIGIKAM_DSLIDER_SPINBOX_H
#define DIGIKAM_DSLIDER_SPINBOX_H

// Qt includes

#include <QAbstractSpinBox>
#include <QStyleOptionSpinBox>
#include <QStyleOptionProgressBar>

namespace Digikam
{

class DAbstractSliderSpinBoxPrivate;
class DSliderSpinBoxPrivate;
class DDoubleSliderSpinBoxPrivate;

class DAbstractSliderSpinBox : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DAbstractSliderSpinBox)
    Q_DECLARE_PRIVATE(DAbstractSliderSpinBox)

protected:

    explicit DAbstractSliderSpinBox(QWidget* const parent, DAbstractSliderSpinBoxPrivate* const q);

public:

    ~DAbstractSliderSpinBox() override;

    void showEdit();
    void hideEdit();

    void setPrefix(const QString& prefix);
    void setSuffix(const QString& suffix);

    void setExponentRatio(double dbl);

    /**
     * If set to block, it informs inheriting classes that they shouldn't emit signals
     * if the update comes from a mouse dragging the slider.
     * Set this to true when dragging the slider and updates during the drag are not needed.
     */
    void setBlockUpdateSignalOnDrag(bool block);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    virtual QSize minimumSize() const;

    bool isDragging() const;

protected:

    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;
    bool eventFilter(QObject* recv, QEvent* e) override;

    QStyleOptionSpinBox spinBoxOptions() const;
    QStyleOptionProgressBar progressBarOptions() const;

    QRect progressRect(const QStyleOptionSpinBox& spinBoxOptions) const;
    QRect upButtonRect(const QStyleOptionSpinBox& spinBoxOptions) const;
    QRect downButtonRect(const QStyleOptionSpinBox& spinBoxOptions) const;

    int valueForX(int x, Qt::KeyboardModifiers modifiers = Qt::NoModifier) const;

    virtual QString valueString() const = 0;

    /**
     * Sets the slider internal value. Inheriting classes should respect blockUpdateSignal
     * so that, in specific cases, we have a performance improvement. See setIgnoreMouseMoveEvents.
     */
    virtual void setInternalValue(int value, bool blockUpdateSignal) = 0;

protected Q_SLOTS:

    void contextMenuEvent(QContextMenuEvent* event) override;
    void editLostFocus();

protected:

    DAbstractSliderSpinBoxPrivate* const d_ptr;

    // -- QWidget interface --------------------------------------

protected:

    void changeEvent(QEvent* e) override;
    void paint(QPainter& painter);
    void paintFusion(QPainter& painter);
    void paintPlastique(QPainter& painter);
    void paintBreeze(QPainter& painter);

private:

    void setInternalValue(int value);
};

// ---------------------------------------------------------------------------------

class DSliderSpinBox : public DAbstractSliderSpinBox
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DSliderSpinBox)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)

public:

    explicit DSliderSpinBox(QWidget* const parent = nullptr);
    ~DSliderSpinBox() override;

    void setRange(int minimum, int maximum);

    int  minimum() const;
    void setMinimum(int minimum);
    int  maximum() const;
    void setMaximum(int maximum);
    int  fastSliderStep() const;
    void setFastSliderStep(int step);

    /// Get the value, don't use value()
    int  value();

    void setSingleStep(int value);
    void setPageStep(int value);

public Q_SLOTS:

    /// Set the value, don't use setValue()
    void setValue(int value);

protected:

    QString valueString() const override;
    void setInternalValue(int value, bool blockUpdateSignal) override;

Q_SIGNALS:

    void valueChanged(int value);
};

// ---------------------------------------------------------------------------------

class DDoubleSliderSpinBox : public DAbstractSliderSpinBox
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DDoubleSliderSpinBox)

public:

    explicit DDoubleSliderSpinBox(QWidget* const parent = nullptr);
    ~DDoubleSliderSpinBox() override;

    void   setRange(double minimum, double maximum, int decimals = 0);

    double minimum() const;
    void   setMinimum(double minimum);
    double maximum() const;
    void   setMaximum(double maximum);
    double fastSliderStep() const;
    void   setFastSliderStep(double step);

    double value();
    void   setSingleStep(double value);

public Q_SLOTS:

    void setValue(double value);

protected:

    QString valueString() const override;
    void setInternalValue(int value, bool blockUpdateSignal) override;

Q_SIGNALS:

    void valueChanged(double value);
};

} // namespace Digikam

#endif // DIGIKAM_DSLIDER_SPINBOX_H
