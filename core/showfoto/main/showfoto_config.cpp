/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : stand alone digiKam image editor - Configure
 *
 * Copyright (C) 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfoto_p.h"

namespace ShowFoto
{

void Showfoto::slotSetup()
{
    setup(false);
}

void Showfoto::slotSetupICC()
{
    setup(true);
}

bool Showfoto::setup(bool iccSetupPage)
{
    QPointer<ShowfotoSetup> setup = new ShowfotoSetup(this, iccSetupPage ? ShowfotoSetup::ICCPage : ShowfotoSetup::LastPageUsed);

    if (setup->exec() != QDialog::Accepted)
    {
        delete setup;
        return false;
    }

    KSharedConfig::openConfig()->sync();

    applySettings();

    if (d->itemsNb == 0)
    {
        slotUpdateItemInfo();
        toggleActions(false);
    }

    delete setup;

    return true;
}

void Showfoto::readSettings()
{
    d->settings        = ShowfotoSettings::instance();

    readStandardSettings();

    QString defaultDir = d->settings->getLastOpenedDir();

    if (defaultDir.isNull())
    {
        defaultDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }

    d->lastOpenedDirectory = QUrl::fromLocalFile(defaultDir);

    d->rightSideBar->loadState();
    d->leftSideBar->loadState();
    d->folderView->loadState();
    d->stackView->loadState();

    Digikam::ThemeManager::instance()->setCurrentTheme(d->settings->getCurrentTheme());

    d->thumbBar->setToolTipEnabled(d->settings->getShowToolTip());
}

void Showfoto::saveSettings()
{
    saveStandardSettings();

    d->settings->setLastOpenedDir(d->lastOpenedDirectory.toLocalFile());
    d->settings->setCurrentTheme(Digikam::ThemeManager::instance()->currentThemeName());
    d->settings->syncConfig();

    d->rightSideBar->saveState();
    d->leftSideBar->saveState();
    d->folderView->saveState();
    d->stackView->saveState();
}

void Showfoto::applySettings()
{
    applyStandardSettings();

    d->settings->readSettings();

    d->rightSideBar->setStyle(d->settings->getRightSideBarStyle() == 0 ?
                              DMultiTabBar::ActiveIconText : DMultiTabBar::AllIconsText);
    d->leftSideBar->setStyle(d->settings->getRightSideBarStyle() == 0 ?
                             DMultiTabBar::ActiveIconText : DMultiTabBar::AllIconsText);

    QString currentStyle = qApp->style()->objectName();
    QString newStyle     = d->settings->getApplicationStyle();

    if (currentStyle.compare(newStyle, Qt::CaseInsensitive) != 0)
    {
        qApp->setStyle(newStyle);
        qApp->style()->polish(qApp);
        qCDebug(DIGIKAM_SHOWFOTO_LOG) << "Switch to widget style: " << newStyle;
    }

    m_fileDeleteAction->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
    m_fileDeleteAction->setText(i18n("Delete File"));

    d->thumbBar->setToolTipEnabled(d->settings->getShowToolTip());

    d->rightSideBar->slotLoadMetadataFilters();

    applySortSettings();
}

void Showfoto::applySortSettings()
{
    if (d->settings->getReverseSort())
    {
        d->filterModel->setSortOrder(ShowfotoItemSortSettings::DescendingOrder);
        d->folderView->setSortOrder(ShowfotoItemSortSettings::DescendingOrder);
        d->stackView->setSortOrder(ShowfotoItemSortSettings::DescendingOrder);
    }
    else
    {
        d->filterModel->setSortOrder(ShowfotoItemSortSettings::AscendingOrder);
        d->folderView->setSortOrder(ShowfotoItemSortSettings::AscendingOrder);
        d->stackView->setSortOrder(ShowfotoItemSortSettings::AscendingOrder);
    }

    switch (d->settings->getSortRole())
    {
        case ShowfotoSetupMisc::SortByName:
        {
            d->filterModel->setSortRole(ShowfotoItemSortSettings::SortByFileName);
            d->folderView->setSortRole(ShowfotoFolderViewList::FileName);
            d->stackView->setSortRole(ShowfotoStackViewList::FileName);
            break;
        }

        case ShowfotoSetupMisc::SortByFileSize:
        {
            d->filterModel->setSortRole(ShowfotoItemSortSettings::SortByFileSize);
            d->folderView->setSortRole(ShowfotoFolderViewList::FileSize);
            d->stackView->setSortRole(ShowfotoStackViewList::FileSize);
            break;
        }

        default:
        {
            d->filterModel->setSortRole(ShowfotoItemSortSettings::SortByCreationDate);
            d->folderView->setSortRole(ShowfotoFolderViewList::FileDate);
            d->stackView->setSortRole(ShowfotoStackViewList::FileDate);
            break;
        }
    }
}

void Showfoto::slotSetupMetadataFilters(int tab)
{
    ShowfotoSetup::execMetadataFilters(this, tab+1);
    d->rightSideBar->slotLoadMetadataFilters();
}

void Showfoto::slotSetupExifTool()
{
    ShowfotoSetup::execExifTool(this);
}

} // namespace ShowFoto
