/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI - Multi-tab bar implementation.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2001-2003 by Joseph Wenninger <jowenn at kde dot org>
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

#include "sidebar_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN DMultiTabBar::Private
{
public:

    explicit Private()
      : internal (nullptr),
        layout   (nullptr),
        btnTabSep(nullptr),
        position (Qt::LeftEdge)
    {
    }

    DMultiTabBarFrame*         internal;
    QBoxLayout*                layout;
    QFrame*                    btnTabSep;
    QList<DMultiTabBarButton*> buttons;
    Qt::Edge                   position;
};

DMultiTabBar::DMultiTabBar(Qt::Edge pos, QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    if ((pos == Qt::LeftEdge) || (pos == Qt::RightEdge))
    {
        d->layout = new QVBoxLayout(this);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    else
    {
        d->layout = new QHBoxLayout(this);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    d->layout->setContentsMargins(QMargins());
    d->layout->setSpacing(0);

    d->internal = new DMultiTabBarFrame(this, pos);
    setPosition(pos);
    setStyle(ActiveIconText);
    d->layout->insertWidget(0, d->internal);
    d->layout->insertWidget(0, d->btnTabSep = new QFrame(this));
    d->btnTabSep->setFixedHeight(4);
    d->btnTabSep->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    d->btnTabSep->setLineWidth(2);
    d->btnTabSep->hide();

    updateGeometry();
}

DMultiTabBar::~DMultiTabBar()
{
    qDeleteAll(d->buttons);
    d->buttons.clear();
    delete d;
}

void DMultiTabBar::appendButton(const QPixmap &pic, int id, QMenu *popup, const QString&)
{
    DMultiTabBarButton* const btn = new DMultiTabBarButton(pic, QString(), id, this);

    // a button with a QMenu can have another size. Make sure the button has always the same size.

    btn->setFixedWidth(btn->height());
    btn->setMenu(popup);
    d->buttons.append(btn);
    d->layout->insertWidget(0, btn);
    btn->show();
    d->btnTabSep->show();
}

void DMultiTabBar::updateSeparator()
{
    bool hideSep = true;
    QListIterator<DMultiTabBarButton*> it(d->buttons);

    while (it.hasNext())
    {
        if (it.next()->isVisibleTo(this))
        {
            hideSep = false;
            break;
        }
    }

    if (hideSep)
    {
        d->btnTabSep->hide();
    }
    else
    {
       d->btnTabSep->show();
    }
}

void DMultiTabBar::appendTab(const QPixmap& pic, int id, const QString& text)
{
    d->internal->appendTab(pic, id, text);
}

DMultiTabBarButton* DMultiTabBar::button(int id) const
{
    QListIterator<DMultiTabBarButton*> it(d->buttons);

    while (it.hasNext())
    {
        DMultiTabBarButton* const button = it.next();

        if (button->id() == id)
        {
            return button;
        }
    }

    return nullptr;
}

DMultiTabBarTab* DMultiTabBar::tab(int id) const
{
    return d->internal->tab(id);
}

void DMultiTabBar::removeButton(int id)
{
    for (int pos = 0 ; pos < d->buttons.count() ; ++pos)
    {
        if (d->buttons.at(pos)->id() == id)
        {
            d->buttons.takeAt(pos)->deleteLater();
            break;
        }
    }

    if (d->buttons.count() == 0)
    {
        d->btnTabSep->hide();
    }
}

void DMultiTabBar::removeTab(int id)
{
    d->internal->removeTab(id);
}

void DMultiTabBar::setTab(int id,bool state)
{
    DMultiTabBarTab* const ttab = tab(id);

    if (ttab)
    {
        ttab->setState(state);
    }
}

bool DMultiTabBar::isTabRaised(int id) const
{
    DMultiTabBarTab* const ttab = tab(id);

    if (ttab)
    {
        return ttab->isChecked();
    }

    return false;
}

void DMultiTabBar::setStyle(TextStyle style)
{
    d->internal->setStyle(style);
}

DMultiTabBar::TextStyle DMultiTabBar::tabStyle() const
{
    return d->internal->d->style;
}

void DMultiTabBar::setPosition(Qt::Edge pos)
{
    d->position = pos;
    d->internal->setPosition(pos);
}

Qt::Edge DMultiTabBar::position() const
{
    return d->position;
}

void DMultiTabBar::fontChange(const QFont&)
{
    updateGeometry();
}

} // namespace Digikam
