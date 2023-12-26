// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "DialogConfigurationInterface.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

namespace Marble
{

class MarblePluginSettingsWidget::Private
{
public:

    Private() = default;

public:

    QListView*                  pluginListView = nullptr;
    PluginItemDelegate*         itemDelegate   = nullptr;
    QPointer<RenderPluginModel> pluginModel;
};

// ---

MarblePluginSettingsWidget::MarblePluginSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    d->pluginListView       = new QListView(this);
    QVBoxLayout* const vlay = new QVBoxLayout(this);
    vlay->addWidget(d->pluginListView);

    d->itemDelegate         = new PluginItemDelegate(d->pluginListView, this);
    d->pluginListView->setItemDelegate(d->itemDelegate);

    connect(d->itemDelegate, SIGNAL(aboutPluginClicked(QModelIndex)),
            this, SLOT(slotPluginAboutDialog(QModelIndex)));

    connect(d->itemDelegate, SIGNAL(configPluginClicked(QModelIndex)),
            this, SLOT(slotPluginConfigDialog(QModelIndex)));
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setAboutIcon(const QIcon& icon)
{
    d->itemDelegate->setAboutIcon(icon);
}

void MarblePluginSettingsWidget::setConfigIcon(const QIcon& icon)
{
    d->itemDelegate->setConfigIcon(icon);
}

void MarblePluginSettingsWidget::setModel(RenderPluginModel* const pluginModel)
{
    if (!d->pluginModel.isNull())
    {
        disconnect(d->pluginModel.data(), nullptr, this, nullptr);
    }

    d->pluginModel = pluginModel;
    d->pluginListView->setModel(pluginModel);

    if (!d->pluginModel.isNull())
    {
        connect(d->pluginModel.data(), SIGNAL(itemChanged(QStandardItem*)),
                this, SIGNAL(pluginListViewClicked()));
    }
}

void MarblePluginSettingsWidget::slotPluginAboutDialog(const QModelIndex& index)
{
    if (d->pluginModel.isNull())
    {
        return;
    }

    QDialog* const dlg = new QDialog(this);
    dlg->setWindowFlags((dlg->windowFlags() & ~Qt::Dialog) |
                   Qt::Window                              |
                   Qt::WindowCloseButtonHint               |
                   Qt::WindowMinMaxButtonsHint);

    dlg->setWindowTitle(i18n("About Plugin %1", d->pluginModel->data(index, RenderPluginModel::Name).toString()));
    QLabel* const icon      = new QLabel(dlg);
    icon->setPixmap(qvariant_cast<QIcon>(d->pluginModel->data(index, RenderPluginModel::Icon)).pixmap(48, 48));
    QLabel* const text      = new QLabel(dlg);
    text->setText(i18n(
                       "<p>Version %1</p>"
                       "<p>%2</p>",
                       d->pluginModel->data(index, RenderPluginModel::Version).toString(),
                       d->pluginModel->data(index, RenderPluginModel::Description).toString()
                      )
                 );

    QDialogButtonBox* const btn = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    btn->button(QDialogButtonBox::Ok)->setDefault(true);

    QVBoxLayout* const vlay = new QVBoxLayout(dlg);
    QHBoxLayout* const hlay = new QHBoxLayout;
    hlay->addWidget(icon);
    hlay->addWidget(text);
    vlay->addLayout(hlay);
    vlay->addWidget(btn);

    connect(btn->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            dlg, SLOT(accept()));

    dlg->adjustSize();
    dlg->exec();
}

void MarblePluginSettingsWidget::slotPluginConfigDialog(const QModelIndex& index)
{
    if (d->pluginModel.isNull())
    {
        return;
    }

    DialogConfigurationInterface* const configInterface = d->pluginModel->pluginDialogConfigurationInterface(index);
    QDialog* const configDialog                         = configInterface ? configInterface->configDialog() : nullptr;

    if (configDialog)
    {
        configDialog->exec();
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
