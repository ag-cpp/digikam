/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to render slideshow.
 *
 * Copyright (C) 2018-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "slideshowplugin.h"

// Qt includes

#include <QPointer>
#include <QMenu>
#include <QDebug>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaenginesettings.h"
#include "setupslideshow_dialog.h"
#include "slideshowloader.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

SlideShowPlugin::SlideShowPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

SlideShowPlugin::~SlideShowPlugin()
{
}

QString SlideShowPlugin::name() const
{
    return i18n("SlideShow");
}

QString SlideShowPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon SlideShowPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("view-presentation"));
}

QString SlideShowPlugin::description() const
{
    return i18n("A tool to render slideshow");
}

QString SlideShowPlugin::details() const
{
    return i18n("<p>This tool render a series of items as a simple slide-show.</p>"
                "<p>Plenty of items properties can be displayed as overlay while running.</p>"
                "<p>This tool can play album contents in recursive mode with children albums if any.</p>");
}

QList<DPluginAuthor> SlideShowPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Enrico Ros"),
                             QString::fromUtf8("eros dot kde at email dot it"),
                             QString::fromUtf8("(C) 2004"))
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2004-2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2020"))
            << DPluginAuthor(QString::fromUtf8("Minh Nghia Duong"),
                             QString::fromUtf8("minhnghiaduong997 at gmail dot com"),
                             QString::fromUtf8("(C) 2019-2020"))
            ;
}

void SlideShowPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac     = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Slideshow"));
    ac->setObjectName(QLatin1String("slideshow_plugin"));
    ac->setActionCategory(DPluginAction::GenericView);

    DInfoInterface* const iface = infoIface(ac);

    if (iface && (parent->objectName() == QLatin1String("Digikam")))
    {
        QMenu* const slideShowActions = new QMenu(i18n("Slideshow"), nullptr);
        slideShowActions->setIcon(icon());
        ac->setMenu(slideShowActions);

        // Action show all

        QAction* const slideShowAllAction = new QAction(i18n("All"), ac);
        slideShowAllAction->setObjectName(QLatin1String("slideshow_all"));
        slideShowAllAction->setShortcut(Qt::Key_F9);
        slideShowActions->addAction(slideShowAllAction);

        connect(slideShowAllAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowAll()));

        connect(slideShowActions->menuAction(), SIGNAL(triggered()),
                slideShowAllAction, SLOT(trigger()));

        // Action show selection

        QAction* const slideShowSelectionAction = new QAction(i18n("Selection"), ac);
        slideShowSelectionAction->setObjectName(QLatin1String("slideshow_selected"));
        slideShowSelectionAction->setShortcut(Qt::ALT + Qt::Key_F9);
        slideShowActions->addAction(slideShowSelectionAction);

        connect(slideShowSelectionAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowSelection()));

        // Action show recursive

        QAction* const slideShowRecursiveAction = new QAction(i18n("With All Sub-Albums"), ac);
        slideShowRecursiveAction->setObjectName(QLatin1String("slideshow_recursive"));
        slideShowRecursiveAction->setShortcut(Qt::SHIFT + Qt::Key_F9);
        slideShowActions->addAction(slideShowRecursiveAction);

        connect(slideShowRecursiveAction, SIGNAL(triggered()),
                this, SLOT(slotMenuSlideShowRecursive()));

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotShowManual()));
    }
    else
    {
        ac->setShortcut(Qt::Key_F9);

        connect(ac, SIGNAL(triggered(bool)),
                this, SLOT(slotMenuSlideShow()));
    }

    addAction(ac);
}

void SlideShowPlugin::addConnectionSlideEnd(QObject* const obj)
{
    Q_UNUSED(obj);
}

void SlideShowPlugin::slotMenuSlideShow()
{
    QUrl startFrom;
    DPluginAction* const ac = dynamic_cast<DPluginAction*>(sender());

    if (ac)
    {
        startFrom = ac->data().toUrl();
        ac->setData(QVariant());
    }

    SlideShowSettings* const settings = new SlideShowSettings();
    settings->iface                   = infoIface(ac);
    settings->readFromConfig();
    settings->exifRotate              = MetaEngineSettings::instance()->settings().exifRotate;
    settings->fileList                = settings->iface->currentAlbumItems();

    slideshow(settings, true, startFrom);
}

void SlideShowPlugin::slotMenuSlideShowAll()
{
    SlideShowSettings* const settings = new SlideShowSettings();
    settings->iface                   = infoIface(sender()->parent());
    settings->readFromConfig();
    settings->fileList                = settings->iface->currentAlbumItems();

    slideshow(settings);
}

void SlideShowPlugin::slotMenuSlideShowSelection()
{
    SlideShowSettings* const settings = new SlideShowSettings();
    settings->iface                   = infoIface(sender()->parent());
    settings->readFromConfig();
    settings->fileList                = settings->iface->currentSelectedItems();

    slideshow(settings);
}

void SlideShowPlugin::slotMenuSlideShowRecursive()
{
    DInfoInterface* const iface = infoIface(sender()->parent());

    connect(iface, SIGNAL(signalAlbumItemsRecursiveCompleted(QList<QUrl>)),
            this, SLOT(slotShowRecursive(QList<QUrl>)));

    iface->parseAlbumItemsRecursive();
}

void SlideShowPlugin::slotShowRecursive(const QList<QUrl>& imageList)
{
    SlideShowSettings* const settings = new SlideShowSettings();
    settings->iface                   = dynamic_cast<DInfoInterface*>(sender());
    settings->readFromConfig();
    settings->fileList                = imageList;

    slideshow(settings);
}

void SlideShowPlugin::slotShowManual()
{
    DPluginAction* const ac = dynamic_cast<DPluginAction*>(sender());

    QUrl startFrom;

    if (ac)
    {
        startFrom = ac->data().toUrl();

        ac->setData(QVariant());

        if (!startFrom.isValid())
        {
            return;
        }
    }
    else
    {
        return;
    }

    SlideShowSettings* const settings = new SlideShowSettings();
    settings->iface                   = infoIface(ac);
    settings->readFromConfig();
    settings->exifRotate              = MetaEngineSettings::instance()->settings().exifRotate;
    settings->fileList                = settings->iface->currentAlbumItems();

    slideshow(settings, false, startFrom);
}

void SlideShowPlugin::slideshow(SlideShowSettings* const settings, bool autoPlayEnabled, const QUrl& startFrom)
{
    settings->autoPlayEnabled = autoPlayEnabled;
    settings->plugin          = this;

    // TODO: preview settings for digikam
    //settings.previewSettings = ApplicationSettings::instance()->getPreviewSettings();

    if (startFrom.isValid())
    {
        settings->imageUrl = startFrom;
    }

    SlideShowLoader* const slide = new SlideShowLoader(settings);
    slide->setShortCutPrefixes(settings->iface->passShortcutActionsToWidget(slide));

    if     (settings->imageUrl.isValid())
    {
        slide->setCurrentItem(settings->imageUrl);
    }
    else if (settings->startWithCurrent)
    {
        if (!settings->iface->currentSelectedItems().isEmpty())
        {
            slide->setCurrentItem(settings->iface->currentSelectedItems()[0]);
        }
        else
        {
            // no current selection, do nothing.
            return;
        }
    }

    connect(slide, SIGNAL(signalLastItemUrl(QUrl)),
            settings->iface, SIGNAL(signalLastItemUrl(QUrl)));

    connect(settings->iface, SIGNAL(signalShortcutPressed(QString,int)),
            slide, SLOT(slotHandleShortcut(QString,int)));

    slide->show();
}

} // namespace DigikamGenericSlideShowPlugin
