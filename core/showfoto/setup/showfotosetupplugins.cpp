/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-06
 * Description : Setup view panel for dplugins.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotosetupplugins.h"

// Qt includes

#include <QTabWidget>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dpluginsetup.h"
#include "dpluginconfviewgeneric.h"
#include "dpluginconfvieweditor.h"
#include "dpluginconfviewdimg.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN SetupPlugins::Private
{
public:

    explicit Private()
      : tab         (nullptr),
        setupGeneric(nullptr),
        setupEditor (nullptr),
        setupDImg   (nullptr)
    {
    }

    QTabWidget*            tab;

    Digikam::DPluginSetup* setupGeneric;
    Digikam::DPluginSetup* setupEditor;
    Digikam::DPluginSetup* setupDImg;
};

SetupPlugins::SetupPlugins(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab          = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    // --------------------

    d->setupGeneric = new Digikam::DPluginSetup(d->tab);
    d->setupGeneric->setPluginConfView(new Digikam::DPluginConfViewGeneric(d->setupGeneric));
    d->tab->insertTab(Generic, d->setupGeneric, i18nc("@title:tab generic plugins list", "Generic"));

    // --------------------

    d->setupEditor  = new Digikam::DPluginSetup(d->tab);
    d->setupEditor->setPluginConfView(new Digikam::DPluginConfViewEditor(d->setupEditor));
    d->tab->insertTab(Editor, d->setupEditor, i18nc("@title:tab editor plugins list", "Image Editor"));

    // --------------------

    d->setupDImg    = new Digikam::DPluginSetup(d->tab);
    d->setupDImg->setPluginConfView(new Digikam::DPluginConfViewDImg(d->setupEditor));
    d->tab->insertTab(Loaders, d->setupDImg, i18nc("@title:tab loader plugins list", "Image Loaders"));
}

SetupPlugins::~SetupPlugins()
{
    delete d;
}

void SetupPlugins::applySettings()
{
    d->setupGeneric->applySettings();
    d->setupEditor->applySettings();
    d->setupDImg->applySettings();
}

} // namespace ShowFoto
