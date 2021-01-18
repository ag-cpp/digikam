/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-14
 * Description : a plugin to export data from digikam's database
 *
 * Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "dbexportplugin.h"

// Qt includes

#include <QPointer>
#include <QMenu>
#include <QDebug>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <kxmlguiwindow.h>

using namespace Digikam;

namespace DigikamDbExportPlugin
{

DbExportPlugin::DbExportPlugin(QObject* const parent)
    : DPluginDb(parent)
{
}

DbExportPlugin::~DbExportPlugin()
{
}

QString DbExportPlugin::name() const
{
    return i18n("DatabaseExport");
}

QString DbExportPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DbExportPlugin::icon() const
{
    // TODO select icon
    return QIcon::fromTheme(QLatin1String("view-presentation"));
}

QString DbExportPlugin::description() const
{
    return i18n("A tool to export data from digikam's database");
}

QString DbExportPlugin::details() const
{
    return i18n("<p>This tool exports chosen data from digikam databases to a text file.</p>"
                "<p>Data will be exported in csv format, separated by comma.</p>"
                "<p>Exporting data can be chosen by its name in corresponding database.</p>");
}

QList<DPluginAuthor> DbExportPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Nghia Duong"),
                             QString::fromUtf8("minhnghiaduong997 at gmail dot com"),
                             QString::fromUtf8("(C) 2019-2021"))
            ;
}

void DbExportPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Database Export"));
    ac->setObjectName(QLatin1String("dbexport_plugin"));

    // TODO setup plugin
    /*
    ac->setActionCategory(DPluginAction::GenericView);
    DInfoInterface* const iface = infoIface(ac);

    if (iface && (parent->objectName() == QLatin1String("Digikam")))
    {

        QMenu* const slideShowActions = new QMenu(i18n("Slideshow"));
        slideShowActions->setIcon(icon());
        ac->setMenu(slideShowActions);

        // Action show all

        QAction* const slideShowAllAction = new QAction(i18n("Play All"), ac);
        slideShowAllAction->setObjectName(QLatin1String("slideshow_all"));
        slideShowActions->addAction(slideShowAllAction);

        connect(slideShowAllAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowAll()));

        // Action show selection

        QAction* const slideShowSelectionAction = new QAction(i18n("Play Selection"), ac);
        slideShowSelectionAction->setObjectName(QLatin1String("slideshow_selected"));
        slideShowActions->addAction(slideShowSelectionAction);

        connect(slideShowSelectionAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowSelection()));

        // Action show recursive

        QAction* const slideShowRecursiveAction = new QAction(i18n("Play With Sub-Albums"), ac);
        slideShowRecursiveAction->setObjectName(QLatin1String("slideshow_recursive"));
        slideShowActions->addAction(slideShowRecursiveAction);

        connect(slideShowRecursiveAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowRecursive()));

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotShowManual()));

        // See bug #425425: register all sub-actions to collection instance to be able to edit keyboard shorcuts

        KXmlGuiWindow* const gui = dynamic_cast<KXmlGuiWindow*>(parent);

        if (gui)
        {
            KActionCollection* const collection = gui->actionCollection();
            collection->setShortcutsConfigurable(slideShowActions->menuAction(), false);

            collection->addAction(slideShowAllAction->objectName(),       slideShowAllAction);
            collection->addAction(slideShowSelectionAction->objectName(), slideShowSelectionAction);
            collection->addAction(slideShowRecursiveAction->objectName(), slideShowRecursiveAction);

            collection->setDefaultShortcut(slideShowAllAction,       Qt::Key_F9);
            collection->setDefaultShortcut(slideShowSelectionAction, Qt::ALT + Qt::Key_F9);
            collection->setDefaultShortcut(slideShowRecursiveAction, Qt::SHIFT + Qt::Key_F9);
        }
    }
    else
    {
        ac->setShortcut(Qt::Key_F9);

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotMenuSlideShow()));
    }
    */

    //addTool(ac);
}

} // namespace DigikamDbExportPlugin
