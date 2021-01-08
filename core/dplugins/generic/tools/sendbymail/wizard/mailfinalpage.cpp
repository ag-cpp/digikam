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

#include "mailfinalpage.h"

// Qt includes

#include <QIcon>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QStyle>
#include <QTimer>
#include <QDir>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "mailwizard.h"
#include "dlayoutbox.h"
#include "digikam_debug.h"
#include "dprogresswdg.h"
#include "dhistoryview.h"
#include "mailprocess.h"

namespace DigikamGenericSendByMailPlugin
{

class Q_DECL_HIDDEN MailFinalPage::Private
{
public:

    explicit Private(QWizard* const dialog)
      : progressView(nullptr),
        progressBar(nullptr),
        complete(false),
        processor(nullptr),
        wizard(nullptr),
        settings(nullptr),
        iface(nullptr)
    {
        wizard = dynamic_cast<MailWizard*>(dialog);

        if (wizard)
        {
            iface    = wizard->iface();
            settings = wizard->settings();
        }
    }

    DHistoryView*     progressView;
    DProgressWdg*     progressBar;
    bool              complete;
    MailProcess*      processor;
    MailWizard*       wizard;
    MailSettings*     settings;
    DInfoInterface*   iface;
};

MailFinalPage::MailFinalPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title),
      d(new Private(dialog))
{
    DVBox* const vbox = new DVBox(this);
    d->progressView   = new DHistoryView(vbox);
    d->progressBar    = new DProgressWdg(vbox);

    vbox->setStretchFactor(d->progressBar, 10);
    vbox->setContentsMargins(QMargins());
    vbox->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    setPageWidget(vbox);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("mail_send")));
}

MailFinalPage::~MailFinalPage()
{
    if (d->processor)
    {
        d->processor->slotCancel();
    }

    delete d;
}

void MailFinalPage::initializePage()
{
    d->complete = false;
    emit completeChanged();
    QTimer::singleShot(0, this, SLOT(slotProcess()));
}

void MailFinalPage::slotDone()
{
    d->complete = true;
    emit completeChanged();
}

void MailFinalPage::slotProcess()
{
    if (!d->wizard)
    {
        d->progressView->addEntry(i18n("Internal Error"),
                                  DHistoryView::ErrorEntry);
        return;
    }

    d->progressView->clear();
    d->progressBar->reset();

    d->progressView->addEntry(i18n("Preparing file to export by mail..."),
                              DHistoryView::ProgressEntry);

    foreach (const QUrl& url, d->settings->inputImages)
    {
        d->settings->setMailUrl(url, QUrl());
    }

    d->progressView->addEntry(i18n("%1 input items to process", d->settings->itemsList.count()),
                                  DHistoryView::ProgressEntry);

    for (QMap<QUrl, QUrl>::const_iterator it = d->settings->itemsList.constBegin() ;
         it != d->settings->itemsList.constEnd() ; ++it)
    {
        d->progressView->addEntry(QDir::toNativeSeparators(it.key().toLocalFile()),
                                  DHistoryView::ProgressEntry);
    }


    d->progressBar->setMinimum(0);
    d->progressBar->setMaximum(d->settings->itemsList.count());

    d->processor = new MailProcess(d->settings, d->iface, this);

    connect(d->processor, SIGNAL(signalProgress(int)),
            d->progressBar, SLOT(setValue(int)));

    connect(d->processor, SIGNAL(signalMessage(QString,bool)),
            this, SLOT(slotMessage(QString,bool)));

    connect(d->processor, SIGNAL(signalDone(bool)),
            this, SLOT(slotDone()));

    d->processor->firstStage();
}

void MailFinalPage::cleanupPage()
{
    if (d->processor)
    {
        d->processor->slotCancel();
    }
}

void MailFinalPage::slotMessage(const QString& mess, bool err)
{
    d->progressView->addEntry(mess, err ? DHistoryView::ErrorEntry
                                        : DHistoryView::ProgressEntry);
}

bool MailFinalPage::isComplete() const
{
    return d->complete;
}

} // namespace DigikamGenericSendByMailPlugin
