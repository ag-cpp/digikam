/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DXML_GUI_WINDOW_H
#define DIGIKAM_DXML_GUI_WINDOW_H

// Qt includes

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QWindow>

// KDE includes

#include <kxmlguiwindow.h>

// Local includes

#include "digikam_export.h"
#include "digikam_config.h"
#include "dlogoaction.h"
#include "dinfointerface.h"
#include "dpluginaction.h"
#include "dpluginloader.h"

class QEvent;

class KToolBar;
class KConfigGroup;

namespace Digikam
{

/**
 * Optional parts which can be hidden or not from managed window configuration panel
 */
enum FullScreenOptions
{
    FS_TOOLBARS   = 0x00000001,                                                ///< Manage Tools bar in full-screen mode.
    FS_THUMBBAR   = 0x00000002,                                                ///< Manage Thumb bar in full-screen mode.
    FS_SIDEBARS   = 0x00000004,                                                ///< Manage Side bars in full-screen mode.
    FS_STATUSBAR  = 0x00000008,                                                ///< Manage Status bar in full-screen mode.
    FS_NONE       = 0x00000010,                                                ///< No full-screen options.

    FS_ALBUMGUI   = FS_TOOLBARS | FS_THUMBBAR | FS_SIDEBARS | FS_STATUSBAR,    ///< Album GUI Config.
    FS_EDITOR     = FS_TOOLBARS | FS_THUMBBAR | FS_SIDEBARS | FS_STATUSBAR,    ///< Image Editor Config.
    FS_LIGHTTABLE = FS_TOOLBARS | FS_SIDEBARS | FS_STATUSBAR,                  ///< Light Table Config.
    FS_IMPORTUI   = FS_TOOLBARS | FS_THUMBBAR | FS_SIDEBARS | FS_STATUSBAR     ///< Import UI Config.
};

enum StdActionType
{
    StdCopyAction = 0,
    StdPasteAction,
    StdCutAction,
    StdQuitAction,
    StdCloseAction,
    StdZoomInAction,
    StdZoomOutAction,
    StdOpenAction,
    StdSaveAction,
    StdSaveAsAction,
    StdRevertAction,
    StdBackAction,
    StdForwardAction
};

static const QString s_configFullScreenHideToolBarsEntry(QLatin1String("FullScreen Hide ToolBars"));
static const QString s_configFullScreenHideThumbBarEntry(QLatin1String("FullScreen Hide ThumbBar"));
static const QString s_configFullScreenHideSideBarsEntry(QLatin1String("FullScreen Hide SideBars"));
static const QString s_configFullScreenHideStatusBarEntry(QLatin1String("FullScreen Hide StatusBar"));

/**
 * Data container to use in managed window.
 */
class DIGIKAM_EXPORT DXmlGuiWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:

    explicit DXmlGuiWindow(QWidget* const parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~DXmlGuiWindow() override;

    /**
     * Manage config group name used by window instance to get/set settings from config file
     */
    void setConfigGroupName(const QString& name);
    QString configGroupName() const;

    /**
     * Create common actions to setup all digiKam main windows.
     */
    void createSettingsActions();

    /**
     * Create common actions from Help menu for all digiKam main windows.
     */
    void createHelpActions(bool coreOptions = true);

    /**
     * Cleanup unwanted actions from action collection.
     */
    void cleanupActions();

    /**
     * Create common actions to handle side-bar through keyboard shortcuts.
     */
    void createSidebarActions();

    /**
     * Set full-screen options to managed window
     */
    void setFullScreenOptions(int options);

    /**
     * Create Full-screen action to action collection instance from managed window
     * set through setManagedWindow(). This action is connected to slotToggleFullScreen() slot.
     * 'name' is action name used in KDE UI rc file.
     */
    void createFullScreenAction(const QString& name);

    /**
     * Read full-screen settings from KDE config file.
     */
    void readFullScreenSettings(const KConfigGroup& group);

    /**
     * Return true if managed window is currently in Full Screen Mode.
     */
    bool fullScreenIsActive() const;

    /**
     * Return all actions from internal collection.
     */
    QList<QAction*> allActions() const;

public:

    /**
     * Register all generic plugins action to this instance.
     * Call registerExtraPluginsActions() to plug oter kind of plugins in GUI.
     */
    void registerPluginsActions();
    virtual void registerExtraPluginsActions(QString& /*dom*/) {};

    /**
     * Return the interface instance to access to items information.
     */
    virtual DInfoInterface* infoIface(DPluginAction* const ac) = 0;

public:

    static void openHandbook();
    static void restoreWindowSize(QWindow* const win, const KConfigGroup& group);
    static void saveWindowSize(QWindow* const win, KConfigGroup& group);

    static QAction* buildStdAction(StdActionType type,
                                   const QObject* const recvr,
                                   const char* const slot,
                                   QObject* const parent);

    /**
     * If we have some local breeze icon resource, prefer it.
     */
    static void setupIconTheme();

protected:

    DLogoAction* m_animLogo;

protected:

    QAction* showMenuBarAction()   const;
    QAction* showStatusBarAction() const;

    /**
     * Call this method from your main window to show keyboard shortcut config dialog
     * with an extra action collection to configure. This method is called by slotEditKeys()
     * which can be re-implement in child class for cutomization.
     */
    void editKeyboardShortcuts(KActionCollection* const extraac = nullptr, const QString& actitle = QString());

    void closeEvent(QCloseEvent* e)            override;
    void keyPressEvent(QKeyEvent* e)           override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

    /**
     * Re-implement this method if you want to manage sidebars visibility in full-screen mode.
     * By default this method do nothing.
     */
    virtual void showSideBars(bool visible);

    /**
     * Re-implement this method if you want to manage thumbbar visibility in full-screen mode.
     * By default this method do nothing.
     */
    virtual void showThumbBar(bool visible);

    /**
     * Re-implement this method if you want to manage customized view visibility in full-screen mode.
     * This method is called by switchWindowToFullScreen(). By default this method do nothing.
     */
    virtual void customizedFullScreenMode(bool set);

    /**
     * Re-implement this method if managed window has a thumbbar. This must return visibility state of it.
     */
    virtual bool thumbbarVisibility() const;

private Q_SLOTS:

    void slotToggleFullScreen(bool);
    void slotShowMenuBar();
    void slotShowStatusBar();
    void slotConfNotifications();
    void slotConfToolbars();
    void slotNewToolbarConfig();

    void slotRawCameraList();
    void slotSolidHardwareList();
    void slotDonateMoney();
    void slotRecipesBook();
    void slotContribute();
    void slotHelpContents();

    /**
     * Slots for common Help Actions
     */
    virtual void slotOnlineVersionCheck()      {};
    virtual void slotComponentsInfo()          {};
    virtual void slotDBStat()                  {};

    /**
     * Slots for common Sidebar Actions
     */
    virtual void slotToggleLeftSideBar()       {};
    virtual void slotToggleRightSideBar()      {};
    virtual void slotPreviousLeftSideBarTab()  {};
    virtual void slotNextLeftSideBarTab()      {};
    virtual void slotPreviousRightSideBarTab() {};
    virtual void slotNextRightSideBarTab()     {};

    /**
     * Slots for common Settings actions
     */
    virtual void slotEditKeys()                { editKeyboardShortcuts(); };
    virtual void slotSetup() = 0;

private:

    /**
     * Used by slotToggleFullScreen() to switch tool-bar visibility in managed window
     */
    void showToolBars(bool visible);

    /**
     * Return main tool bar instance created in managed window.
     */
    KToolBar* mainToolBar() const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DXML_GUI_WINDOW_H
