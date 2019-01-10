/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to FaceBook web-service.
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

#include "fbplugin.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "fbwindow.h"

namespace Digikam
{

FbPlugin::FbPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

FbPlugin::~FbPlugin()
{
    delete m_toolDlg;
}

QString FbPlugin::name() const
{
    return i18n("FaceBook");
}

QString FbPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon FbPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-facebook"));
}

QString FbPlugin::description() const
{
    return i18n("A tool to export to FaceBook web-service");
}

QString FbPlugin::details() const
{
    return i18n("<p>This tool permit to export items to FaceBook web-service.</p>"
                "<p>See FaceBook web site for details: <a href='https://www.facebook.com/'>https://www.facebook.com/</a></p>");
}

QList<DPluginAuthor> FbPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QLatin1String("Gilles Caulier"),
                             QLatin1String("caulier dot gilles at gmail dot com"),
                             QLatin1String("(C) 2012-2019"))
            << DPluginAuthor(QLatin1String("Maik Qualmann"),
                             QLatin1String("metzpinguin at gmail dot com"),
                             QLatin1String("(C) 2017-2019"))
            << DPluginAuthor(QLatin1String("Luka Renko"),
                             QLatin1String("lure at kubuntu dot org"),
                             QLatin1String("(C) 2009"))
            << DPluginAuthor(QLatin1String("Vardhman Jain"),
                             QLatin1String("vardhman at gmail dot com"),
                             QLatin1String("(C) 2005-2008"))
            ;
}

void FbPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &FaceBook..."));
    ac->setObjectName(QLatin1String("export_facebook"));
    ac->setActionCategory(DPluginAction::GenericExport);
    ac->setShortcut(Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_F);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotFaceBook()));

    addAction(ac);
}

void FbPlugin::slotFaceBook()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        // This message is not translated, and it will be removed after the review process.
        QMessageBox::warning(0, QLatin1String("Facebook"),
                                QLatin1String("This Facebook export tool is under "
                                              "review process and only works for "
                                              "registered test users."));

        delete m_toolDlg;
        m_toolDlg = new FbWindow(infoIface(sender()), 0);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace Digikam
