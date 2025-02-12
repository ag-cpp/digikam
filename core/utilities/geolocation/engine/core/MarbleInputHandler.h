/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"
#include "GeoDataCoordinates.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QTouchEvent;
class QWheelEvent;
class QGestureEvent;
class QCursor;
class QTimer;
class QString;
class QRect;

namespace Marble
{

class MarbleAbstractPresenter;
class AbstractDataPluginItem;
class RenderPlugin;

class DIGIKAM_EXPORT MarbleInputHandler  : public QObject
{
    Q_OBJECT

public:

    explicit MarbleInputHandler(MarbleAbstractPresenter*);
    ~MarbleInputHandler() override;

    void setPositionSignalConnected(bool connected);
    bool isPositionSignalConnected() const;

    /**
    //
    // The MarbleInputHandler handles mouse and keyboard input.
    //

     * @brief  Set whether a popup menu appears on a click (not drag) with the left mouse button
     * @param  enabled True to enable the popup menu (default), false to disable it
     */
    void setMouseButtonPopupEnabled(Qt::MouseButton mouseButton, bool enabled);

    /**
     * @brief  Return whether the left mouse button popup menu is active
     * @return True iff a popup menu is shown on left mouse button clicks
     */
    bool isMouseButtonPopupEnabled(Qt::MouseButton mouseButton) const;

    void setPanViaArrowsEnabled(bool enabled);

    bool panViaArrowsEnabled() const;

    void setInertialEarthRotationEnabled(bool enabled);

    /**
     * @brief Returns true iff dragging the map with the mouse keeps spinning
     * in the chosen direction for a slightly longer time than the mouse is
     * actually performing the drag operation
     */
    bool inertialEarthRotationEnabled() const;

    void setMouseViewRotationEnabled(bool enabled);

    bool mouseViewRotationEnabled() const;

    /// should the map do kinetic scrolling, this would stop the operation
    virtual void stopInertialEarthRotation();

Q_SIGNALS:

    // Mouse button menus
    void lmbRequest(int, int);
    void rmbRequest(int, int);

    //Gps coordinates
    void mouseClickScreenPosition(int, int);
    void mouseMoveGeoPosition(const QString&);

    /*
     * To detect mouse click followed by mouse move
     * with no mouse move in between
     */
    void mouseClickGeoPosition(qreal, qreal, GeoDataCoordinates::Unit);

protected Q_SLOTS:

    void restoreViewContext();

protected:

    class Protected;
    Protected* const d = nullptr;

private Q_SLOTS:

    virtual void installPluginEventFilter(RenderPlugin* renderPlugin) = 0;

private:

    Q_DISABLE_COPY(MarbleInputHandler)
};

class AbstractSelectionRubber
{
public:

    virtual ~AbstractSelectionRubber() {}
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isVisible() const = 0;
    virtual const QRect& geometry() const = 0;
    virtual void setGeometry(const QRect& geometry) = 0;
};

class DIGIKAM_EXPORT MarbleDefaultInputHandler  : public MarbleInputHandler
{
    Q_OBJECT

public:

    explicit MarbleDefaultInputHandler(MarbleAbstractPresenter* marblePresenter);
    ~MarbleDefaultInputHandler() override;

    void stopInertialEarthRotation() override;

protected:

    bool eventFilter(QObject*, QEvent*) override;
    bool handleMouseEvent(QMouseEvent* e);
    bool handlePinch(const QPointF& center, qreal scaleFactor, Qt::GestureState state);

    //FIXME - refactor (abstraction & composition)
    const AbstractDataPluginItem* lastToolTipItem() const;
    QTimer* toolTipTimer();
    QPoint toolTipPosition() const;

    virtual bool handleKeyPress(QKeyEvent* e);
    virtual void handleMouseButtonPressAndHold(const QPoint& position);

private Q_SLOTS:

    void installPluginEventFilter(RenderPlugin* renderPlugin) override = 0;
    virtual void showLmbMenu(int, int) = 0;
    virtual void showRmbMenu(int, int) = 0;
    void handlePressAndHold();

    virtual void openItemToolTip() = 0;
    virtual void setCursor(const QCursor&) = 0;

    void lmbTimeout();

private:

    virtual AbstractSelectionRubber* selectionRubber() = 0;
    virtual bool layersEventFilter(QObject*, QEvent*) = 0;

    virtual bool handleTouch(QTouchEvent* e);
    virtual bool handleDoubleClick(QMouseEvent* e);
    virtual bool handleWheel(QWheelEvent* e);
    virtual bool handleGesture(QGestureEvent* e);

    virtual void handleMouseButtonPress(QMouseEvent* e);
    virtual void handleLeftMouseButtonPress(QMouseEvent* e);
    virtual void handleRightMouseButtonPress(QMouseEvent* e);
    virtual void handleMiddleMouseButtonPress(QMouseEvent* e);
    virtual void handleMouseButtonRelease(QMouseEvent* e);

    virtual void hideSelectionIfCtrlReleased(QEvent* e);
    virtual void checkReleasedMove(QMouseEvent* e);

    //Returns whatever should be returned from mouse event handling loop
    virtual bool acceptMouse();

    void notifyPosition(bool isAboveMap, qreal mouseLon, qreal mouseLat);
    QPoint mouseMovedOutside(QMouseEvent* event);
    void adjustCursorShape(const QPoint& mousePosition, const QPoint& mouseDirection);

    Q_DISABLE_COPY(MarbleDefaultInputHandler)

private:

    class Private;
    Private* const d = nullptr;      // cppcheck-suppress duplInheritedMember
};

} // Namespace Marble
