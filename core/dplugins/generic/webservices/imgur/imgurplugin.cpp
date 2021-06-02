/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to ImgUr web-service.
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

#include "imgurplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "imgurwindow.h"

namespace DigikamGenericImgUrPlugin
{

ImgUrPlugin::ImgUrPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

ImgUrPlugin::~ImgUrPlugin()
{
}

void ImgUrPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString ImgUrPlugin::name() const
{
    return i18nc("@title", "ImgUr");
}

QString ImgUrPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ImgUrPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-imgur"));
}

QString ImgUrPlugin::description() const
{
    return i18nc("@info", "A tool to export to ImgUr web-service");
}

QString ImgUrPlugin::details() const
{
    return i18nc("@info", "This tool allow to export items to ImgUr web-service.\n\n"
                 "You can export items as anonymous or with an user account.\n\n"
                 "See ImgUr web site for details: %1",
                 QLatin1String("<a href='https://imgur.com/'>https://imgur.com/</a>"));
}

QList<DPluginAuthor> ImgUrPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Marius Orcsik"),
                             QString::fromUtf8("marius at habarnam dot ro"),
                             QString::fromUtf8("(C) 2012-2013"))
            << DPluginAuthor(QString::fromUtf8("Fabian Vogt"),
                             QString::fromUtf8("fabian at ritter dash vogt dot de"),
                             QString::fromUtf8("(C) 2014"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2017-2021"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2012-2021"))
            ;
}

void ImgUrPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Imgur..."));
    ac->setObjectName(QLatin1String("export_imgur"));
    ac->setActionCategory(DPluginAction::GenericExport);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotImgUr()));

    addAction(ac);
}

void ImgUrPlugin::slotImgUr()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new ImgurWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericImgUrPlugin
