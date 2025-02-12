/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-16
 * Description : XMP categories settings page.
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Victor Dodon <dodonvictor at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "xmpcategories.h"

// Qt includes

#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QListWidget>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "dtextedit.h"

namespace DigikamGenericMetadataEditPlugin
{

class Q_DECL_HIDDEN XMPCategories::Private
{
public:

    Private() = default;

    QStringList  oldSubCategories;

    QPushButton* addSubCategoryButton   = nullptr;
    QPushButton* delSubCategoryButton   = nullptr;
    QPushButton* repSubCategoryButton   = nullptr;

    QCheckBox*   subCategoriesCheck     = nullptr;
    QCheckBox*   categoryCheck          = nullptr;

    QLineEdit*   categoryEdit           = nullptr;
    DTextEdit*   subCategoryEdit        = nullptr;

    QListWidget* subCategoriesBox       = nullptr;
};

XMPCategories::XMPCategories(QWidget* const parent)
    :  MetadataEditPage(parent),
       d               (new Private)
{
    QGridLayout* const grid = new QGridLayout(widget());

    // --------------------------------------------------------

    d->categoryCheck = new QCheckBox(i18n("Identify subject of content (3 chars max):"), this);
    d->categoryEdit  = new QLineEdit(this);
    d->categoryEdit->setClearButtonEnabled(true);
    d->categoryEdit->setMaxLength(3);
    d->categoryEdit->setPlaceholderText(i18n("Set here the category ID"));
    d->categoryEdit->setWhatsThis(i18n("Set here the category of content. This field is limited "
                                       "to 3 characters."));

    d->subCategoriesCheck = new QCheckBox(i18n("Supplemental categories:"), this);

    d->subCategoryEdit    = new DTextEdit(this);
    d->subCategoryEdit->setLinesVisible(1);
    d->subCategoryEdit->setPlaceholderText(i18n("Set here the extra category"));
    d->subCategoryEdit->setWhatsThis(i18n("Enter here a new supplemental category of content."));

    d->subCategoriesBox = new QListWidget(this);
    d->subCategoriesBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addSubCategoryButton = new QPushButton(i18n("&Add"),     this);
    d->delSubCategoryButton = new QPushButton(i18n("&Delete"),  this);
    d->repSubCategoryButton = new QPushButton(i18n("&Replace"), this);
    d->addSubCategoryButton->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->delSubCategoryButton->setIcon(QIcon::fromTheme(QLatin1String("edit-delete")));
    d->repSubCategoryButton->setIcon(QIcon::fromTheme(QLatin1String("view-refresh")));
    d->delSubCategoryButton->setEnabled(false);
    d->repSubCategoryButton->setEnabled(false);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->categoryCheck,        0, 0, 1, 2);
    grid->addWidget(d->categoryEdit,         0, 2, 1, 1);
    grid->addWidget(d->subCategoriesCheck,   1, 0, 1, 3);
    grid->addWidget(d->subCategoryEdit,      2, 0, 1, 3);
    grid->addWidget(d->subCategoriesBox,     3, 0, 5, 3);
    grid->addWidget(d->addSubCategoryButton, 3, 3, 1, 1);
    grid->addWidget(d->delSubCategoryButton, 4, 3, 1, 1);
    grid->addWidget(d->repSubCategoryButton, 5, 3, 1, 1);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(6, 10);

    int spacing = layoutSpacing();

    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotCheckCategoryToggled(bool)));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotCheckSubCategoryToggled(bool)));

    enableWidgets(d->categoryCheck->isChecked(), d->subCategoriesCheck->isChecked());

    // --------------------------------------------------------

    connect(d->subCategoriesBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotCategorySelectionChanged()));

    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotAddCategory()));

    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotDelCategory()));

    connect(d->repSubCategoryButton, SIGNAL(clicked()),
            this, SLOT(slotRepCategory()));

    // --------------------------------------------------------

    connect(d->categoryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->subCategoriesCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repSubCategoryButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->categoryEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));
}

XMPCategories::~XMPCategories()
{
    delete d;
}

void XMPCategories::slotCheckCategoryToggled(bool checked)
{
    enableWidgets(checked, d->subCategoriesCheck->isChecked());
}

void XMPCategories::slotCheckSubCategoryToggled(bool checked)
{
    enableWidgets(d->categoryCheck->isChecked(), checked);
}

void XMPCategories::enableWidgets(bool checked1, bool checked2)
{
    d->categoryEdit->setEnabled(checked1);
    d->subCategoriesCheck->setEnabled(checked1);

    // --------------------------------------------------------

    d->subCategoryEdit->setEnabled(checked1 && checked2);
    d->subCategoriesBox->setEnabled(checked1 && checked2);
    d->addSubCategoryButton->setEnabled(checked1 && checked2);
    d->delSubCategoryButton->setEnabled(checked1 && checked2);
    d->repSubCategoryButton->setEnabled(checked1 && checked2);
}

void XMPCategories::slotDelCategory()
{
    QListWidgetItem* const item = d->subCategoriesBox->currentItem();

    if (!item)
    {
        return;
    }

    d->subCategoriesBox->takeItem(d->subCategoriesBox->row(item));
    delete item;
}

void XMPCategories::slotRepCategory()
{
    QString newCategory = d->subCategoryEdit->text();

    if (newCategory.isEmpty())
    {
        return;
    }

    if (!d->subCategoriesBox->selectedItems().isEmpty())
    {
        d->subCategoriesBox->selectedItems()[0]->setText(newCategory);
        d->subCategoryEdit->clear();
    }
}

void XMPCategories::slotCategorySelectionChanged()
{
    if (!d->subCategoriesBox->selectedItems().isEmpty())
    {
        d->subCategoryEdit->setText(d->subCategoriesBox->selectedItems()[0]->text());
        d->delSubCategoryButton->setEnabled(true);
        d->repSubCategoryButton->setEnabled(true);
    }
    else
    {
        d->delSubCategoryButton->setEnabled(false);
        d->repSubCategoryButton->setEnabled(false);
    }
}

void XMPCategories::slotAddCategory()
{
    QString newCategory = d->subCategoryEdit->text();

    if (newCategory.isEmpty())
    {
        return;
    }

    bool found = false;

    for (int i = 0 ; i < d->subCategoriesBox->count() ; ++i)
    {
        QListWidgetItem* const item = d->subCategoriesBox->item(i);

        if (newCategory == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->subCategoriesBox->insertItem(d->subCategoriesBox->count(), newCategory);
        d->subCategoryEdit->clear();
    }
}

void XMPCategories::readMetadata(const DMetadata& meta)
{
    blockSignals(true);

    QString data;

    // In first we handle all sub-categories.

    d->subCategoriesBox->clear();
    d->subCategoriesCheck->setChecked(false);
    d->oldSubCategories = meta.getXmpSubCategories();

    if (!d->oldSubCategories.isEmpty())
    {
        d->subCategoriesBox->insertItems(0, d->oldSubCategories);
        d->subCategoriesCheck->setChecked(true);
    }

    // And in second, the main category because all sub-categories status depend of this one.

    d->categoryEdit->clear();
    d->categoryCheck->setChecked(false);
    data = meta.getXmpTagString("Xmp.photoshop.Category", false);

    if (!data.isNull())
    {
        d->categoryEdit->setText(data);
        d->categoryCheck->setChecked(true);
    }

    d->categoryEdit->setEnabled(d->categoryCheck->isChecked());
    d->subCategoriesCheck->setEnabled(d->categoryCheck->isChecked());
    d->subCategoryEdit->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->subCategoriesBox->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->addSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());
    d->delSubCategoryButton->setEnabled(d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked());

    blockSignals(false);
}

void XMPCategories::applyMetadata(const DMetadata& meta)
{
    QStringList newCategories;

    if (d->categoryCheck->isChecked())
    {
        meta.setXmpTagString("Xmp.photoshop.Category", d->categoryEdit->text());
    }
    else
    {
        meta.removeXmpTag("Xmp.photoshop.Category");
    }

    for (int i = 0 ; i < d->subCategoriesBox->count() ; ++i)
    {
        QListWidgetItem* const item = d->subCategoriesBox->item(i);
        newCategories.append(item->text());
    }

    // We remove in first all existing sub-categories.

    meta.removeXmpTag("Xmp.photoshop.SupplementalCategories");

    // And add new list if necessary.

    if (d->categoryCheck->isChecked() && d->subCategoriesCheck->isChecked())
    {
        meta.setXmpSubCategories(newCategories);
    }
}

} // namespace DigikamGenericMetadataEditPlugin

#include "moc_xmpcategories.cpp"
