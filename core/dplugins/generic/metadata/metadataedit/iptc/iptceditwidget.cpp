/*============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a DConfigDlgWdg to edit IPTC metadata
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodon dot victor at gmail dot com>
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

#include "iptceditwidget.h"

// Qt includes

#include <QMenu>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QUrl>
#include <QApplication>

// KDE includes

#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "metadataedit.h"
#include "iptccategories.h"
#include "iptccontent.h"
#include "iptccredits.h"
#include "iptcenvelope.h"
#include "iptckeywords.h"
#include "iptcorigin.h"
#include "iptcproperties.h"
#include "iptcstatus.h"
#include "iptcsubjects.h"
#include "dmetadata.h"

namespace DigikamGenericMetadataEditPlugin
{

class Q_DECL_HIDDEN IPTCEditWidget::Private
{

public:

    explicit Private()
    {
        modified        = false;
        isReadOnly      = false;
        page_content    = nullptr;
        page_properties = nullptr;
        page_subjects   = nullptr;
        page_keywords   = nullptr;
        page_categories = nullptr;
        page_credits    = nullptr;
        page_status     = nullptr;
        page_origin     = nullptr;
        page_envelope   = nullptr;
        contentPage     = nullptr;
        propertiesPage  = nullptr;
        subjectsPage    = nullptr;
        keywordsPage    = nullptr;
        categoriesPage  = nullptr;
        creditsPage     = nullptr;
        statusPage      = nullptr;
        originPage      = nullptr;
        envelopePage    = nullptr;
        dlg             = nullptr;
    }

    bool                  modified;
    bool                  isReadOnly;

    QByteArray            exifData;
    QByteArray            iptcData;

    DConfigDlgWdgItem*    page_content;
    DConfigDlgWdgItem*    page_properties;
    DConfigDlgWdgItem*    page_subjects;
    DConfigDlgWdgItem*    page_keywords;
    DConfigDlgWdgItem*    page_categories;
    DConfigDlgWdgItem*    page_credits;
    DConfigDlgWdgItem*    page_status;
    DConfigDlgWdgItem*    page_origin;
    DConfigDlgWdgItem*    page_envelope;

    IPTCContent*          contentPage;
    IPTCProperties*       propertiesPage;
    IPTCSubjects*         subjectsPage;
    IPTCKeywords*         keywordsPage;
    IPTCCategories*       categoriesPage;
    IPTCCredits*          creditsPage;
    IPTCStatus*           statusPage;
    IPTCOrigin*           originPage;
    IPTCEnvelope*         envelopePage;

    MetadataEditDialog*   dlg;
};

IPTCEditWidget::IPTCEditWidget(MetadataEditDialog* const parent)
    : DConfigDlgWdg(parent),
      d            (new Private)
{
    d->dlg           = parent;

    d->contentPage   = new IPTCContent(this);
    d->page_content  = addPage(d->contentPage, i18nc("@item: iptc", "Content"));
    d->page_content->setIcon(QIcon::fromTheme(QLatin1String("draw-text")));

    d->originPage  = new IPTCOrigin(this);
    d->page_origin = addPage(d->originPage, i18nc("@item: iptc", "Origin"));
    d->page_origin->setIcon(QIcon::fromTheme(QLatin1String("globe")));

    d->creditsPage  = new IPTCCredits(this);
    d->page_credits = addPage(d->creditsPage, i18nc("@item: iptc", "Credits"));
    d->page_credits->setIcon(QIcon::fromTheme(QLatin1String("address-book-new")));

    d->subjectsPage  = new IPTCSubjects(this);
    d->page_subjects = addPage(d->subjectsPage, i18nc("@item: iptc", "Subjects"));
    d->page_subjects->setIcon(QIcon::fromTheme(QLatin1String("feed-subscribe")));

    d->keywordsPage  = new IPTCKeywords(this);
    d->page_keywords = addPage(d->keywordsPage, i18nc("@item: iptc", "Keywords"));
    d->page_keywords->setIcon(QIcon::fromTheme(QLatin1String("bookmark-new")));

    d->categoriesPage  = new IPTCCategories(this);
    d->page_categories = addPage(d->categoriesPage, i18nc("@item: iptc", "Categories"));
    d->page_categories->setIcon(QIcon::fromTheme(QLatin1String("folder-pictures")));

    d->statusPage  = new IPTCStatus(this);
    d->page_status = addPage(d->statusPage, i18nc("@item: iptc", "Status"));
    d->page_status->setIcon(QIcon::fromTheme(QLatin1String("view-task")));

    d->propertiesPage  = new IPTCProperties(this);
    d->page_properties = addPage(d->propertiesPage, i18nc("@item: iptc", "Properties"));
    d->page_properties->setIcon(QIcon::fromTheme(QLatin1String("draw-freehand")));

    d->envelopePage  = new IPTCEnvelope(this);
    d->page_envelope = addPage(d->envelopePage, i18nc("@item: iptc", "Envelope"));
    d->page_envelope->setIcon(QIcon::fromTheme(QLatin1String("mail-mark-unread")));

    // ---------------------------------------------------------------

    connect(d->contentPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->propertiesPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->subjectsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->keywordsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->categoriesPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->creditsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->statusPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->originPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->envelopePage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    // ------------------------------------------------------------

    readSettings();
    slotItemChanged();
}

IPTCEditWidget::~IPTCEditWidget()
{
    delete d;
}

void IPTCEditWidget::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("All Metadata Edit Settings"));
    showPage(group.readEntry(QLatin1String("All IPTC Edit Page"), 0));
    d->contentPage->setCheckedSyncJFIFComment(group.readEntry(QLatin1String("All Sync JFIF Comment"), true));
    d->contentPage->setCheckedSyncEXIFComment(group.readEntry(QLatin1String("All Sync EXIF Comment"), true));
    d->originPage->setCheckedSyncEXIFDate(group.readEntry(QLatin1String("All Sync EXIF Date"), true));
}

void IPTCEditWidget::saveSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("All Metadata Edit Settings"));
    group.writeEntry(QLatin1String("All IPTC Edit Page"),    activePageIndex());
    group.writeEntry(QLatin1String("All Sync JFIF Comment"), d->contentPage->syncJFIFCommentIsChecked());
    group.writeEntry(QLatin1String("All Sync EXIF Comment"), d->contentPage->syncEXIFCommentIsChecked());
    group.writeEntry(QLatin1String("All Sync EXIF Date"),    d->originPage->syncEXIFDateIsChecked());
    config->sync();
}

void IPTCEditWidget::slotItemChanged()
{
    d->page_content->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Content Information\n"
                     "Use this panel to describe the visual content of the image")));
    d->page_origin->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Origin Information\n"
                     "Use this panel for formal descriptive information about the image")));
    d->page_credits->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Credit Information\n"
                     "Use this panel to record copyright information about the image")));
    d->page_subjects->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Subject Information\n"
                     "Use this panel to record subject information about the image")));
    d->page_keywords->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Keyword Information\n"
                     "Use this panel to record keywords relevant to the image")));
    d->page_categories->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Category Information\n"
                     "Use this panel to record categories relevant to the image")));
    d->page_status->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Status Information\n"
                     "Use this panel to record workflow information")));
    d->page_properties->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Status Properties\n"
                     "Use this panel to record workflow properties")));
    d->page_envelope->setHeader(d->dlg->currentItemTitleHeader(i18nc("@info", "Envelope Information\n"
                     "Use this panel to record editorial details")));

    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->load((*d->dlg->currentItem()).toLocalFile());

    d->exifData = meta->getExifEncoded();
    d->iptcData = meta->getIptc();
    d->contentPage->readMetadata(d->iptcData);
    d->originPage->readMetadata(d->iptcData);
    d->creditsPage->readMetadata(d->iptcData);
    d->subjectsPage->readMetadata(d->iptcData);
    d->keywordsPage->readMetadata(d->iptcData);
    d->categoriesPage->readMetadata(d->iptcData);
    d->statusPage->readMetadata(d->iptcData);
    d->propertiesPage->readMetadata(d->iptcData);
    d->envelopePage->readMetadata(d->iptcData);

    d->isReadOnly = !DMetadata::canWriteIptc((*d->dlg->currentItem()).toLocalFile());
    emit signalSetReadOnly(d->isReadOnly);

    d->page_content->setEnabled(!d->isReadOnly);
    d->page_origin->setEnabled(!d->isReadOnly);
    d->page_credits->setEnabled(!d->isReadOnly);
    d->page_subjects->setEnabled(!d->isReadOnly);
    d->page_keywords->setEnabled(!d->isReadOnly);
    d->page_categories->setEnabled(!d->isReadOnly);
    d->page_status->setEnabled(!d->isReadOnly);
    d->page_properties->setEnabled(!d->isReadOnly);
    d->page_envelope->setEnabled(!d->isReadOnly);
}

void IPTCEditWidget::apply()
{
    if (d->modified && !d->isReadOnly)
    {
        QScopedPointer<DMetadata> meta(new DMetadata);
        meta->load((*d->dlg->currentItem()).toLocalFile());

        d->exifData = meta->getExifEncoded();
        d->iptcData = meta->getIptc();

        d->contentPage->applyMetadata(d->exifData, d->iptcData);
        d->originPage->applyMetadata(d->exifData, d->iptcData);
        d->creditsPage->applyMetadata(d->iptcData);
        d->subjectsPage->applyMetadata(d->iptcData);
        d->keywordsPage->applyMetadata(d->iptcData);
        d->categoriesPage->applyMetadata(d->iptcData);
        d->statusPage->applyMetadata(d->iptcData);
        d->propertiesPage->applyMetadata(d->iptcData);
        d->envelopePage->applyMetadata(d->iptcData);

        meta->setExif(d->exifData);
        meta->setIptc(d->iptcData);
        meta->save((*d->dlg->currentItem()).toLocalFile());

        d->modified = false;
    }
}

void IPTCEditWidget::slotModified()
{
    if (!d->isReadOnly)
    {
        d->modified = true;
        emit signalModified();
    }
}

void IPTCEditWidget::showPage(int page)
{
    switch (page)
    {
        case 0:
            setCurrentPage(d->page_content);
            break;
        case 1:
            setCurrentPage(d->page_origin);
            break;
        case 2:
            setCurrentPage(d->page_credits);
            break;
        case 3:
            setCurrentPage(d->page_subjects);
            break;
        case 4:
            setCurrentPage(d->page_keywords);
            break;
        case 5:
            setCurrentPage(d->page_categories);
            break;
        case 6:
            setCurrentPage(d->page_status);
            break;
        case 7:
            setCurrentPage(d->page_properties);
            break;
        case 8:
            setCurrentPage(d->page_envelope);
            break;
        default:
            setCurrentPage(d->page_content);
            break;
    }
}

int IPTCEditWidget::activePageIndex() const
{
    DConfigDlgWdgItem* const cur = currentPage();

    if (cur == d->page_content)    return 0;
    if (cur == d->page_origin)     return 1;
    if (cur == d->page_credits)    return 2;
    if (cur == d->page_subjects)   return 3;
    if (cur == d->page_keywords)   return 4;
    if (cur == d->page_categories) return 5;
    if (cur == d->page_status)     return 6;
    if (cur == d->page_properties) return 7;
    if (cur == d->page_envelope)   return 8;

    return 0;
}

bool IPTCEditWidget::isModified() const
{
    return d->modified;
}

} // namespace DigikamGenericMetadataEditPlugin
