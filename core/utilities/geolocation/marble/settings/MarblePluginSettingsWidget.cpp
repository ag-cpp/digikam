/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings View.
 *
 * SPDX-FileCopyrightText: 2007      by Torsten Rahn <rahn at kde dot org>
 * SPDX-FileCopyrightText: 2008      by Jens-Michael Hoffmann <jensmh at gmx dot de>
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "MarblePluginSettingsWidget.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "geopluginaboutdlg.h"
#include "DialogConfigurationInterface.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

using namespace Digikam;

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

    QPointer<GeoPluginAboutDlg> dlg = new GeoPluginAboutDlg(d->pluginModel->pluginIface(index), this);
    dlg->exec();
    delete dlg;
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
