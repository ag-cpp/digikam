/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-28-04
 * Description : first run assistant dialog
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "firstrundlg.h"

// Qt includes

#include <QPushButton>

// Local includes

#include "dxmlguiwindow.h"
#include "welcomepage.h"
#include "migratefromdigikam4page.h"
#include "collectionpage.h"
#include "databasepage.h"
#include "rawpage.h"
#include "metadatapage.h"
#include "previewpage.h"
#include "openfilepage.h"
#include "tooltipspage.h"
#include "startscanpage.h"

namespace Digikam
{

class Q_DECL_HIDDEN FirstRunDlg::Private
{
public:

    explicit Private()
      : welcomePage(nullptr),
        migrateFromDigikam4Page(nullptr),
        collectionPage(nullptr),
        databasePage(nullptr),
        rawPage(nullptr),
        metadataPage(nullptr),
        previewPage(nullptr),
        openFilePage(nullptr),
        tooltipsPage(nullptr),
        startScanPage(nullptr)
    {
    }

    WelcomePage*             welcomePage;
    MigrateFromDigikam4Page* migrateFromDigikam4Page;
    CollectionPage*          collectionPage;
    DatabasePage*            databasePage;
    RawPage*                 rawPage;
    MetadataPage*            metadataPage;
    PreviewPage*             previewPage;
    OpenFilePage*            openFilePage;
    TooltipsPage*            tooltipsPage;
    StartScanPage*           startScanPage;
};

FirstRunDlg::FirstRunDlg(QWidget* const parent)
    : QWizard(parent),
      d(new Private)
{
    setWizardStyle(QWizard::ClassicStyle);
    setButtonLayout(QList<QWizard::WizardButton>() << QWizard::HelpButton
                                                   << QWizard::BackButton
                                                   << QWizard::CancelButton
                                                   << QWizard::NextButton
                                                   << QWizard::FinishButton);

    d->welcomePage    = new WelcomePage(this);    // First assistant page

    if (MigrateFromDigikam4Page::checkForMigration())
    {
        d->migrateFromDigikam4Page = new MigrateFromDigikam4Page(this);
    }

    d->collectionPage = new CollectionPage(this);
    d->databasePage   = new DatabasePage(this);
    d->rawPage        = new RawPage(this);
    d->metadataPage   = new MetadataPage(this);
    d->previewPage    = new PreviewPage(this);
    d->openFilePage   = new OpenFilePage(this);
    d->tooltipsPage   = new TooltipsPage(this);

    // NOTE: Added here new assistant pages...

    d->startScanPage  = new StartScanPage(this);  // Last assistant page

    resize(600, 600);

    connect(button(QWizard::FinishButton), SIGNAL(clicked()),
            this, SLOT(slotFinishPressed()));

    connect(this, SIGNAL(helpRequested()),
            this, SLOT(slotHelp()));
}

FirstRunDlg::~FirstRunDlg()
{
    delete d;
}

void FirstRunDlg::slotHelp()
{
    DXmlGuiWindow::openHandbook();
}

QString FirstRunDlg::firstAlbumPath() const
{
    return d->collectionPage->firstAlbumPath();
}

DbEngineParameters FirstRunDlg::getDbEngineParameters() const
{
    return d->databasePage->getDbEngineParameters();
}

bool FirstRunDlg::validateCurrentPage()
{
    if (currentPage() == d->collectionPage)
    {
        if (!d->collectionPage->checkSettings())
        {
            return false;
        }
        else
        {
            d->databasePage->setDatabasePath(firstAlbumPath());
        }
    }

    if (currentPage() == d->databasePage)
    {
        if (!d->databasePage->checkSettings())
        {
            return false;
        }
    }

    return true;
}

void FirstRunDlg::slotFinishPressed()
{
    if (d->migrateFromDigikam4Page && d->migrateFromDigikam4Page->isMigrationChecked())
    {
       // The user choose to do a migration from digikam4

       d->migrateFromDigikam4Page->doMigration();
    }
    else
    {
       // Save settings to rc files.

       d->collectionPage->saveSettings();
       d->databasePage->saveSettings();
       d->rawPage->saveSettings();
       d->metadataPage->saveSettings();
       d->previewPage->saveSettings();
       d->openFilePage->saveSettings();
       d->tooltipsPage->saveSettings();
    }
}

} // namespace Digikam
