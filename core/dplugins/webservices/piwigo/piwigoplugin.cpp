/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to Piwigo web-service.
 *
 * Copyright (C) 2018-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "piwigoplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "piwigowindow.h"

namespace Digikam
{

PiwigoPlugin::PiwigoPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

PiwigoPlugin::~PiwigoPlugin()
{
    delete m_toolDlg;
}

QString PiwigoPlugin::name() const
{
    return i18n("Piwigo");
}

QString PiwigoPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon PiwigoPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-piwigo"));
}

QString PiwigoPlugin::description() const
{
    return i18n("A tool to export to Piwigo web-service");
}

QString PiwigoPlugin::details() const
{
    return i18n("<p>This tool permit to export items to Piwigo web-service.</p>"
                "<p>See Piwigo web site for details: <a href='https://piwigo.org/'>https://piwigo.org/</a></p>");
}

QList<DPluginAuthor> PiwigoPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QLatin1String("Renchi Raju"),
                             QLatin1String("renchi dot raju at gmail dot com"),
                             QLatin1String("(C) 2003-2005"))
            << DPluginAuthor(QLatin1String("Colin Guthrie"),
                             QLatin1String("kde at colin dot guthr dot ie"),
                             QLatin1String("(C)  2006-2007"))
            << DPluginAuthor(QLatin1String("Andrea Diamantini"),
                             QLatin1String("adjam7 at gmail dot com"),
                             QLatin1String("(C) 2008"))
            << DPluginAuthor(QLatin1String("Frédéric Coiffier"),
                             QLatin1String("frederic dot coiffier at free dot com"),
                             QLatin1String("(C) 2010-2014"))
            << DPluginAuthor(QLatin1String("Gilles Caulier"),
                             QLatin1String("caulier dot gilles at gmail dot com"),
                             QLatin1String("(C) 2006-2019"))
            ;
}

void PiwigoPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Piwigo..."));
    ac->setObjectName(QLatin1String("export_piwigo"));
    ac->setActionCategory(DPluginAction::GenericExport);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotPiwigo()));

    addAction(ac);
}

void PiwigoPlugin::slotPiwigo()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new PiwigoWindow(infoIface(sender()), 0);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace Digikam
