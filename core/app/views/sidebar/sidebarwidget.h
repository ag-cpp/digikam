/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : base class for sidebar widgets
 *
 * Copyright (C) 2009      by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SIDE_BAR_WIDGET_H
#define DIGIKAM_SIDE_BAR_WIDGET_H

// QT includes

#include <QPixmap>
#include <QWidget>

// Local includes

#include "album.h"
#include "iteminfo.h"
#include "statesavingobject.h"

namespace Digikam
{

/**
 * Abstract base class for widgets that are use in one of digikams's sidebars.
 */
class SidebarWidget : public QWidget, public StateSavingObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent of this widget, may be null
     */
    explicit SidebarWidget(QWidget* const parent);

    /**
     * Destructor.
     */
    ~SidebarWidget() override;

    /**
     * This method is called if the visible sidebar widget is changed.
     *
     * @param active if true, this widget is the new active widget, if false another
     *        widget is active
     */
    virtual void setActive(bool active)                             = 0;

    /**
     * This method is invoked when the application settings should be (re-)
     * applied to this widget.
     */
    virtual void applySettings()                                    = 0;

    /**
     * This is called on this widget when the history requires to move back to
     * the specified album
     */
    virtual void changeAlbumFromHistory(const QList<Album*>& album) = 0;

    /**
     * Must be implemented and return the icon that shall be visible for this
     * sidebar widget.
     *
     * @return pixmap icon
     */
    virtual const QIcon getIcon()                                   = 0;

    /**
     * Must be implemented to return the title of this sidebar's tab.
     *
     * @return localized title string
     */
    virtual const QString getCaption()                              = 0;

Q_SIGNALS:

    /**
     * This signal can be emitted if this sidebar widget wants to be the one
     * that is active.
     */
    void requestActiveTab(SidebarWidget*);

    /**
     * To dispatch error message to temporized pop-up notification widget hosted with icon-view.
     */
    void signalNotificationError(const QString& message, int type);
};

} // namespace Digikam

#endif // DIGIKAM_SIDE_BAR_WIDGET_H
