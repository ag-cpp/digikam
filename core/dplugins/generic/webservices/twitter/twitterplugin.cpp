/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to Twitter web-service.
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

#include "twitterplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "twitterwindow.h"

namespace DigikamGenericTwitterPlugin
{

TwitterPlugin::TwitterPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

TwitterPlugin::~TwitterPlugin()
{
}

void TwitterPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString TwitterPlugin::name() const
{
    return i18nc("@title", "Twitter");
}

QString TwitterPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon TwitterPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("internet-web-browser"));
}

QString TwitterPlugin::description() const
{
    return i18nc("@info", "A tool to export to Twitter web-service");
}

QString TwitterPlugin::details() const
{
    return i18nc("@info", "This tool allow to export items to Twitter web-service.\n\n"
                 "See Twitter web site for details: %1",
                 QLatin1String("<a href='https://twitter.com/'>https://twitter.com/</a>"));
}

QList<DPluginAuthor> TwitterPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Thanh Trung Dinh"),
                             QString::fromUtf8("dinhthanhtrung1996 at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            << DPluginAuthor(QString::fromUtf8("Tarek Talaat"),
                             QString::fromUtf8("tarektalaat93 at gmail dot com"),
                             QString::fromUtf8("(C) 2018"))
            ;
}

void TwitterPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Twitter..."));
    ac->setObjectName(QLatin1String("export_twitter"));
    ac->setActionCategory(DPluginAction::GenericExport);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotTwitter()));

    addAction(ac);
}

void TwitterPlugin::slotTwitter()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new TwWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericTwitterPlugin
