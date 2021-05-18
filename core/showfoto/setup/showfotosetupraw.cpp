/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-06
 * Description : setup RAW decoding settings.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotosetupraw.h"

// Qt includes

#include <QTabWidget>

// Local includes

#include "setupraw.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoSetupRaw::Private
{
public:

    explicit Private()
      : tab(nullptr),
        raw(nullptr)
    {
    }

    QTabWidget*        tab;
    Digikam::SetupRaw* raw;
};

ShowfotoSetupRaw::ShowfotoSetupRaw(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab = new QTabWidget;
    d->raw = new Digikam::SetupRaw(d->tab);

    setWidget(d->tab);
    setWidgetResizable(true);

    readSettings();
}

ShowfotoSetupRaw::~ShowfotoSetupRaw()
{
    delete d;
}

void ShowfotoSetupRaw::applySettings()
{
    d->raw->applySettings();
}

void ShowfotoSetupRaw::readSettings()
{
    d->raw->readSettings();
}

} // namespace ShowFoto
