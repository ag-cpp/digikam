/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dxmlguiwindow_p.h"

namespace Digikam
{

DXmlGuiWindow::DXmlGuiWindow(QWidget* const parent, Qt::WindowFlags f)
    : KXmlGuiWindow(parent, f),
      d            (new Private)
{
    m_animLogo = nullptr;

    installEventFilter(this);
}

DXmlGuiWindow::~DXmlGuiWindow()
{
    delete d;
}

void DXmlGuiWindow::setConfigGroupName(const QString& name)
{
    d->configGroupName = name;
}

QString DXmlGuiWindow::configGroupName() const
{
    return d->configGroupName;
}

void DXmlGuiWindow::closeEvent(QCloseEvent* e)
{
    if (fullScreenIsActive())
    {
        slotToggleFullScreen(false);
    }

    if (!testAttribute(Qt::WA_DeleteOnClose))
    {
        setVisible(false);
        e->ignore();
        return;
    }

    KXmlGuiWindow::closeEvent(e);
}

void DXmlGuiWindow::slotShowMenuBar()
{
    menuBar()->setVisible(d->showMenuBarAction->isChecked());
}

void DXmlGuiWindow::slotShowStatusBar()
{
    statusBar()->setVisible(d->showStatusBarAction->isChecked());
}

void DXmlGuiWindow::slotConfNotifications()
{

#ifdef HAVE_KNOTIFYCONFIG

    KNotifyConfigWidget::configure(this);

#endif

}

void DXmlGuiWindow::editKeyboardShortcuts(KActionCollection* const extraac, const QString& actitle)
{
    KShortcutsDialog dialog(KShortcutsEditor::AllActions,
                            KShortcutsEditor::LetterShortcutsAllowed, this);
    dialog.addCollection(actionCollection(), i18nc("general keyboard shortcuts", "General"));

    if (extraac)
    {
        dialog.addCollection(extraac, actitle);
    }

    dialog.configure();
}

bool DXmlGuiWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == this)
    {
        if (ev && (ev->type() == QEvent::HoverMove) && fullScreenIsActive())
        {
            // We will handle a stand alone FullScreen button action on top/right corner of screen
            // only if managed window tool bar is hidden, and if we switched already in Full Screen mode.

            KToolBar* const mainbar = mainToolBar();

            if (mainbar)
            {
                if (((d->fsOptions & FS_TOOLBARS) && d->fullScreenHideToolBars) || !mainbar->isVisible())
                {
                    QHoverEvent* const mev = dynamic_cast<QHoverEvent*>(ev);

                    if (mev)
                    {
                        QPoint pos(mev->pos());
                        QRect  desktopRect = windowHandle()->screen()->geometry();

                        QRect sizeRect(QPoint(0, 0), d->fullScreenBtn->size());
                        QRect topLeft, topRight;
                        QRect topRightLarger;

                        desktopRect        = QRect(desktopRect.y(), desktopRect.y(), desktopRect.width(), desktopRect.height());
                        topLeft            = sizeRect;
                        topRight           = sizeRect;

                        topLeft.moveTo(desktopRect.x(), desktopRect.y());
                        topRight.moveTo(desktopRect.x() + desktopRect.width() - sizeRect.width() - 1, topLeft.y());

                        topRightLarger     = topRight.adjusted(-25, 0, 0, 10);

                        if (topRightLarger.contains(pos))
                        {
                            d->fullScreenBtn->move(topRight.topLeft());
                            d->fullScreenBtn->show();
                        }
                        else
                        {
                            d->fullScreenBtn->hide();
                        }

                        return false;
                    }
                }
            }
        }
    }

    // pass the event on to the parent class

    return QObject::eventFilter(obj, ev);
}

void DXmlGuiWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        if (fullScreenIsActive())
        {
            d->fullScreenAction->activate(QAction::Trigger);
        }
    }
}

void DXmlGuiWindow::showSideBars(bool visible)
{
    Q_UNUSED(visible);
}

void DXmlGuiWindow::showThumbBar(bool visible)
{
    Q_UNUSED(visible);
}

bool DXmlGuiWindow::thumbbarVisibility() const
{
    return true;
}

void DXmlGuiWindow::slotHelpContents()
{
    openHandbook();
}

void DXmlGuiWindow::openHandbook()
{
    QUrl url = QUrl(QString::fromUtf8("https://docs.kde.org/?application=%1&branch=trunk5")
               .arg(QApplication::applicationName()));

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::restoreWindowSize(QWindow* const win, const KConfigGroup& group)
{
    KWindowConfig::restoreWindowSize(win, group);
}

void DXmlGuiWindow::saveWindowSize(QWindow* const win, KConfigGroup& group)
{
    KWindowConfig::saveWindowSize(win, group);
}

void DXmlGuiWindow::slotRawCameraList()
{
    showRawCameraList();
}

void DXmlGuiWindow::slotSolidHardwareList()
{
    SolidHardwareDlg* const dlg = new SolidHardwareDlg(qApp->activeWindow());
    dlg->exec();
}

void DXmlGuiWindow::slotDonateMoney()
{
    WebBrowserDlg* const browser = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/donate/")),
                                                     qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::slotRecipesBook()
{
    WebBrowserDlg* const browser = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/recipes_book/")),
                                                     qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::slotContribute()
{
    WebBrowserDlg* const browser = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/contribute/")),
                                                     qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::setupIconTheme()
{
    // Let QStandardPaths handle this, it will look for app local stuff
    // this means e.g. for mac: "<APPDIR>/../Resources" and for win: "<APPDIR>/data".

    bool hasBreeze                = false;
    const QString breezeIcons     = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                           QLatin1String("breeze.rcc"));

    if (!breezeIcons.isEmpty() && QFile::exists(breezeIcons))
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze icons resource file found";
        QResource::registerResource(breezeIcons);
        hasBreeze = true;
    }

    bool hasBreezeDark            = false;
    const QString breezeDarkIcons = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                           QLatin1String("breeze-dark.rcc"));

    if (!breezeDarkIcons.isEmpty() && QFile::exists(breezeDarkIcons))
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze-dark icons resource file found";
        QResource::registerResource(breezeDarkIcons);
        hasBreezeDark = true;
    }

    if (hasBreeze || hasBreezeDark)
    {
        // Tell Qt about the theme

        QIcon::setThemeSearchPaths(QStringList() << QLatin1String(":/icons"));

        const QStringList iconsDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                                QLatin1String("icons"),
                                                                QStandardPaths::LocateDirectory);

        if (!iconsDirs.isEmpty())
        {
            QIcon::setThemeSearchPaths(QStringList() << QIcon::themeSearchPaths() << iconsDirs);
        }

        if      (hasBreeze)
        {
            QIcon::setThemeName(QLatin1String("breeze"));
        }
        else if (hasBreezeDark)
        {
            QIcon::setThemeName(QLatin1String("breeze-dark"));
        }
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Use installed icons";
    }
}

} // namespace Digikam
