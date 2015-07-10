/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2015-06-16
 * Description : Advanced Configuration tab for metadata.
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail.com>
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

#include "advancedmetadatatab.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

#include "klocale.h"
#include "dmetadatasettings.h"
#include "namespacelistview.h"
#include "namespaceeditdlg.h"

namespace Digikam
{

class AdvancedMetadataTab::Private
{
public:
    Private()
    {

    }
    QComboBox* metadataType;
    QComboBox* operationType;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* moveUpButton;
    QPushButton* moveDownButton;
    QPushButton* resetButton;
    QCheckBox*   unifyReadWrite;
    QStandardItemModel* model;
    NamespaceListView* namespaceView;
    DMetadataSettingsContainer container;
};
AdvancedMetadataTab::AdvancedMetadataTab(QWidget* parent)
    :QWidget(parent), d(new Private())
{
    // ---------- Advanced Configuration Panel -----------------------------
    d->container = DMetadataSettings::instance()->settings();
    setUi();
    setModelData();
    connectButtons();

    d->unifyReadWrite->setChecked(d->container.unifyReadWrite);
    connect(d->unifyReadWrite, SIGNAL(toggled(bool)), this, SLOT(slotUnifyChecked(bool)));

    if(d->unifyReadWrite->isChecked())
    {
        d->operationType->setEnabled(false);
    }
}

AdvancedMetadataTab::~AdvancedMetadataTab()
{
    delete d;
}

void AdvancedMetadataTab::slotResetView()
{
    setModelData();
}

void AdvancedMetadataTab::slotAddNewNamespace()
{
    NamespaceEntry entry;

    if (!NamespaceEditDlg::create(qApp->activeWindow(), entry))
    {
        return;
    }

    QStandardItem* root = d->model->invisibleRootItem();
    QString text = entry.namespaceName + QLatin1String(",") + i18n("Separator:") + entry.separator;
    QStandardItem* item = new QStandardItem(text);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
    root->appendRow(item);
    d->container.readTagNamespaces.append(entry);
}

void AdvancedMetadataTab::slotEditNamespace()
{

    if(!d->namespaceView->currentIndex().isValid())
        return;

    NamespaceEntry entry = d->container.readTagNamespaces.at(d->namespaceView->currentIndex().row());


    if (!NamespaceEditDlg::edit(qApp->activeWindow(), entry))
    {
        return;
    }


    QStandardItem* root = d->model->invisibleRootItem();
    QStandardItem* item = root->child(d->namespaceView->currentIndex().row());
    QString text = entry.namespaceName + QLatin1String(",") + i18n("Separator:") + entry.separator;
    item->setText(text);

}

void AdvancedMetadataTab::slotUnifyChecked(bool value)
{
    d->operationType->setDisabled(value);
    d->container.unifyReadWrite = value;
}


void AdvancedMetadataTab::connectButtons()
{
    connect(d->addButton, SIGNAL(clicked()), this, SLOT(slotAddNewNamespace()));
    connect(d->editButton, SIGNAL(clicked()), this, SLOT(slotEditNamespace()));
    connect(d->deleteButton, SIGNAL(clicked()), d->namespaceView, SLOT(slotDeleteSelected()));
    connect(d->resetButton, SIGNAL(clicked()), this, SLOT(slotResetView()));
    connect(d->moveUpButton, SIGNAL(clicked()), d->namespaceView, SLOT(slotMoveItemUp()));
    connect(d->moveDownButton, SIGNAL(clicked()), d->namespaceView, SLOT(slotMoveItemDown()));
}

void AdvancedMetadataTab::setModelData()
{
    qDebug() << "Setting model data";
    d->model->clear();

    QStandardItem* root = d->model->invisibleRootItem();
    for(NamespaceEntry e : d->container.readTagNamespaces)
    {
        QString text = e.namespaceName + QLatin1String(",") + i18n("Separator:") + e.separator;
        QStandardItem* item = new QStandardItem(text);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
        root->appendRow(item);
    }
    d->namespaceView->setModel(d->model);
}

void AdvancedMetadataTab::setUi()
{
    QVBoxLayout* const advancedConfLayout = new QVBoxLayout(this);

    QHBoxLayout* const topLayout = new QHBoxLayout(this);
    QHBoxLayout* const bottomLayout = new QHBoxLayout(this);

    //--- Top layout ----------------
    d->metadataType = new QComboBox(this);

    d->operationType = new QComboBox(this);

    d->metadataType->insertItems(0, QStringList() << i18n("Tags") << i18n("Ratings") << i18n("Comments"));

    d->operationType->insertItems(0, QStringList() << i18n("Read Options") << i18n("Write Options"));

    d->unifyReadWrite = new QCheckBox(i18n("Unify read and write"));


    topLayout->addWidget(d->metadataType);
    topLayout->addWidget(d->operationType);
    topLayout->addWidget(d->unifyReadWrite);

    //------------ Bottom Layout-------------
    // View
    d->namespaceView = new NamespaceListView(this);
    d->model = new QStandardItemModel(this);


    // Buttons
    QVBoxLayout* buttonsLayout = new QVBoxLayout(this);
    buttonsLayout->setAlignment(Qt::AlignTop);
    d->addButton = new QPushButton(i18n("Add"));
    d->editButton = new QPushButton(i18n("Edit"));
    d->deleteButton = new QPushButton(i18n("Delete"));

    d->moveUpButton = new QPushButton(i18n("Move Up"));
    d->moveDownButton = new QPushButton(i18n("Move Down"));
    d->resetButton = new QPushButton(i18n("Reset"));


    buttonsLayout->addWidget(d->addButton);
    buttonsLayout->addWidget(d->editButton);
    buttonsLayout->addWidget(d->deleteButton);
    buttonsLayout->addWidget(d->moveUpButton);
    buttonsLayout->addWidget(d->moveDownButton);
    buttonsLayout->addWidget(d->resetButton);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(d->namespaceView);

    bottomLayout->addLayout(vbox);
    bottomLayout->addLayout(buttonsLayout);

    advancedConfLayout->addLayout(topLayout);
    advancedConfLayout->addLayout(bottomLayout);

    this->setLayout(advancedConfLayout);
}


}


