/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-02-01
 * Description : Kinetic Scroller for Thumbnail Bar
 *               based on Razvan Petru implementation.
 *
 * Copyright (C) 2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#ifndef SHOW_FOTO_KINETIC_SCROLLER_H
#define SHOW_FOTO_KINETIC_SCROLLER_H

// Qt includes

#include <QObject>
#include <QScopedPointer>
#include <QAbstractScrollArea>
#include <QListView>

namespace ShowFoto
{

/**
 * Vertical kinetic scroller implementation without overshoot and bouncing.
 * A temporary solution to get kinetic-like scrolling on Symbian.
 */
class ShowfotoKineticScroller: public QObject
{
   Q_OBJECT

public:

    explicit ShowfotoKineticScroller(QObject* const parent = nullptr);
    ~ShowfotoKineticScroller() override;

    //! enabled for one widget only, new calls remove previous association
    void enableKineticScrollFor(QAbstractScrollArea* const scrollArea);
    void setScrollFlow(QListView::Flow flow);

protected:

    bool eventFilter(QObject* object, QEvent* event) override;

private Q_SLOTS:

    void onKineticTimerElapsed();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_KINETIC_SCROLLER_H
