/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to export items by email.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mailwizard.h"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QApplication>
#include <QComboBox>
#include <QListWidget>
#include <QTextBrowser>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "dwizardpage.h"
#include "digikam_debug.h"
#include "mailintropage.h"
#include "mailalbumspage.h"
#include "mailimagespage.h"
#include "mailsettingspage.h"
#include "mailfinalpage.h"

namespace DigikamGenericSendByMailPlugin
{

class Q_DECL_HIDDEN MailWizard::Private
{
public:

    explicit Private()
      : iface(nullptr),
        introPage(nullptr),
        albumsPage(nullptr),
        imagesPage(nullptr),
        settingsPage(nullptr),
        finalPage(nullptr),
        settings(nullptr)
    {
    }

    DInfoInterface*     iface;
    MailIntroPage*      introPage;
    MailAlbumsPage*     albumsPage;
    MailImagesPage*     imagesPage;
    MailSettingsPage*   settingsPage;
    MailFinalPage*      finalPage;
    MailSettings*       settings;
};

MailWizard::MailWizard(QWidget* const parent, DInfoInterface* const iface)
    : DWizardDlg(parent, QLatin1String("Email Dialog")),
      d(new Private)
{
    setOption(QWizard::NoCancelButtonOnLastPage);
    setWindowTitle(i18n("Send by Email"));

    d->iface             = iface;
    d->settings          = new MailSettings;

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group("Email Dialog Settings");
    d->settings->readSettings(group);

    d->introPage         = new MailIntroPage(this,    i18n("Welcome to Email Tool"));
    d->albumsPage        = new MailAlbumsPage(this,   i18n("Albums Selection"));
    d->imagesPage        = new MailImagesPage(this,   i18n("Images List"));
    d->settingsPage      = new MailSettingsPage(this, i18n("Email Settings"));
    d->finalPage         = new MailFinalPage(this,    i18n("Export by Email"));
}

MailWizard::~MailWizard()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group("Email Dialog Settings");
    d->settings->writeSettings(group);

    delete d;
}

void MailWizard::setItemsList(const QList<QUrl>& urls)
{
    d->imagesPage->setItemsList(urls);
}

DInfoInterface* MailWizard::iface() const
{
    return d->iface;
}

MailSettings* MailWizard::settings() const
{
    return d->settings;
}

bool MailWizard::validateCurrentPage()
{
    if (!DWizardDlg::validateCurrentPage())
    {
        return false;
    }

    return true;
}

int MailWizard::nextId() const
{
    if (d->settings->selMode == MailSettings::ALBUMS)
    {
        if (currentPage() == d->introPage)
        {
            return d->albumsPage->id();
        }
    }
    else
    {
        if (currentPage() == d->introPage)
        {
            return d->imagesPage->id();
        }
    }

    return DWizardDlg::nextId();
}

} // namespace DigikamGenericSendByMailPlugin
