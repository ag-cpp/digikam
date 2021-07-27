/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-15
 * Description : IPTC keywords settings page.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iptckeywords.h"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QListWidget>
#include <QToolTip>

// KDE includes

#include <klocalizedstring.h>

// local includes

#include "dmetadata.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class Q_DECL_HIDDEN IPTCKeywords::Private
{
public:

    explicit Private()
    {
        addKeywordButton = nullptr;
        delKeywordButton = nullptr;
        repKeywordButton = nullptr;
        keywordsBox      = nullptr;
        keywordsCheck    = nullptr;
        keywordsEdit     = nullptr;
    }

    QStringList  oldKeywords;

    QPushButton* addKeywordButton;
    QPushButton* delKeywordButton;
    QPushButton* repKeywordButton;

    QCheckBox*   keywordsCheck;

    QLineEdit*   keywordsEdit;

    QListWidget* keywordsBox;
};

IPTCKeywords::IPTCKeywords(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    QGridLayout* const grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->keywordsCheck = new QCheckBox(i18n("Use information retrieval words:"), this);

    d->keywordsEdit   = new QLineEdit(this);
    d->keywordsEdit->setClearButtonEnabled(true);
    d->keywordsEdit->setMaxLength(64);
    d->keywordsEdit->setWhatsThis(i18n("Enter here a new keyword. "
                                      "This field is limited to 64 characters."));

    d->keywordsBox   = new QListWidget(this);
    d->keywordsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addKeywordButton = new QPushButton( i18n("&Add"), this);
    d->delKeywordButton = new QPushButton( i18n("&Delete"), this);
    d->repKeywordButton = new QPushButton( i18n("&Replace"), this);
    d->addKeywordButton->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->delKeywordButton->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
    d->repKeywordButton->setIcon(QIcon::fromTheme(QLatin1String("view-refresh")));
    d->delKeywordButton->setEnabled(false);
    d->repKeywordButton->setEnabled(false);

    // --------------------------------------------------------

    QLabel* const note = new QLabel(i18n("<b>Note: "
                 "<a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> "
                 "text tags are limited string sizes. Use contextual help for details. "
                 "Consider to use <a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> instead.</b>"),
                 this);
    note->setMaximumWidth(150);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->keywordsCheck,       0, 0, 1, 2);
    grid->addWidget(d->keywordsEdit,         1, 0, 1, 1);
    grid->addWidget(d->keywordsBox,         2, 0, 5, 1);
    grid->addWidget(d->addKeywordButton,    2, 1, 1, 1);
    grid->addWidget(d->delKeywordButton,    3, 1, 1, 1);
    grid->addWidget(d->repKeywordButton,    4, 1, 1, 1);
    grid->addWidget(note,                   5, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(6, 10);
    grid->setContentsMargins(QMargins());
    grid->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    // --------------------------------------------------------

    connect(d->keywordsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotKeywordSelectionChanged()));

    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotAddKeyword()));

    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotDelKeyword()));

    connect(d->repKeywordButton, SIGNAL(clicked()),
            this, SLOT(slotRepKeyword()));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordsEdit, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->addKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->delKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->repKeywordButton, SLOT(setEnabled(bool)));

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            d->keywordsBox, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->keywordsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repKeywordButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->keywordsEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));
}

IPTCKeywords::~IPTCKeywords()
{
    delete d;
}

void IPTCKeywords::slotDelKeyword()
{
    QListWidgetItem* const item = d->keywordsBox->currentItem();
    if (!item) return;

    d->keywordsBox->takeItem(d->keywordsBox->row(item));
    delete item;
}

void IPTCKeywords::slotRepKeyword()
{
    QString newKeyword = d->keywordsEdit->text();
    if (newKeyword.isEmpty()) return;

    if (!d->keywordsBox->selectedItems().isEmpty())
    {
        d->keywordsBox->selectedItems()[0]->setText(newKeyword);
        d->keywordsEdit->clear();
    }
}

void IPTCKeywords::slotKeywordSelectionChanged()
{
    if (!d->keywordsBox->selectedItems().isEmpty())
    {
        d->keywordsEdit->setText(d->keywordsBox->selectedItems()[0]->text());
        d->delKeywordButton->setEnabled(true);
        d->repKeywordButton->setEnabled(true);
    }
    else
    {
        d->delKeywordButton->setEnabled(false);
        d->repKeywordButton->setEnabled(false);
    }
}

void IPTCKeywords::slotAddKeyword()
{
    QString newKeyword = d->keywordsEdit->text();
    if (newKeyword.isEmpty()) return;

    bool found = false;

    for (int i = 0 ; i < d->keywordsBox->count(); ++i)
    {
        QListWidgetItem* const item = d->keywordsBox->item(i);

        if (newKeyword == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->keywordsBox->insertItem(d->keywordsBox->count(), newKeyword);
        d->keywordsEdit->clear();
    }
}

void IPTCKeywords::slotLineEditModified()
{
    QLineEdit* const ledit = dynamic_cast<QLineEdit*>(sender());

    if (!ledit)
    {
        return;
    }

    QToolTip::showText(ledit->mapToGlobal(QPoint(0, (-1)*(ledit->height() + 16))),
                       i18np("%1 character left", "%1 characters left", ledit->maxLength() - ledit->text().size()),
                       ledit);
}

void IPTCKeywords::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);
    d->oldKeywords = meta->getIptcKeywords();

    d->keywordsBox->clear();
    d->keywordsCheck->setChecked(false);

    if (!d->oldKeywords.isEmpty())
    {
        d->keywordsBox->insertItems(0, d->oldKeywords);
        d->keywordsCheck->setChecked(true);
    }

    d->keywordsEdit->setEnabled(d->keywordsCheck->isChecked());
    d->keywordsBox->setEnabled(d->keywordsCheck->isChecked());
    d->addKeywordButton->setEnabled(d->keywordsCheck->isChecked());
    d->delKeywordButton->setEnabled(d->keywordsCheck->isChecked());

    blockSignals(false);
}

void IPTCKeywords::applyMetadata(QByteArray& iptcData)
{
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);
    QStringList newKeywords;

    for (int i = 0 ; i < d->keywordsBox->count(); ++i)
    {
        QListWidgetItem* const item = d->keywordsBox->item(i);
        newKeywords.append(item->text());
    }

    if (d->keywordsCheck->isChecked())
        meta->setIptcKeywords(d->oldKeywords, newKeywords);
    else
        meta->setIptcKeywords(d->oldKeywords, QStringList());

    iptcData = meta->getIptc();
}

} // namespace DigikamGenericMetadataEditPlugin
