/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to OneDrive web-service.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "odplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "odwindow.h"

namespace DigikamGenericOneDrivePlugin
{

ODPlugin::ODPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

ODPlugin::~ODPlugin()
{
}

void ODPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString ODPlugin::name() const
{
    return i18nc("@title", "OneDrive");
}

QString ODPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ODPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-onedrive"));
}

QString ODPlugin::description() const
{
    return i18nc("@info", "A tool to export to OneDrive web-service");
}

QString ODPlugin::details() const
{
    return i18nc("@info", "This tool permit to export items to OneDrive web-service.\n\n"
                 "See OneDrive web site for details: %1",
                 QLatin1String("<a href='https://onedrive.live.com/'>https://onedrive.live.com/</a>"));
}

QList<DPluginAuthor> ODPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Tarek Talaat"),
                             QString::fromUtf8("tarektalaat93 at gmail dot com"),
                             QString::fromUtf8("(C) 2018"))
            ;
}

void ODPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Onedrive..."));
    ac->setObjectName(QLatin1String("export_onedrive"));
    ac->setActionCategory(DPluginAction::GenericExport);
    ac->setShortcut(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_O);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotOneDrive()));

    addAction(ac);
}

void ODPlugin::slotOneDrive()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new ODWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericOneDrivePlugin
