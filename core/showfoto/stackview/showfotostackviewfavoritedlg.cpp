/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorite edit dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotostackviewfavoritedlg.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QLineEdit>
#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QLocale>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

#ifdef HAVE_KICONTHEMES
#   include <kicondialog.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "drawdecoder.h"
#include "showfotostackviewfavoritelist.h"
#include "showfotostackviewfavoriteitem.h"
#include "showfotostackviewlist.h"
#include "showfotoitemsortsettings.h"
#include "itempropertiestab.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavoriteDlg::Private
{
public:

    explicit Private()
      : create         (false),
        topLabel       (nullptr),
        icon           (QLatin1String("folder-favorites")),
        iconButton     (nullptr),
        resetIconButton(nullptr),
        buttons        (nullptr),
        nameEdit       (nullptr),
        descEdit       (nullptr),
        dateEdit       (nullptr),
        urlsEdit       (nullptr),
        nbImagesLabel  (nullptr),
        list           (nullptr),
        pitem          (nullptr)
    {
    }

    bool                                        create;

    QLabel*                                     topLabel;

    QString                                     icon;

    QPushButton*                                iconButton;
    QPushButton*                                resetIconButton;

    QDialogButtonBox*                           buttons;

    QLineEdit*                                  nameEdit;
    QLineEdit*                                  descEdit;
    QDateTimeEdit*                              dateEdit;
    DItemsList*                                 urlsEdit;
    QLabel*                                     nbImagesLabel;
    ShowfotoStackViewFavoriteList*              list;
    ShowfotoStackViewFavoriteBase*              pitem;
};

static Qt::SortOrder                        s_sortOrder(Qt::AscendingOrder);
static ShowfotoStackViewList::StackViewRole s_sortRole(ShowfotoStackViewList::FileName);

ShowfotoStackViewFavoriteDlg::ShowfotoStackViewFavoriteDlg(ShowfotoStackViewFavoriteList* const list,
                                                           bool create)
    : QDialog(list),
      d      (new Private)
{
    setModal(true);

    d->create  = create;
    d->list    = list;
    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    if (d->create)
    {
        setWindowTitle(i18nc("@title", "New Favorite"));
    }
    else
    {
        setWindowTitle(i18nc("@title", "Edit Favorite"));
    }

    QWidget* const page     = new QWidget(this);

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(page);

    d->topLabel             = new QLabel(page);
    d->topLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->topLabel->setWordWrap(false);

    // --------------------------------------------------------

    QLabel* const nameLabel = new QLabel(page);
    nameLabel->setText(i18nc("@label: favorite title properties", "&Name:"));

    d->nameEdit             = new QLineEdit(page);
    d->nameEdit->setPlaceholderText(i18nc("@info", "Enter favorite name here..."));
    d->nameEdit->setToolTip(i18nc("@info", "The favorite name which must be unique and not empty"));
    nameLabel->setBuddy(d->nameEdit);

    // --------------------------------------------------------

    QLabel* const descLabel = new QLabel(page);
    descLabel->setText(i18nc("@label: favorite caption properties", "&Description:"));

    d->descEdit             = new QLineEdit(page);
    d->descEdit->setPlaceholderText(i18nc("@info", "Enter favorite description here..."));
    descLabel->setBuddy(d->descEdit);

    // --------------------------------------------------------

    QLabel* const dateLabel = new QLabel(page);
    dateLabel->setText(i18nc("@label: favorite date properties", "&Created:"));

    d->dateEdit             = new QDateTimeEdit(QDate::currentDate(), page);
    d->dateEdit->setMinimumDate(QDate(1970, 1, 1));
    d->dateEdit->setMaximumDate(QDate::currentDate().addDays(365));
    d->dateEdit->setDisplayFormat(QLatin1String("yyyy.MM.dd"));
    d->dateEdit->setCalendarPopup(true);
    dateLabel->setBuddy(d->dateEdit);

    // --------------------------------------------------------

    QLabel* const iconTextLabel = new QLabel(page);
    iconTextLabel->setText(i18nc("@label", "&Icon:"));

    d->iconButton               = new QPushButton(page);
    d->iconButton->setFixedSize(40, 40);
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
    iconTextLabel->setBuddy(d->iconButton);

    d->resetIconButton          = new QPushButton(QIcon::fromTheme(QLatin1String("view-refresh")),
                                                  i18nc("@action:button", "Reset"), page);

#ifndef HAVE_KICONTHEMES

    iconTextLabel->hide();
    d->iconButton->hide();
    d->resetIconButton->hide();

#endif

    // --------------------------------------------------------

    QLabel* const urlsLabel = new QLabel(page);
    urlsLabel->setText(i18nc("@label: favorites properties", "&Items:"));

    d->nbImagesLabel        = new QLabel(page);
    QFont fnt;
    fnt.setItalic(true);
    d->nbImagesLabel->setFont(fnt);

    d->urlsEdit             = new DItemsList(page);
    d->urlsEdit->setIsLessThanHandler(myIsLessThanHandler);
    d->urlsEdit->setIconSize(d->list->iconSize().width());
    d->urlsEdit->setAllowRAW(true);
    d->urlsEdit->setAllowDuplicate(false);
    d->urlsEdit->setControlButtonsPlacement(DItemsList::ControlButtonsBelow);
    d->urlsEdit->setControlButtons(DItemsList::Add | DItemsList::Remove | DItemsList::Clear);
    d->urlsEdit->listView()->setColumn(DItemsListView::Filename, i18nc("@title", "Name"), true);
    d->urlsEdit->listView()->setColumn(DItemsListView::User1,    i18nc("@title", "Size"), true);
    d->urlsEdit->listView()->setColumn(DItemsListView::User2,    i18nc("@title", "Type"), true);
    d->urlsEdit->listView()->setColumn(DItemsListView::User3,    i18nc("@title", "Date"), true);
    d->urlsEdit->listView()->setColumn(DItemsListView::User4,    i18nc("@title", "Path"), true);

    d->urlsEdit->setWhatsThis(i18nc("@info", "This is the list of items hosted by this favorite.\n"
                                             "The current selected item from this list will be automatically\n"
                                             "shown in editor when favorite is open. If none is selected,\n"
                                             "first one from the list will be displayed."));
    urlsLabel->setBuddy(d->urlsEdit);

    // --------------------------------------------------------

    grid->addWidget(nameLabel,          0, 0, 1, 1);
    grid->addWidget(d->nameEdit,        0, 1, 1, 3);
    grid->addWidget(descLabel,          1, 0, 1, 1);
    grid->addWidget(d->descEdit,        1, 1, 1, 3);
    grid->addWidget(dateLabel,          2, 0, 1, 1);
    grid->addWidget(d->dateEdit,        2, 1, 1, 3);
    grid->addWidget(iconTextLabel,      3, 0, 1, 1);
    grid->addWidget(d->iconButton,      3, 1, 1, 1);
    grid->addWidget(d->resetIconButton, 3, 2, 1, 1);
    grid->addWidget(d->urlsEdit,        4, 1, 4, 3);
    grid->addWidget(urlsLabel,          5, 0, 1, 1);
    grid->addWidget(d->nbImagesLabel,   6, 0, 1, 1);
    grid->setRowStretch(7, 10);
    grid->setColumnStretch(3, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------

    connect(d->nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotModified()));

    connect(d->urlsEdit, SIGNAL(signalImageListChanged()),
            this, SLOT(slotModified()));

    connect(d->iconButton, SIGNAL(clicked()),
            this, SLOT(slotIconChanged()));

    connect(d->resetIconButton, SIGNAL(clicked()),
            this, SLOT(slotIconResetClicked()));

    connect(d->urlsEdit, SIGNAL(signalImageListChanged()),
            this, SLOT(slotUpdateMetadata()));

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    // --------------------------------------------------------

    d->nameEdit->setFocus();
    adjustSize();
}

ShowfotoStackViewFavoriteDlg::~ShowfotoStackViewFavoriteDlg()
{
    delete d;
}

bool ShowfotoStackViewFavoriteDlg::canAccept() const
{
    return (
            !name().isEmpty() &&
            !urls().isEmpty() &&
            !d->list->findFavoriteByHierarchy(ShowfotoStackViewFavoriteBase::hierarchyFromParent(name(), d->pitem))
           );
}

void ShowfotoStackViewFavoriteDlg::slotAccept()
{
    if (canAccept())
    {
        accept();
    }
}

void ShowfotoStackViewFavoriteDlg::slotModified()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(canAccept());

    int numberOfImages = d->urlsEdit->imageUrls().count();
    d->nbImagesLabel->setText(i18ncp("@info", "%1 image", "%1 images", numberOfImages));
}

QString ShowfotoStackViewFavoriteDlg::name() const
{
    return d->nameEdit->text();
}

QString ShowfotoStackViewFavoriteDlg::description() const
{
    return d->descEdit->text();
}

QDate ShowfotoStackViewFavoriteDlg::date() const
{
    return d->dateEdit->date();
}

QString ShowfotoStackViewFavoriteDlg::icon() const
{
    return d->icon;
}

QList<QUrl> ShowfotoStackViewFavoriteDlg::urls() const
{
    return d->urlsEdit->imageUrls();
}

QUrl ShowfotoStackViewFavoriteDlg::currentUrl() const
{
    return d->urlsEdit->getCurrentUrl();
}

void ShowfotoStackViewFavoriteDlg::setName(const QString& name)
{
    d->nameEdit->setText(name);
}

void ShowfotoStackViewFavoriteDlg::setDescription(const QString& desc)
{
    d->descEdit->setText(desc);
}

void ShowfotoStackViewFavoriteDlg::setDate(const QDate& date)
{
    d->dateEdit->setDate(date);
}

void ShowfotoStackViewFavoriteDlg::setIcon(const QString& icon)
{
    d->icon = icon;
}

void ShowfotoStackViewFavoriteDlg::setUrls(const QList<QUrl>& urls)
{
    d->urlsEdit->slotAddImages(urls);
}

void ShowfotoStackViewFavoriteDlg::setCurrentUrl(const QUrl& url)
{
    d->urlsEdit->setCurrentUrl(url);
}

void ShowfotoStackViewFavoriteDlg::setIconSize(int size)
{
    d->urlsEdit->setIconSize(size);
}

void ShowfotoStackViewFavoriteDlg::setSortOrder(int order)
{
    s_sortOrder = (Qt::SortOrder)order;
    d->urlsEdit->listView()->sortItems(s_sortRole + DItemsListView::Filename, s_sortOrder);
}

void ShowfotoStackViewFavoriteDlg::setSortRole(int role)
{
    s_sortRole = (ShowfotoStackViewList::StackViewRole)role;
    d->urlsEdit->listView()->sortItems(s_sortRole + DItemsListView::Filename, s_sortOrder);
}

void ShowfotoStackViewFavoriteDlg::setParentItem(ShowfotoStackViewFavoriteBase* const pitem)
{
    d->pitem = pitem;
}

void ShowfotoStackViewFavoriteDlg::slotIconResetClicked()
{
    d->icon = QLatin1String("folder-favorites");
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
}

void ShowfotoStackViewFavoriteDlg::slotIconChanged()
{

#ifdef HAVE_KICONTHEMES

    QPointer<KIconDialog> dlg = new KIconDialog(this);
    dlg->setup(KIconLoader::NoGroup, KIconLoader::Application, false, 20, false, false, false);
    QString icon              = dlg->openDialog();
    delete dlg;

    if (icon.isEmpty() || (icon == d->icon))
    {
        return;
    }

    d->icon                   = icon;
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));

#endif

}

void ShowfotoStackViewFavoriteDlg::slotUpdateMetadata()
{
    QTreeWidgetItemIterator it(d->urlsEdit->listView());

    while (*it)
    {
        DItemsListViewItem* const lvItem = dynamic_cast<DItemsListViewItem*>(*it);

        if (lvItem)
        {
            QFileInfo inf(lvItem->url().toLocalFile());
            ShowfotoItemInfo iteminf = ShowfotoItemInfo::itemInfoFromFile(inf);

            QString localeFileSize   = QLocale().toString(iteminf.size);
            QString str              = ItemPropertiesTab::humanReadableBytesCount(iteminf.size);
            lvItem->setText(DItemsListView::User1, str);
            lvItem->setData(DItemsListView::User1, Qt::UserRole, iteminf.size);

            QFileInfo fileInfo(iteminf.name);
            QString rawFilesExt      = DRawDecoder::rawFiles();
            QString ext              = fileInfo.suffix().toUpper();

            if (!ext.isEmpty() && rawFilesExt.toUpper().contains(ext))
            {
                lvItem->setText(DItemsListView::User2, i18nc("@info: item properties", "RAW Image"));
            }
            else
            {
                lvItem->setText(DItemsListView::User2, QMimeDatabase().mimeTypeForFile(fileInfo).comment());
            }

            QDateTime dt             = (iteminf.ctime.isValid() ? iteminf.ctime : iteminf.dtime);
            str                      = QLocale().toString(dt, QLocale::ShortFormat);
            lvItem->setText(DItemsListView::User3, str);
            lvItem->setData(DItemsListView::User3, Qt::UserRole, dt);

            lvItem->setText(DItemsListView::User4, inf.absolutePath());
        }

        ++it;
    }
}

bool ShowfotoStackViewFavoriteDlg::favoriteDialog(ShowfotoStackViewFavoriteList* const list,
                                                  QString& name,
                                                  QString& desc,
                                                  QDate& date,
                                                  QString& icon,
                                                  QList<QUrl>& urls,
                                                  QUrl& current,
                                                  int iconSize,
                                                  int sortOrder,
                                                  int sortRole,
                                                  ShowfotoStackViewFavoriteBase* const pitem,
                                                  bool create)
{
    QPointer<ShowfotoStackViewFavoriteDlg> dlg = new ShowfotoStackViewFavoriteDlg(list, create);
    dlg->setName(name);
    dlg->setDescription(desc);
    dlg->setDate(date);
    dlg->setIcon(icon);
    dlg->setUrls(urls);
    dlg->setCurrentUrl(current);
    dlg->setIconSize(iconSize);
    dlg->setSortOrder(sortOrder);
    dlg->setSortRole(sortRole);
    dlg->setParentItem(pitem);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name    = dlg->name();
        desc    = dlg->description();
        date    = dlg->date();
        icon    = dlg->icon();
        urls    = dlg->urls();
        current = dlg->currentUrl();
    }

    delete dlg;

    return valRet;
}

bool ShowfotoStackViewFavoriteDlg::myIsLessThanHandler(const QTreeWidgetItem* current, const QTreeWidgetItem& other)
{
    int result = 0;

    switch (s_sortRole + DItemsListView::Filename)
    {
        case DItemsListView::User1:     // Size
        {
            result = (ShowfotoItemSortSettings::compareByOrder(current->data(DItemsListView::User1, Qt::UserRole).toInt(),
                                                               other.data(DItemsListView::User1, Qt::UserRole).toInt(),
                                                               s_sortOrder));
            break;
        }

        case DItemsListView::User2:     // Type
        {
            result = (ShowfotoItemSortSettings::naturalCompare(current->text(DItemsListView::User2),
                                                               other.text(DItemsListView::User2),
                                                               s_sortOrder,
                                                               Qt::CaseSensitive));
            break;
        }

        case DItemsListView::User3:     // Date
        {
            result = (ShowfotoItemSortSettings::compareByOrder(current->data(DItemsListView::User3, Qt::UserRole).toDateTime(),
                                                               other.data(DItemsListView::User3, Qt::UserRole).toDateTime(),
                                                               s_sortOrder));
            break;
        }

        default:                        // Name
        {
            result = (ShowfotoItemSortSettings::naturalCompare(current->text(DItemsListView::Filename),
                                                               other.text(DItemsListView::Filename),
                                                               s_sortOrder,
                                                               Qt::CaseSensitive));
            break;
        }
    }

    return (result < 0);
}

} // namespace ShowFoto
