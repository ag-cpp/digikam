/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : digiKam plugin main dialog
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dplugindialog.h"

// Qt includes

#include <QApplication>
#include <QPushButton>
#include <QPointer>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "dxmlguiwindow.h"
#include "dpluginaboutdlg.h"
#include "dplugin.h"

namespace Digikam
{

DPluginDialog::DPluginDialog(QWidget* const parent, const QString& objName)
    : QDialog  (parent),
      m_buttons(nullptr),
      m_tool   (nullptr)
{
    setObjectName(objName);
    setWindowFlags((windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    m_buttons = new QDialogButtonBox(this);
    restoreDialogSize();
}

DPluginDialog::~DPluginDialog()
{
    saveDialogSize();
}

void DPluginDialog::setPlugin(DPlugin* const tool)
{
    m_tool = tool;

    if (m_tool)
    {
        QPushButton* const help = m_buttons->addButton(QDialogButtonBox::Help);
        help->setText(i18nc("@action: button", "About..."));

        connect(help, SIGNAL(clicked()),
                this, SLOT(slotAboutPlugin()));
    }
}

void DPluginDialog::slotAboutPlugin()
{
    QPointer<DPluginAboutDlg> dlg = new DPluginAboutDlg(m_tool);
    dlg->exec();
    delete dlg;
}

void DPluginDialog::restoreDialogSize()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(objectName());

    winId();
    DXmlGuiWindow::setGoodDefaultWindowSize(windowHandle());
    DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

void DPluginDialog::saveDialogSize()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(objectName());
    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
}

} // namespace Digikam
