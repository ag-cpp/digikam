 /* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-16
 * Description : Camera interface
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "cameraui.h"
#include "cameraui.moc"

// Qt includes

#include <QCheckBox>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QSplitter>
#include <QTimer>
#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QScrollArea>

// KDE includes

#include <kaboutdata.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcalendarsystem.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kimageio.h>
#include <kinputdialog.h>
#include <kio/global.h>
#include <kio/previewjob.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kshortcutsdialog.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kvbox.h>
#include <ktoggleaction.h>

// Libkdcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes

#include "componentsinfo.h"
#include "statuszoombar.h"
#include "statusprogressbar.h"
#include "statusnavigatebar.h"
#include "dlogoaction.h"
#include "clicklabel.h"
#include "thumbnailsize.h"
#include "kdatetimeedit.h"
#include "sidebar.h"
#include "themeengine.h"
#include "setup.h"
#include "downloadsettingscontainer.h"
#include "downloadhistory.h"
#include "imagepropertiessidebarcamgui.h"
#include "albummanager.h"
#include "albumsettings.h"
#include "album.h"
#include "albumselectdialog.h"
#include "renamecustomizer.h"
#include "freespacewidget.h"
#include "collectionscanner.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "scancontroller.h"
#include "rawcameradlg.h"
#include "capturedlg.h"
#include "camerafolderdialog.h"
#include "camerainfodialog.h"
#include "cameraiconview.h"
#include "cameraiconitem.h"
#include "cameracontroller.h"
#include "cameralist.h"
#include "cameratype.h"
#include "cameraui_p.h"

namespace Digikam
{

CameraUI::CameraUI(QWidget* parent, const QString& cameraTitle,
                   const QString& model, const QString& port,
                   const QString& path)
        : KXmlGuiWindow(parent), d(new CameraUIPriv)

{
    d->cameraTitle = cameraTitle;
    setCaption(cameraTitle);

    // -------------------------------------------------------------------

    setupUserArea();
    setupStatusBar();
    setupActions();
    setupAccelerators();

    // -- Make signals/slots connections ---------------------------------

    setupConnections();
    slotSidebarTabTitleStyleChanged();

    // -- Read settings --------------------------------------------------

    readSettings();
    setAutoSaveSettings("Camera Settings", true);


    // -- Init. camera controller ----------------------------------------

    setupCameraController(model, port, path);

    d->view->setFocus();
    QTimer::singleShot(0, d->controller, SLOT(slotConnect()));
}

CameraUI::~CameraUI()
{
    disconnect(d->view, 0, this, 0);
    delete d->view;
    delete d->rightSideBar;
    delete d->controller;
    delete d;
}

void CameraUI::setupUserArea()
{
    KHBox* widget   = new KHBox(this);
    d->splitter     = new SidebarSplitter(widget);
    d->view         = new CameraIconView(this, d->splitter);
    d->rightSideBar = new ImagePropertiesSideBarCamGui(widget, d->splitter, KMultiTabBar::Right, true);
    d->rightSideBar->setObjectName("CameraGui Sidebar Right");
    d->splitter->setFrameStyle( QFrame::NoFrame );
    d->splitter->setFrameShadow( QFrame::Plain );
    d->splitter->setFrameShape( QFrame::NoFrame );
    d->splitter->setOpaqueResize(false);
    d->splitter->setStretchFactor(0, 10);      // set iconview default size to max.

    // -------------------------------------------------------------------------

    QScrollArea *sv     = new QScrollArea(d->rightSideBar);
    sv->setFrameStyle(QFrame::NoFrame);
    sv->setWidgetResizable(true);

    d->advBox           = new KVBox(sv->viewport());
    sv->setWidget(d->advBox);

    d->renameCustomizer = new RenameCustomizer(d->advBox, d->cameraTitle);
    d->renameCustomizer->setWhatsThis( i18n("Set how digiKam will rename files as they are downloaded."));
    d->view->setRenameCustomizer(d->renameCustomizer);

    d->renameFileLabel = new DLabelExpander(d->advBox);
    d->renameFileLabel->setText(i18n("File Renaming Options"));
    d->renameFileLabel->setPixmap(SmallIcon("insert-image"));
    d->renameFileLabel->setContainer(d->renameCustomizer);
    d->renameFileLabel->setLineVisible(false);

    // -- Albums Auto-creation options -----------------------------------------

    QWidget* albumBox      = new QWidget(d->advBox);
    QVBoxLayout* albumVlay = new QVBoxLayout(albumBox);
    d->autoAlbumExtCheck   = new QCheckBox(i18n("Extension-based sub-albums"), albumBox);
    d->autoAlbumDateCheck  = new QCheckBox(i18n("Date-based sub-albums"), albumBox);
    KHBox *hbox1           = new KHBox(albumBox);
    d->folderDateLabel     = new QLabel(i18n("Date format:"), hbox1);
    d->folderDateFormat    = new KComboBox(hbox1);
    d->folderDateFormat->insertItem(CameraUIPriv::IsoDateFormat,   i18n("ISO"));
    d->folderDateFormat->insertItem(CameraUIPriv::TextDateFormat,  i18n("Full Text"));
    d->folderDateFormat->insertItem(CameraUIPriv::LocalDateFormat, i18n("Local Settings"));

    albumVlay->addWidget(d->autoAlbumExtCheck);
    albumVlay->addWidget(d->autoAlbumDateCheck);
    albumVlay->addWidget(hbox1);
    albumVlay->addStretch();
    albumVlay->setMargin(KDialog::spacingHint());
    albumVlay->setSpacing(KDialog::spacingHint());

    albumBox->setWhatsThis( i18n("Set how digiKam creates albums automatically when downloading."));
    d->autoAlbumExtCheck->setWhatsThis( i18n("Enable this option if you want to download your "
                     "pictures into automatically created file extension-based sub-albums of the destination "
                     "album. This way, you can separate JPEG and RAW files as they are downloaded from your camera."));
    d->autoAlbumDateCheck->setWhatsThis( i18n("Enable this option if you want to "
                     "download your pictures into automatically created file date-based sub-albums "
                     "of the destination album."));
    d->folderDateFormat->setWhatsThis( i18n("<p>Select your preferred date format used to "
                     "create new albums. The options available are:</p>"
                     "<p><b>ISO</b>: the date format is in accordance with ISO 8601 "
                     "(YYYY-MM-DD). E.g.: <i>2006-08-24</i></p>"
                     "<p><b>Full Text</b>: the date format is in a user-readable string. "
                     "E.g.: <i>Thu Aug 24 2006</i></p>"
                     "<p><b>Local Settings</b>: the date format depending on KDE control panel settings.</p>"));

    d->autoAlbumLabel = new DLabelExpander(d->advBox);
    d->autoAlbumLabel->setText(i18n("Auto-creation of Albums"));
    d->autoAlbumLabel->setPixmap(SmallIcon("folder-new"));
    d->autoAlbumLabel->setContainer(albumBox);

    // -- On the Fly options ---------------------------------------------------

    QWidget* onFlyBox      = new QWidget(d->advBox);
    QVBoxLayout* onFlyVlay = new QVBoxLayout(onFlyBox);
    d->setPhotographerId   = new QCheckBox(i18n("Set default photographer identity"), onFlyBox);
    d->setCredits          = new QCheckBox(i18n("Set default credit and copyright"), onFlyBox);
    d->fixDateTimeCheck    = new QCheckBox(i18n("Fix internal date && time"), onFlyBox);
    d->dateTimeEdit        = new KDateTimeEdit(onFlyBox, "datepicker");
    d->autoRotateCheck     = new QCheckBox(i18n("Auto-rotate/flip image"), onFlyBox);
    d->convertJpegCheck    = new QCheckBox(i18n("Convert to lossless file format"), onFlyBox);
    KHBox *hbox2           = new KHBox(onFlyBox);
    d->formatLabel         = new QLabel(i18n("New image format:"), hbox2);
    d->losslessFormat      = new KComboBox(hbox2);
    d->losslessFormat->insertItem(0, "PNG");

    onFlyVlay->addWidget(d->setPhotographerId);
    onFlyVlay->addWidget(d->setCredits);
    onFlyVlay->addWidget(d->fixDateTimeCheck);
    onFlyVlay->addWidget(d->dateTimeEdit);
    onFlyVlay->addWidget(d->autoRotateCheck);
    onFlyVlay->addWidget(d->convertJpegCheck);
    onFlyVlay->addWidget(hbox2);
    onFlyVlay->addStretch();
    onFlyVlay->setMargin(KDialog::spacingHint());
    onFlyVlay->setSpacing(KDialog::spacingHint());

    onFlyBox->setWhatsThis( i18n("Set here all options to fix/transform JPEG files automatically "
                     "as they are downloaded."));
    d->autoRotateCheck->setWhatsThis( i18n("Enable this option if you want images automatically "
                     "rotated or flipped using EXIF information provided by the camera."));
    d->setPhotographerId->setWhatsThis( i18n("Enable this option to store the default "
                     "photographer identity in the XMP and IPTC tags using digiKam's metadata settings."));
    d->setCredits->setWhatsThis( i18n("Enable this option to store the default credit "
                     "and copyright information in the XMP and IPTC tags using digiKam's metadata settings."));
    d->fixDateTimeCheck->setWhatsThis( i18n("Enable this option to set date and time metadata "
                     "tags to the right values if your camera does not set "
                     "these tags correctly when pictures are taken. The values will "
                     "be saved in the DateTimeDigitized and DateTimeCreated EXIF, XMP, and IPTC tags."));
    d->convertJpegCheck->setWhatsThis( i18n("Enable this option to automatically convert "
                     "all JPEG files to a lossless image format. <b>Note:</b> Image conversion can take a "
                     "while on a slow computer."));
    d->losslessFormat->setWhatsThis( i18n("Select your preferred lossless image file format to "
                     "convert to. <b>Note:</b> All metadata will be preserved during the conversion."));

    d->onFlyLabel = new DLabelExpander(d->advBox);
    d->onFlyLabel->setText(i18n("On the Fly Operations (JPEG only)"));
    d->onFlyLabel->setPixmap(SmallIcon("system-run"));
    d->onFlyLabel->setContainer(onFlyBox);

    QLabel *space = new QLabel(d->advBox);
    d->advBox->setStretchFactor(space, 10),

    d->rightSideBar->appendTab(sv, SmallIcon("configure"), i18n("Settings"));
    d->rightSideBar->loadViewState();

    // -------------------------------------------------------------------------

    setCentralWidget(widget);
}

void CameraUI::setupActions()
{
    // -- File menu ----------------------------------------------------

    d->cameraCancelAction = new KAction(KIcon("process-stop"), i18n("Cancel"), this);
    connect(d->cameraCancelAction, SIGNAL(triggered()), this, SLOT(slotCancelButton()));
    actionCollection()->addAction("cameraui_cancelprocess", d->cameraCancelAction);
    d->cameraCancelAction->setEnabled(false);

    // -----------------------------------------------------------------

    d->cameraInfoAction = new KAction(KIcon("camera-photo"), i18n("Information"), this);
    connect(d->cameraInfoAction, SIGNAL(triggered()), this, SLOT(slotInformation()));
    actionCollection()->addAction("cameraui_info", d->cameraInfoAction);

    // -----------------------------------------------------------------

    d->cameraCaptureAction = new KAction(KIcon("webcamreceive"), i18n("Capture"), this);
    connect(d->cameraCaptureAction, SIGNAL(triggered()), this, SLOT(slotCapture()));
    actionCollection()->addAction("cameraui_capture", d->cameraCaptureAction);

    // -----------------------------------------------------------------

    KAction* closeAction = KStandardAction::close(this, SLOT(close()), this);
    actionCollection()->addAction("cameraui_close", closeAction);

    // -- Edit menu ----------------------------------------------------

    d->selectAllAction = new KAction(i18n("Select All"), this);
    d->selectAllAction->setShortcut(Qt::CTRL+Qt::Key_A);
    connect(d->selectAllAction, SIGNAL(triggered()), d->view, SLOT(slotSelectAll()));
    actionCollection()->addAction("cameraui_selectall", d->selectAllAction);

    // -----------------------------------------------------------------

    d->selectNoneAction = new KAction(i18n("Select None"), this);
    d->selectNoneAction->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_A);
    connect(d->selectNoneAction, SIGNAL(triggered()), d->view, SLOT(slotSelectNone()));
    actionCollection()->addAction("cameraui_selectnone", d->selectNoneAction);

    // -----------------------------------------------------------------

    d->selectInvertAction = new KAction(i18n("Invert Selection"), this);
    d->selectInvertAction->setShortcut(Qt::CTRL+Qt::Key_Asterisk);
    connect(d->selectInvertAction, SIGNAL(triggered()), d->view, SLOT(slotSelectInvert()));
    actionCollection()->addAction("cameraui_selectinvert", d->selectInvertAction);

    // -----------------------------------------------------------

    d->selectNewItemsAction = new KAction(KIcon("document-new"), i18n("Select New Items"), this);
    connect(d->selectNewItemsAction, SIGNAL(triggered()), d->view, SLOT(slotSelectNew()));
    actionCollection()->addAction("cameraui_selectnewitems", d->selectNewItemsAction);

    // -----------------------------------------------------------

    d->selectLockedItemsAction = new KAction(KIcon("object-locked"), i18n("Select Locked Items"), this);
    connect(d->selectLockedItemsAction, SIGNAL(triggered()), d->view, SLOT(slotSelectLocked()));
    actionCollection()->addAction("cameraui_selectlockeditems", d->selectLockedItemsAction);

    // -- Image menu ---------------------------------------------

    d->imageViewAction = new KAction(KIcon("editimage"), i18nc("View the selected image", "View"), this);
    connect(d->imageViewAction, SIGNAL(triggered()), this, SLOT(slotFileView()));
    actionCollection()->addAction("cameraui_imageview", d->imageViewAction);
    d->imageViewAction->setEnabled(false);

    // -----------------------------------------------------------------

    d->downloadSelectedAction = new KAction(KIcon("computer"), i18n("Download Selected"), this);
    connect(d->downloadSelectedAction, SIGNAL(triggered()), this, SLOT(slotDownloadSelected()));
    actionCollection()->addAction("cameraui_imagedownloadselected", d->downloadSelectedAction);
    d->downloadSelectedAction->setEnabled(false);

    // -----------------------------------------------------------------

    d->downloadAllAction = new KAction(i18n("Download All"), this);
    connect(d->downloadAllAction, SIGNAL(triggered()), this, SLOT(slotDownloadAll()));
    actionCollection()->addAction("cameraui_imagedownloadall", d->downloadAllAction);

    // -----------------------------------------------------------------

    d->downloadDelSelectedAction = new KAction(i18n("Download/Delete Selected"), this);
    connect(d->downloadDelSelectedAction, SIGNAL(triggered()), this, SLOT(slotDownloadAndDeleteSelected()));
    actionCollection()->addAction("cameraui_imagedownloaddeleteselected", d->downloadDelSelectedAction);
    d->downloadDelSelectedAction->setEnabled(false);

    // -------------------------------------------------------------------------

    d->downloadDelAllAction = new KAction(i18n("Download/Delete All"), this);
    connect(d->downloadDelAllAction, SIGNAL(triggered()), this, SLOT(slotDownloadAndDeleteAll()));
    actionCollection()->addAction("cameraui_imagedownloaddeleteall", d->downloadDelAllAction);

    // -------------------------------------------------------------------------

    d->uploadAction = new KAction(KIcon("media-flash-smart-media"), i18n("Upload..."), this);
    connect(d->uploadAction, SIGNAL(triggered()), this, SLOT(slotUpload()));
    actionCollection()->addAction("cameraui_imageupload", d->uploadAction);

    // -------------------------------------------------------------------------

    d->lockAction = new KAction(KIcon("object-locked"), i18n("Toggle Lock"), this);
    connect(d->lockAction, SIGNAL(triggered()), this, SLOT(slotToggleLock()));
    actionCollection()->addAction("cameraui_imagelock", d->lockAction);

    // -------------------------------------------------------------------------

    d->deleteSelectedAction = new KAction(KIcon("edit-delete"), i18n("Delete Selected"), this);
    connect(d->deleteSelectedAction, SIGNAL(triggered()), this, SLOT(slotDeleteSelected()));
    actionCollection()->addAction("cameraui_imagedeleteselected", d->deleteSelectedAction);
    d->deleteSelectedAction->setEnabled(false);

    // -------------------------------------------------------------------------

    d->deleteAllAction = new KAction(i18n("Delete All"), this);
    connect(d->deleteAllAction, SIGNAL(triggered()), this, SLOT(slotDeleteAll()));
    actionCollection()->addAction("cameraui_imagedeleteall", d->deleteAllAction);

    // -- Standard 'View' menu actions ---------------------------------------------

    d->increaseThumbsAction = KStandardAction::zoomIn(this, SLOT(slotIncreaseThumbSize()), this);
    d->increaseThumbsAction->setEnabled(false);
    actionCollection()->addAction("cameraui_zoomplus", d->increaseThumbsAction);

    d->decreaseThumbsAction = KStandardAction::zoomOut(this, SLOT(slotDecreaseThumbSize()), this);
    d->decreaseThumbsAction->setEnabled(false);
    actionCollection()->addAction("cameraui_zoomminus", d->decreaseThumbsAction);

    d->fullScreenAction = actionCollection()->addAction(KStandardAction::FullScreen,
                          "cameraui_fullscreen", this, SLOT(slotToggleFullScreen()));

    // -- Standard 'Configure' menu actions ----------------------------------------

    d->showMenuBarAction = KStandardAction::showMenubar(this, SLOT(slotShowMenuBar()), actionCollection());

    KStandardAction::keyBindings(this, SLOT(slotEditKeys()),           actionCollection());
    KStandardAction::configureToolbars(this, SLOT(slotConfToolbars()), actionCollection());
    KStandardAction::preferences(this, SLOT(slotSetup()),              actionCollection());

    // ---------------------------------------------------------------------------------

    d->themeMenuAction = new KSelectAction(i18n("&Themes"), this);
    connect(d->themeMenuAction, SIGNAL(triggered(const QString&)),
            this, SLOT(slotChangeTheme(const QString&)));
    actionCollection()->addAction("theme_menu", d->themeMenuAction);

    d->themeMenuAction->setItems(ThemeEngine::instance()->themeNames());
    slotThemeChanged();

    // -- Standard 'Help' menu actions ---------------------------------------------

    d->donateMoneyAction = new KAction(i18n("Donate..."), this);
    connect(d->donateMoneyAction, SIGNAL(triggered()), this, SLOT(slotDonateMoney()));
    actionCollection()->addAction("cameraui_donatemoney", d->donateMoneyAction);

    d->contributeAction = new KAction(i18n("Contribute..."), this);
    connect(d->contributeAction, SIGNAL(triggered()), this, SLOT(slotContribute()));
    actionCollection()->addAction("cameraui_contribute", d->contributeAction);

    d->rawCameraListAction = new KAction(KIcon("kdcraw"), i18n("Supported RAW Cameras"), this);
    connect(d->rawCameraListAction, SIGNAL(triggered()), this, SLOT(slotRawCameraList()));
    actionCollection()->addAction("cameraui_rawcameralist", d->rawCameraListAction);

    d->libsInfoAction = new KAction(KIcon("help-about"), i18n("Components Information"), this);
    connect(d->libsInfoAction, SIGNAL(triggered()), this, SLOT(slotComponentsInfo()));
    actionCollection()->addAction("cameraui_librariesinfo", d->libsInfoAction);

    // Provides a menu entry that allows showing/hiding the toolbar(s)
    setStandardToolBarMenuEnabled(true);

    // Provides a menu entry that allows showing/hiding the statusbar
    createStandardStatusBarAction();

    // -- Keyboard-only actions added to <MainWindow> ------------------------------

    KAction *exitFullscreenAction = new KAction(i18n("Exit Fullscreen mode"), this);
    actionCollection()->addAction("cameraui_exitfullscreen", exitFullscreenAction);
    exitFullscreenAction->setShortcut( QKeySequence(Qt::Key_Escape) );
    connect(exitFullscreenAction, SIGNAL(triggered()), this, SLOT(slotEscapePressed()));

    KAction *altBackwardAction = new KAction(i18n("Previous Image"), this);
    actionCollection()->addAction("cameraui_backward_shift_space", altBackwardAction);
    altBackwardAction->setShortcut( KShortcut(Qt::SHIFT+Qt::Key_Space) );
    connect(altBackwardAction, SIGNAL(triggered()), this, SLOT(slotPrevItem()));

    // ---------------------------------------------------------------------------------

    d->anim = new DLogoAction(this);
    actionCollection()->addAction("logo_action", d->anim);

    createGUI("cameraui.rc");

    d->showMenuBarAction->setChecked(!menuBar()->isHidden());  // NOTE: workaround for B.K.O #171080
}

void CameraUI::setupConnections()
{
    connect(d->autoAlbumDateCheck, SIGNAL(toggled(bool)),
            d->folderDateFormat, SLOT(setEnabled(bool)));

    connect(d->autoAlbumDateCheck, SIGNAL(toggled(bool)),
            d->folderDateLabel, SLOT(setEnabled(bool)));

    connect(d->convertJpegCheck, SIGNAL(toggled(bool)),
            d->losslessFormat, SLOT(setEnabled(bool)));

    connect(d->convertJpegCheck, SIGNAL(toggled(bool)),
            d->formatLabel, SLOT(setEnabled(bool)));

    connect(d->convertJpegCheck, SIGNAL(toggled(bool)),
            d->view, SLOT(slotDownloadNameChanged()));

    connect(d->fixDateTimeCheck, SIGNAL(toggled(bool)),
            d->dateTimeEdit, SLOT(setEnabled(bool)));

    // -------------------------------------------------------------------------

    connect(d->view, SIGNAL(signalSelected(CameraIconItem*, bool)),
            this, SLOT(slotItemsSelected(CameraIconItem*, bool)));

    connect(d->view, SIGNAL(signalFileView(CameraIconItem*)),
            this, SLOT(slotFileView(CameraIconItem*)));

    connect(d->view, SIGNAL(signalUpload(const KUrl::List&)),
            this, SLOT(slotUploadItems(const KUrl::List&)));

    connect(d->view, SIGNAL(signalDownload()),
            this, SLOT(slotDownloadSelected()));

    connect(d->view, SIGNAL(signalDownloadAndDelete()),
            this, SLOT(slotDownloadAndDeleteSelected()));

    connect(d->view, SIGNAL(signalDelete()),
            this, SLOT(slotDeleteSelected()));

    connect(d->view, SIGNAL(signalToggleLock()),
            this, SLOT(slotToggleLock()));

    connect(d->view, SIGNAL(signalNewSelection(bool)),
            this, SLOT(slotNewSelection(bool)));

    connect(d->view, SIGNAL(signalZoomOut()),
            this, SLOT(slotDecreaseThumbSize()));

    connect(d->view, SIGNAL(signalZoomIn()),
            this, SLOT(slotIncreaseThumbSize()));

    connect(d->view, SIGNAL(signalThumbSizeChanged(int)),
            this, SLOT(slotThumbSizeChanged(int)));

    // -------------------------------------------------------------------------

    connect(d->statusNavigateBar, SIGNAL(signalFirstItem()),
            this, SLOT(slotFirstItem()));

    connect(d->statusNavigateBar, SIGNAL(signalNextItem()),
            this, SLOT(slotNextItem()));

    connect(d->statusNavigateBar, SIGNAL(signalPrevItem()),
            this, SLOT(slotPrevItem()));

    connect(d->statusNavigateBar, SIGNAL(signalLastItem()),
            this, SLOT(slotLastItem()));

    // -------------------------------------------------------------------------

    connect(d->statusZoomBar, SIGNAL(signalZoomMinusClicked()),
           this, SLOT(slotDecreaseThumbSize()));

    connect(d->statusZoomBar, SIGNAL(signalZoomPlusClicked()),
           this, SLOT(slotIncreaseThumbSize()));

    connect(d->statusZoomBar, SIGNAL(signalZoomSliderChanged(int)),
           this, SLOT(slotZoomSliderChanged(int)));

    connect(this, SIGNAL(signalWindowHasMoved()),
            d->statusZoomBar, SLOT(slotUpdateTrackerPos()));

    connect(this, SIGNAL(signalWindowHasMoved()),
            d->renameCustomizer, SLOT(slotUpdateTrackerPos()));

    connect(this, SIGNAL(signalWindowLostFocus()),
            d->renameCustomizer, SLOT(slotHideToolTipTracker()));

    // -------------------------------------------------------------------------

    connect(CollectionManager::instance(), SIGNAL(locationStatusChanged(const CollectionLocation &, int)),
            this, SLOT(slotCollectionLocationStatusChanged(const CollectionLocation &, int)));

    connect(AlbumSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSidebarTabTitleStyleChanged()));
}

void CameraUI::setupStatusBar()
{
    d->statusProgressBar = new StatusProgressBar(statusBar());
    d->statusProgressBar->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    d->statusProgressBar->setMaximumHeight(fontMetrics().height()+2);
    statusBar()->addWidget(d->statusProgressBar, 100);

    //------------------------------------------------------------------------------

    d->cameraFreeSpace = new FreeSpaceWidget(statusBar(), 100);
    statusBar()->addWidget(d->cameraFreeSpace, 1);

    //------------------------------------------------------------------------------

    d->albumLibraryFreeSpace = new FreeSpaceWidget(statusBar(), 100);
    d->albumLibraryFreeSpace->setMode(FreeSpaceWidget::AlbumLibrary);
    statusBar()->addWidget(d->albumLibraryFreeSpace, 1);
    refreshCollectionFreeSpace();

    //------------------------------------------------------------------------------

    d->statusZoomBar = new StatusZoomBar(statusBar());
    d->statusZoomBar->setMaximumHeight(fontMetrics().height()+2);
    statusBar()->addPermanentWidget(d->statusZoomBar, 1);

    //------------------------------------------------------------------------------

    d->statusNavigateBar = new StatusNavigateBar(statusBar());
    d->statusNavigateBar->setMaximumHeight(fontMetrics().height()+2);
    statusBar()->addPermanentWidget(d->statusNavigateBar, 1);
}

void CameraUI::setupCameraController(const QString& model, const QString& port, const QString& path)
{
    d->controller = new CameraController(this, d->cameraTitle, model, port, path);

    if (d->controller->cameraDriverType() == DKCamera::GPhotoDriver)
    {
        d->cameraFreeSpace->setMode(FreeSpaceWidget::GPhotoCamera);
        connect(d->controller, SIGNAL(signalFreeSpace(unsigned long, unsigned long)),
                this, SLOT(slotCameraFreeSpaceInfo(unsigned long, unsigned long)));
    }
    else
    {
        d->cameraFreeSpace->setMode(FreeSpaceWidget::UMSCamera);
        d->cameraFreeSpace->setPath(d->controller->cameraPath());
    }

    connect(d->controller, SIGNAL(signalConnected(bool)),
            this, SLOT(slotConnected(bool)));

    connect(d->controller, SIGNAL(signalInfoMsg(const QString&)),
            d->statusProgressBar, SLOT(setProgressText(const QString&)));

    connect(d->controller, SIGNAL(signalErrorMsg(const QString&)),
            this, SLOT(slotErrorMsg(const QString&)));

    connect(d->controller, SIGNAL(signalCameraInformation(const QString&, const QString&, const QString&)),
            this, SLOT(slotCameraInformation(const QString&, const QString&, const QString&)));

    connect(d->controller, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(d->controller, SIGNAL(signalFolderList(const QStringList&)),
            this, SLOT(slotFolderList(const QStringList&)));

    connect(d->controller, SIGNAL(signalFileList(const GPItemInfoList&)),
            this, SLOT(slotFileList(const GPItemInfoList&)));

    connect(d->controller, SIGNAL(signalThumbnail(const QString&, const QString&, const QImage&)),
            this, SLOT(slotThumbnail(const QString&, const QString&, const QImage&)));

    connect(d->controller, SIGNAL(signalThumbnailFailed(const QString&, const QString&)),
            this, SLOT(slotThumbnailFailed(const QString&, const QString&)));

    connect(d->controller, SIGNAL(signalDownloaded(const QString&, const QString&, int)),
            this, SLOT(slotDownloaded(const QString&, const QString&, int)));

    connect(d->controller, SIGNAL(signalDownloadComplete(const QString&, const QString&, const QString&, const QString&)),
            this, SLOT(slotDownloadComplete(const QString&, const QString&, const QString&, const QString&)));

    connect(d->controller, SIGNAL(signalSkipped(const QString&, const QString&)),
            this, SLOT(slotSkipped(const QString&, const QString&)));

    connect(d->controller, SIGNAL(signalDeleted(const QString&, const QString&, bool)),
            this, SLOT(slotDeleted(const QString&, const QString&, bool)));

    connect(d->controller, SIGNAL(signalLocked(const QString&, const QString&, bool)),
            this, SLOT(slotLocked(const QString&, const QString&, bool)));

    connect(d->controller, SIGNAL(signalExifFromFile(const QString&, const QString&)),
            this, SLOT(slotExifFromFile(const QString&, const QString&)));

    connect(d->controller, SIGNAL(signalExifData(const QByteArray&)),
            this, SLOT(slotExifFromData(const QByteArray&)));

    connect(d->controller, SIGNAL(signalUploaded(const GPItemInfo&)),
            this, SLOT(slotUploaded(const GPItemInfo&)));
}

void CameraUI::setupAccelerators()
{
}

void CameraUI::readSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("Camera Settings");

    d->renameFileLabel->setExpanded(group.readEntry("RenameFile Expanded", false));
    d->autoAlbumLabel->setExpanded(group.readEntry("AutoAlbum Expanded", false));
    d->onFlyLabel->setExpanded(group.readEntry("OnFly Expanded", true));
    d->autoRotateCheck->setChecked(group.readEntry("AutoRotate", true));
    d->autoAlbumDateCheck->setChecked(group.readEntry("AutoAlbumDate", false));
    d->autoAlbumExtCheck->setChecked(group.readEntry("AutoAlbumExt", false));
    d->fixDateTimeCheck->setChecked(group.readEntry("FixDateTime", false));
    d->setPhotographerId->setChecked(group.readEntry("SetPhotographerId", false));
    d->setCredits->setChecked(group.readEntry("SetCredits", false));
    d->convertJpegCheck->setChecked(group.readEntry("ConvertJpeg", false));
    d->losslessFormat->setCurrentIndex(group.readEntry("LossLessFormat", 0));   // PNG by default
    d->folderDateFormat->setCurrentIndex(group.readEntry("FolderDateFormat", (int)CameraUIPriv::IsoDateFormat));

    d->view->setThumbnailSize(group.readEntry("ThumbnailSize", (int)ThumbnailSize::Large));

    d->splitter->restoreState(group);

    d->dateTimeEdit->setEnabled(d->fixDateTimeCheck->isChecked());
    d->losslessFormat->setEnabled(convertLosslessJpegFiles());
    d->formatLabel->setEnabled(convertLosslessJpegFiles());
    d->folderDateFormat->setEnabled(d->autoAlbumDateCheck->isChecked());
    d->folderDateLabel->setEnabled(d->autoAlbumDateCheck->isChecked());
}

void CameraUI::saveSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("Camera Settings");

    group.writeEntry("RenameFile Expanded", d->renameFileLabel->isExpanded());
    group.writeEntry("AutoAlbum Expanded",  d->autoAlbumLabel->isExpanded());
    group.writeEntry("OnFly Expanded",      d->onFlyLabel->isExpanded());
    group.writeEntry("AutoRotate",          d->autoRotateCheck->isChecked());
    group.writeEntry("AutoAlbumDate",       d->autoAlbumDateCheck->isChecked());
    group.writeEntry("AutoAlbumExt",        d->autoAlbumExtCheck->isChecked());
    group.writeEntry("FixDateTime",         d->fixDateTimeCheck->isChecked());
    group.writeEntry("SetPhotographerId",   d->setPhotographerId->isChecked());
    group.writeEntry("SetCredits",          d->setCredits->isChecked());
    group.writeEntry("ConvertJpeg",         convertLosslessJpegFiles());
    group.writeEntry("LossLessFormat",      d->losslessFormat->currentIndex());
    group.writeEntry("ThumbnailSize",       d->view->thumbnailSize());
    group.writeEntry("FolderDateFormat",    d->folderDateFormat->currentIndex());
    d->splitter->saveState(group);
    config->sync();
}

void CameraUI::slotProcessUrl(const QString& url)
{
    KToolInvocation::invokeBrowser(url);
}

bool CameraUI::isBusy() const
{
    return d->busy;
}

bool CameraUI::isClosed() const
{
    return d->closed;
}

bool CameraUI::autoRotateJpegFiles() const
{
    return d->autoRotateCheck->isChecked();
}

bool CameraUI::convertLosslessJpegFiles() const
{
    return d->convertJpegCheck->isChecked();
}

QString CameraUI::losslessFormat()
{
    return d->losslessFormat->currentText();
}

QString CameraUI::cameraTitle() const
{
    return d->cameraTitle;
}

void CameraUI::slotCancelButton()
{
    d->statusProgressBar->progressBarMode(StatusProgressBar::TextMode,
                                          i18n("Canceling current operation, please wait..."));
    d->controller->slotCancel();
    d->currentlyDeleting.clear();
    refreshFreeSpace();
}

void CameraUI::refreshFreeSpace()
{
    if (d->controller->cameraDriverType() == DKCamera::GPhotoDriver)
        d->controller->getFreeSpace();
    else
        d->cameraFreeSpace->refresh();
}

void CameraUI::closeEvent(QCloseEvent* e)
{
    if (dialogClosed())
        e->accept();
    else
        e->ignore();
}

void CameraUI::moveEvent(QMoveEvent *e)
{
    Q_UNUSED(e)
    emit signalWindowHasMoved();
}

bool CameraUI::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::WindowDeactivate:
            emit signalWindowLostFocus();
            break;
        default:
            break;
    }
    return KXmlGuiWindow::event(e);
}

void CameraUI::slotClose()
{
/*FIXME    if (dialogClosed())
        reject();*/
}

bool CameraUI::dialogClosed()
{
    if (d->closed)
        return true;

    if (isBusy())
    {
        if (KMessageBox::questionYesNo(this,
                                       i18n("Do you want to close the dialog "
                                            "and cancel the current operation?"))
            == KMessageBox::No)
            return false;
    }

    d->statusProgressBar->progressBarMode(StatusProgressBar::TextMode,
                                          i18n("Disconnecting from camera, please wait..."));

    if (isBusy())
    {
        d->controller->slotCancel();
        // will be read in slotBusy later and finishDialog
        // will be called only when everything is finished
        d->closed = true;
    }
    else
    {
        d->closed = true;
        finishDialog();
    }

    return true;
}

void CameraUI::finishDialog()
{
    // Look if an item have been downloaded to computer during camera GUI session.
    // If yes, update the lastAccess date property of camera in digiKam camera list.

    if (d->view->itemsDownloaded() > 0)
    {
        CameraList* clist = CameraList::defaultList();
        if (clist)
            clist->changeCameraAccessTime(d->cameraTitle, QDateTime::currentDateTime());
    }

    // When a directory is created, a watch is put on it to spot new files
    // but it can occur that the file is copied there before the watch is
    // completely setup. That is why as an extra safeguard run CollectionScanner
    // over the folders we used. Bug: 119201

    d->statusProgressBar->progressBarMode(StatusProgressBar::TextMode,
                                          i18n("Scanning for new files, please wait..."));
    foreach (const QString& folder, d->foldersToScan)
        ScanController::instance()->scheduleCollectionScan(folder);
    d->foldersToScan.clear();

    deleteLater();

    if(!d->lastDestURL.isEmpty())
        emit signalLastDestination(d->lastDestURL);

    saveSettings();
}

void CameraUI::slotBusy(bool val)
{
    if (!val)   // Camera is available for actions.
    {
        if (!d->busy)
            return;

        d->busy = false;
        d->cameraCancelAction->setEnabled(false);

        d->advBox->setEnabled(true);
        // B.K.O #127614: The Focus need to be restored in custom prefix widget.
        // commenting this out again: If we do not disable, no need to restore focus
        // d->renameCustomizer->restoreFocus();

        d->uploadAction->setEnabled(d->controller->cameraUploadSupport());
        d->downloadSelectedAction->setEnabled(true);
        d->downloadDelSelectedAction->setEnabled(d->controller->cameraDeleteSupport());
        d->downloadAllAction->setEnabled(true);
        d->downloadDelAllAction->setEnabled(d->controller->cameraDeleteSupport());
        d->deleteSelectedAction->setEnabled(d->controller->cameraDeleteSupport());
        d->deleteAllAction->setEnabled(d->controller->cameraDeleteSupport());
        d->selectAllAction->setEnabled(true);
        d->selectNoneAction->setEnabled(true);
        d->selectInvertAction->setEnabled(true);
        d->selectNewItemsAction->setEnabled(true);
        d->selectLockedItemsAction->setEnabled(true);
        d->lockAction->setEnabled(true);
        d->cameraInfoAction->setEnabled(true);
        d->cameraCaptureAction->setEnabled(d->controller->cameraCaptureImageSupport());

        d->anim->stop();
        d->statusProgressBar->progressBarMode(StatusProgressBar::TextMode, i18n("Ready"));

        // like WDestructiveClose, but after camera controller operation has safely finished
        if (d->closed)
        {
            finishDialog();
        }
    }
    else    // Camera is busy.
    {
        if (d->busy)
            return;

        if (!d->anim->running())
            d->anim->start();

        d->busy = true;
        d->cameraCancelAction->setEnabled(true);
        d->statusProgressBar->progressBarMode(StatusProgressBar::ProgressBarMode);

        // Settings tab is disabled in slotDownload, selectively when downloading
        // Fast dis/enabling would create the impression of flicker, e.g. when retrieving EXIF from camera
        //d->advBox->setEnabled(false);

        d->uploadAction->setEnabled(false);
        d->downloadSelectedAction->setEnabled(false);
        d->downloadDelSelectedAction->setEnabled(false);
        d->downloadAllAction->setEnabled(false);
        d->downloadDelAllAction->setEnabled(false);
        d->deleteSelectedAction->setEnabled(false);
        d->deleteAllAction->setEnabled(false);
        d->selectAllAction->setEnabled(false);
        d->selectNoneAction->setEnabled(false);
        d->selectInvertAction->setEnabled(false);
        d->selectNewItemsAction->setEnabled(false);
        d->selectLockedItemsAction->setEnabled(false);
        d->lockAction->setEnabled(false);
        d->cameraInfoAction->setEnabled(false);
        d->cameraCaptureAction->setEnabled(false);
    }
}

void CameraUI::slotIncreaseThumbSize()
{
    int thumbSize = d->view->thumbnailSize() + ThumbnailSize::Step;
    d->view->setThumbnailSize(thumbSize);
}

void CameraUI::slotDecreaseThumbSize()
{
    int thumbSize = d->view->thumbnailSize() - ThumbnailSize::Step;
    d->view->setThumbnailSize(thumbSize);
}

void CameraUI::slotZoomSliderChanged(int size)
{
    d->view->setThumbnailSize(size);
}

void CameraUI::slotThumbSizeChanged(int size)
{
    d->statusZoomBar->setZoomSliderValue(size);
    d->statusZoomBar->setZoomTrackerText(i18n("Size: %1", size));
    d->statusZoomBar->triggerZoomTrackerToolTip();

    d->statusZoomBar->setEnableZoomPlus(true);
    d->statusZoomBar->setEnableZoomMinus(true);
    d->increaseThumbsAction->setEnabled(true);
    d->decreaseThumbsAction->setEnabled(true);

    if (d->view->thumbnailSize() == ThumbnailSize::Small)
    {
        d->decreaseThumbsAction->setEnabled(false);
        d->statusZoomBar->setEnableZoomMinus(false);
    }

    if (d->view->thumbnailSize() == ThumbnailSize::Huge)
    {
        d->increaseThumbsAction->setEnabled(false);
        d->statusZoomBar->setEnableZoomPlus(false);
    }
}

void CameraUI::slotConnected(bool val)
{
    if (!val)
    {
      if (KMessageBox::warningYesNo(this,
                                    i18n("Failed to connect to the camera. "
                                         "Please make sure it is connected "
                                         "properly and turned on. "
                                         "Would you like to try again?"),
                                    i18n("Connection Failed"),
                                    KGuiItem(i18n("Retry")),
                                    KGuiItem(i18n("Abort")))
          == KMessageBox::Yes)
          QTimer::singleShot(0, d->controller, SLOT(slotConnect()));
      else
          close();
    }
    else
    {
        refreshFreeSpace();
        d->controller->listFolders();
    }
}

void CameraUI::slotFolderList(const QStringList& folderList)
{
    if (d->closed)
        return;

    d->statusProgressBar->setProgressValue(0);
    d->statusProgressBar->setProgressTotalSteps(0);

    d->cameraFolderList = folderList;
    for (QStringList::const_iterator it = folderList.constBegin();
         it != folderList.constEnd(); ++it)
    {
        d->controller->listFiles(*it);
    }
}

void CameraUI::slotFileList(const GPItemInfoList& fileList)
{
    if (d->closed)
        return;

    if (fileList.empty())
        return;

    AlbumSettings* settings = AlbumSettings::instance();
    if (!settings) return;

    // We sort the map by time stamp
    // and we remove internal camera files which are not image/video/sounds.
    QStringList fileNames, fileExts;
    QFileInfo   info;

    // JVC camera (see B.K.O #133185).
    fileNames.append("mgr_data");
    fileNames.append("pgr_mgr");

    // HP Photosmart camera (see B.K.O #156338).
    fileExts.append("dsp");

    // Minolta camera in PTP mode
    fileExts.append("dps");

    // NOTE: see B.K.O #181726: list of accepted file extensions from Album Settings.
    QStringList list = settings->getAllFileFilter().toLower().split(" ");

    QMultiMap<QDateTime, GPItemInfo> map;
    foreach(const GPItemInfo& item, fileList)
    {
        info.setFile(item.name);
        if (!fileNames.contains(info.fileName().toLower()) &&
            !fileExts.contains(info.suffix().toLower())    &&
            list.contains(QString("*.%1").arg(info.suffix().toLower())))
        {
            map.insertMulti(item.mtime, item);
        }
    }

    if (map.empty())
        return;

    QMultiMap<QDateTime, GPItemInfo>::iterator it = map.end();

    do
    {
        --it;
        GPItemInfo item = *it;

        // We query database to check if item have been already downloaded from camera.
        switch(DownloadHistory::status(d->controller->cameraMD5ID(), item.name, item.size, item.mtime))
        {
            case DownloadHistory::NotDownloaded:
                item.downloaded = GPItemInfo::NewPicture;
                break;
            case DownloadHistory::Downloaded:
                item.downloaded = GPItemInfo::DownloadedYes;
                break;
            default:      // DownloadHistory::StatusUnknown
                item.downloaded = GPItemInfo::DownloadUnknow;
                break;
        }
        d->view->addItem(item);
        d->controller->getThumbnail(item.folder, item.name);
    }
    while(it != map.begin());

    d->statusProgressBar->setProgressTotalSteps(d->statusProgressBar->progressTotalSteps() +
                                                fileList.count());
}

void CameraUI::slotThumbnail(const QString& folder, const QString& file,
                             const QImage& thumbnail)
{
    QImage thumb = thumbnail;

    if (thumb.isNull())
    {
        // This call must be run outside Camera Controller thread.
        thumb = d->controller->mimeTypeThumbnail(file).toImage();
    }

    d->view->setThumbnail(folder, file, thumb);
    int curr = d->statusProgressBar->progressValue();
    d->statusProgressBar->setProgressValue(curr+1);
}

void CameraUI::slotThumbnailFailed(const QString& folder, const QString& file)
{
    if (d->controller->cameraDriverType() == DKCamera::UMSDriver)
    {
        KUrl url = KUrl::fromPath(folder + QString("/") + file);
        d->kdeTodo << url;
        startKdePreviewJob();
    }
    else
    {
        // This call must be run outside Camera Controller thread.
        QImage thumb = d->controller->mimeTypeThumbnail(file).toImage();
        d->view->setThumbnail(folder, file, thumb);
    }

    int curr = d->statusProgressBar->progressValue();
    d->statusProgressBar->setProgressValue(curr+1);
}

void CameraUI::startKdePreviewJob()
{
    if (d->kdeJob || d->kdeTodo.isEmpty())
        return;

    KUrl::List list = d->kdeTodo;
    d->kdeTodo.clear();
    d->kdeJob = KIO::filePreview(list, 256);

    connect(d->kdeJob, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
            this, SLOT(slotGotKDEPreview(const KFileItem&, const QPixmap&)));

    connect(d->kdeJob, SIGNAL(failed(const KFileItem&)),
            this, SLOT(slotFailedKDEPreview(const KFileItem&)));

    connect(d->kdeJob, SIGNAL(finished(KJob*)),
            this, SLOT(slotKdePreviewFinished(KJob*)));
}

void CameraUI::slotGotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    QString file   = item.url().fileName();
    QString folder = item.url().path().remove(QString("/") + file);
    QImage thumb   = pix.toImage();
    if (thumb.isNull())
    {
        // This call must be run outside Camera Controller thread.
        thumb = d->controller->mimeTypeThumbnail(file).toImage();
    }
    d->view->setThumbnail(folder, file, thumb);
}

void CameraUI::slotFailedKDEPreview(const KFileItem& item)
{
    QString file   = item.url().fileName();
    QString folder = item.url().path().remove(QString("/") + file);
    QImage thumb   = d->controller->mimeTypeThumbnail(file).toImage();
    d->view->setThumbnail(folder, file, thumb);
}

void CameraUI::slotKdePreviewFinished(KJob*)
{
    d->kdeJob = 0;
}

void CameraUI::slotCapture()
{
    if (d->busy)
        return;

    CaptureDlg *captureDlg = new CaptureDlg(this, d->controller, d->cameraTitle);
    captureDlg->show();
}

void CameraUI::slotInformation()
{
    if (d->busy)
        return;

    d->controller->getCameraInformation();
}

void CameraUI::slotCameraInformation(const QString& summary, const QString& manual, const QString& about)
{
    CameraInfoDialog *infoDlg = new CameraInfoDialog(this, summary, manual, about);
    infoDlg->show();
}

void CameraUI::slotErrorMsg(const QString& msg)
{
    KMessageBox::error(this, msg);
}

void CameraUI::slotUpload()
{
    if (d->busy)
        return;

    QString fileformats;

    QStringList patternList = KImageIO::pattern(KImageIO::Reading).split('\n');

    // All Images from list must been always the first entry given by KDE API
    QString allPictures = patternList[0];

    // Add RAW file format to All Images" type mime and replace current.
#if KDCRAW_VERSION < 0x000400
    allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::DcrawBinary::instance()->rawFiles()));
#else
    allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::KDcraw::rawFiles()));
#endif
    patternList.removeAll(patternList[0]);
    patternList.prepend(allPictures);

    // Added RAW file formats supported by dcraw program like a type mime.
    // Note: we cannot use here "image/x-raw" type mime from KDE because it incomplete
    // or unavailable(dcraw_0)(see file #121242 in B.K.O).
#if KDCRAW_VERSION < 0x000400
    patternList.append(QString("\n%1|Camera RAW files").arg(QString(KDcrawIface::DcrawBinary::instance()->rawFiles())));
#else
    patternList.append(QString("\n%1|Camera RAW files").arg(QString(KDcrawIface::KDcraw::rawFiles())));
#endif
    fileformats = patternList.join("\n");

    kDebug(50003) << "fileformats=" << fileformats << endl;

    KUrl::List urls = KFileDialog::getOpenUrls(CollectionManager::instance()->oneAlbumRootPath(),
                                               fileformats, this, i18n("Select Image to Upload"));
    if (!urls.isEmpty())
        slotUploadItems(urls);
}

void CameraUI::slotUploadItems(const KUrl::List& urls)
{
    if (d->busy)
        return;

    if (urls.isEmpty())
        return;

    if (d->cameraFreeSpace->isValid())
    {
        // Check if space require to upload new items in camera is enough.
        quint64 totalKbSize = 0;
        for (KUrl::List::const_iterator it = urls.constBegin() ; it != urls.constEnd() ; ++it)
        {
            QFileInfo fi((*it).path());
            totalKbSize += fi.size()/1024;
        }

        if (totalKbSize >= d->cameraFreeSpace->kBAvail())
        {
            KMessageBox::error(this, i18n("There is not enough free space on the Camera Medium "
                                          "to upload pictures.\n\n"
                                          "Space require: %1\n"
                                          "Available free space: %2",
                                    KIO::convertSizeFromKiB(totalKbSize),
                                    KIO::convertSizeFromKiB(d->cameraFreeSpace->kBAvail())));
            return;
        }
    }

    CameraFolderDialog dlg(this, d->view, d->cameraFolderList,
                           d->controller->cameraTitle(),
                           d->controller->cameraPath());

    if (dlg.exec() != QDialog::Accepted)
        return;

    QString cameraFolder = dlg.selectedFolderPath();

    for (KUrl::List::const_iterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        QFileInfo fi((*it).path());
        if (!fi.exists()) continue;
        if (fi.isDir()) continue;

        QString ext  = QString(".") + fi.completeSuffix();
        QString name = fi.fileName();
        name.truncate(fi.fileName().length() - ext.length());

        bool ok;

        while (d->view->findItem(cameraFolder, name + ext))
        {
            QString msg(i18n("Camera Folder <b>%1</b> already contains the item <b>%2</b>.<br/>"
                             "Please enter a new filename (without extension):",
                             cameraFolder, fi.fileName()));
            name = KInputDialog::getText(i18n("File already exists"), msg, name, &ok, this);

            if (!ok)
                return;
        }

        d->controller->upload(fi, name + ext, cameraFolder);
    }
}

void CameraUI::slotUploaded(const GPItemInfo& itemInfo)
{
    if (d->closed)
        return;

    d->view->addItem(itemInfo);
    d->controller->getThumbnail(itemInfo.folder, itemInfo.name);
    refreshFreeSpace();
}

void CameraUI::slotDownloadSelected()
{
    slotDownload(true, false);
}

void CameraUI::slotDownloadAndDeleteSelected()
{
    slotDownload(true, true);
}

void CameraUI::slotDownloadAll()
{
    slotDownload(false, false);
}

void CameraUI::slotDownloadAndDeleteAll()
{
    slotDownload(false, true);
}

void CameraUI::slotDownload(bool onlySelected, bool deleteAfter, Album *album)
{
    // See B.K.O #143934: force to select all items to prevent problem
    // when !renameCustomizer->useDefault() ==> iconItem->getDownloadName()
    // can return an empty string in this case because it depends on selection.
    if (!onlySelected)
        d->view->slotSelectAll();

    QString   newDirName;
    IconItem* firstItem = d->view->firstItem();
    if (firstItem)
    {
        CameraIconItem* iconItem = static_cast<CameraIconItem*>(firstItem);

        QDateTime dateTime = iconItem->itemInfo()->mtime;

        switch(d->folderDateFormat->currentIndex())
        {
            case CameraUIPriv::TextDateFormat:
                newDirName = dateTime.date().toString(Qt::TextDate);
                break;
            case CameraUIPriv::LocalDateFormat:
                newDirName = dateTime.date().toString(Qt::LocalDate);
                break;
            default:        // IsoDateFormat
                newDirName = dateTime.date().toString(Qt::ISODate);
                break;
        }
    }

    // -- Get the destination album from digiKam library ---------------

    if (!album)
    {
        AlbumManager* man = AlbumManager::instance();

        album = man->currentAlbum();
        if (album && album->type() != Album::PHYSICAL)
            album = 0;

        QString header(i18n("<p>Please select the destination album from the digiKam library to "
                            "import the camera pictures into.</p>"));

        album = AlbumSelectDialog::selectAlbum(this, (PAlbum*)album, header, newDirName,
                                               d->autoAlbumDateCheck->isChecked());

        if (!album) return;
    }

    PAlbum *pAlbum = dynamic_cast<PAlbum*>(album);
    if (!pAlbum) return;

    // -- Check disk space ------------------------

    // See B.K.O #139519: Always check free space available before to
    // download items selection from camera.
    unsigned long fSize = 0;
    unsigned long dSize = 0;
    d->view->itemsSelectionSizeInfo(fSize, dSize);
    QString albumRootPath = pAlbum->albumRootPath();
    unsigned long kBAvail = d->albumLibraryFreeSpace->kBAvail(albumRootPath);
    if (dSize >= kBAvail)
    {
        KGuiItem cont = KStandardGuiItem::cont();
        cont.setText(i18n("Try Anyway"));
        KGuiItem cancel = KStandardGuiItem::cancel();
        cancel.setText(i18n("Cancel Download"));
        int result =
        KMessageBox::warningYesNo(this,
                                  i18n("There is not enough free space on the disk of the album you selected "
                                       "to download and process the selected pictures from the camera.\n\n"
                                       "Estimated space required: %1\n"
                                       "Available free space: %2",
                                       KIO::convertSizeFromKiB(dSize),
                                       KIO::convertSizeFromKiB(kBAvail)),
                                  i18n("Insufficient Disk Space"),
                                  cont, cancel);
        if (result == KMessageBox::No)
            return;
    }

    // -- Prepare downloading of camera items ------------------------

    KUrl url = pAlbum->fileUrl();

    d->controller->downloadPrep();

    DownloadSettingsContainer downloadSettings;
    QString   downloadName;
    QDateTime dateTime;
    int       total = 0;

    downloadSettings.autoRotate        = d->autoRotateCheck->isChecked();
    downloadSettings.fixDateTime       = d->fixDateTimeCheck->isChecked();
    downloadSettings.newDateTime       = d->dateTimeEdit->dateTime();
    downloadSettings.setPhotographerId = d->setPhotographerId->isChecked();
    downloadSettings.setCredits        = d->setCredits->isChecked();
    downloadSettings.convertJpeg       = convertLosslessJpegFiles();
    downloadSettings.losslessFormat    = losslessFormat();

    AlbumSettings* settings = AlbumSettings::instance();
    if (settings)
    {
        downloadSettings.author      = settings->getAuthor();
        downloadSettings.authorTitle = settings->getAuthorTitle();
        downloadSettings.credit      = settings->getCredit();
        downloadSettings.source      = settings->getSource();
        downloadSettings.copyright   = settings->getCopyright();
    }

    // -- Download camera items -------------------------------

    QSet<QString> usedDownloadPaths;

    for (IconItem* item = d->view->firstItem(); item;
         item = item->nextItem())
    {
        if (onlySelected && !(item->isSelected()))
            continue;

        CameraIconItem* iconItem = static_cast<CameraIconItem*>(item);
        downloadSettings.folder      = iconItem->itemInfo()->folder;
        downloadSettings.file        = iconItem->itemInfo()->name;
        downloadName                 = iconItem->getDownloadName();
        dateTime                     = iconItem->itemInfo()->mtime;

        KUrl downloadUrl(url);
        QString errMsg;

        // Auto sub-albums creation based on file date.

        if (d->autoAlbumDateCheck->isChecked())
        {
            QString dirName;

            switch(d->folderDateFormat->currentIndex())
            {
                case CameraUIPriv::TextDateFormat:
                    dirName = dateTime.date().toString(Qt::TextDate);
                    break;
                case CameraUIPriv::LocalDateFormat:
                    dirName = dateTime.date().toString(Qt::LocalDate);
                    break;
                default:        // IsoDateFormat
                    dirName = dateTime.date().toString(Qt::ISODate);
                    break;
            }

            // See B.K.O #136927 : we need to support file system which do not
            // handle upper case properly.
            dirName = dirName.toLower();
            if (!createAutoAlbum(downloadUrl, dirName, dateTime.date(), errMsg))
            {
                KMessageBox::error(this, errMsg);
                return;
            }

            downloadUrl.addPath(dirName);
        }

        // Auto sub-albums creation based on file extensions.

        if (d->autoAlbumExtCheck->isChecked())
        {
            // We use the target file name to compute sub-albums name to take a care about
            // conversion on the fly option.
            QFileInfo fi(downloadName);

            QString subAlbum = fi.suffix().toUpper();
            if (fi.suffix().toUpper() == QString("JPEG") ||
                fi.suffix().toUpper() == QString("JPE"))
                subAlbum = QString("JPG");
            if (fi.suffix().toUpper() == QString("TIFF"))
                subAlbum = QString("TIF");
            if (fi.suffix().toUpper() == QString("MPEG") ||
                fi.suffix().toUpper() == QString("MPE") ||
                fi.suffix().toUpper() == QString("MPO"))
                subAlbum = QString("MPG");

            // See B.K.O #136927 : we need to support file system which do not
            // handle upper case properly.
            subAlbum = subAlbum.toLower();
            if (!createAutoAlbum(downloadUrl, subAlbum, dateTime.date(), errMsg))
            {
                KMessageBox::error(this, errMsg);
                return;
            }

            downloadUrl.addPath(subAlbum);
        }

        d->foldersToScan << downloadUrl.path();

        if (downloadName.isEmpty())
        {
            downloadUrl.addPath(downloadSettings.file);
        }
        else
        {
            // when using custom renaming (e.g. by date, see bug 179902)
            // make sure that we create unique names
            downloadUrl.addPath(downloadName);
            QString suggestedPath = downloadUrl.path();
            if (usedDownloadPaths.contains(suggestedPath))
            {
                QFileInfo fi(downloadName);
                QString suffix = "." + fi.suffix();
                QString pathWithoutSuffix(suggestedPath);
                pathWithoutSuffix.chop(suffix.length());
                QString currentVariant;
                int counter = 1;
                do
                    currentVariant = pathWithoutSuffix + "-" + QString::number(counter++) + suffix;
                while (usedDownloadPaths.contains(currentVariant));

                usedDownloadPaths << currentVariant;
                downloadUrl = KUrl(currentVariant);
            }
            else
                usedDownloadPaths << suggestedPath;
        }

        downloadSettings.dest = downloadUrl.path();

        d->controller->download(downloadSettings);
        ++total;
    }

    if (total <= 0)
        return;

    d->lastDestURL = url;
    d->statusProgressBar->setProgressValue(0);
    d->statusProgressBar->setProgressTotalSteps(total);
    d->statusProgressBar->progressBarMode(StatusProgressBar::ProgressBarMode);

    // disable settings tab here instead of slotBusy:
    // Only needs to be disabled while downloading
    d->advBox->setEnabled(false);

    if (deleteAfter)
    {
        if (onlySelected)
            slotDeleteSelected();
        else
            slotDeleteAll();
    }
}

void CameraUI::slotDownloaded(const QString& folder, const QString& file, int status)
{
    CameraIconItem* iconItem = d->view->findItem(folder, file);
    if (iconItem)
    {
        iconItem->setDownloaded(status);

        //if (iconItem->isSelected())
          //  slotItemsSelected(iconItem, true);

        if (status == GPItemInfo::DownloadedYes)
        {
            int curr = d->statusProgressBar->progressValue();
            d->statusProgressBar->setProgressValue(curr+1);

            DownloadHistory::setDownloaded(d->controller->cameraMD5ID(),
                                           iconItem->itemInfo()->name,
                                           iconItem->itemInfo()->size,
                                           iconItem->itemInfo()->mtime);
        }
    }
}

void CameraUI::slotDownloadComplete(const QString&, const QString&,
                                    const QString& destFolder, const QString&)
{
    ScanController::instance()->scheduleCollectionScanRelaxed(destFolder);
}

void CameraUI::slotSkipped(const QString& folder, const QString& file)
{
    CameraIconItem* iconItem = d->view->findItem(folder, file);
    if (iconItem)
        iconItem->setDownloaded(GPItemInfo::DownloadedNo);

    int curr = d->statusProgressBar->progressValue();
    d->statusProgressBar->setProgressValue(curr+1);
}

void CameraUI::slotToggleLock()
{
    int count = 0;
    for (IconItem* item = d->view->firstItem(); item;
         item = item->nextItem())
    {
        CameraIconItem* iconItem = static_cast<CameraIconItem*>(item);
        if (iconItem->isSelected())
        {
            QString folder = iconItem->itemInfo()->folder;
            QString file   = iconItem->itemInfo()->name;
            int writePerm  = iconItem->itemInfo()->writePermissions;
            bool lock      = true;

            // If item is currently locked, unlock it.
            if (writePerm == 0)
                lock = false;

            d->controller->lockFile(folder, file, lock);
            ++count;
        }
    }

    if (count > 0)
    {
        d->statusProgressBar->setProgressValue(0);
        d->statusProgressBar->setProgressTotalSteps(count);
        d->statusProgressBar->progressBarMode(StatusProgressBar::ProgressBarMode);
    }
}

void CameraUI::slotLocked(const QString& folder, const QString& file, bool status)
{
    if (status)
    {
        CameraIconItem* iconItem = d->view->findItem(folder, file);
        if (iconItem)
        {
            iconItem->toggleLock();
            //if (iconItem->isSelected())
              //  slotItemsSelected(iconItem, true);
        }
    }

    int curr = d->statusProgressBar->progressValue();
    d->statusProgressBar->setProgressValue(curr+1);
}

void CameraUI::slotDeleteSelected()
{
    QStringList folders;
    QStringList files;
    QStringList deleteList;
    QStringList lockedList;

    for (IconItem* item = d->view->firstItem(); item;
         item = item->nextItem())
    {
        CameraIconItem* iconItem = static_cast<CameraIconItem*>(item);
        if (iconItem->isSelected())
        {
            if (iconItem->itemInfo()->writePermissions != 0)  // Item not locked ?
            {
                QString folder = iconItem->itemInfo()->folder;
                QString file   = iconItem->itemInfo()->name;
                folders.append(folder);
                files.append(file);
                deleteList.append(folder + QString("/") + file);
            }
            else
            {
                lockedList.append(iconItem->itemInfo()->name);
            }
        }
    }

    // If we want to delete some locked files, just give a feedback to user.
    if (!lockedList.isEmpty())
    {
        QString infoMsg(i18n("The items listed below are locked by camera (read-only). "
                             "These items will not be deleted. If you really want to delete these items, "
                             "please unlock them and try again."));
        KMessageBox::informationList(this, infoMsg, lockedList, i18n("Information"));
    }

    if (folders.isEmpty())
        return;

    QString warnMsg(i18np("About to delete this image. "
                          "Deleted file is unrecoverable. "
                          "Are you sure?",
                          "About to delete these %1 images. "
                          "Deleted files are unrecoverable. "
                          "Are you sure?",
                          deleteList.count()));
    if (KMessageBox::warningContinueCancelList(this, warnMsg,
                                               deleteList,
                                               i18n("Warning"),
                                               KGuiItem(i18n("Delete")))
        ==  KMessageBox::Continue)
    {
        QStringList::iterator itFolder = folders.begin();
        QStringList::iterator itFile   = files.begin();

        d->statusProgressBar->setProgressValue(0);
        d->statusProgressBar->setProgressTotalSteps(deleteList.count());
        d->statusProgressBar->progressBarMode(StatusProgressBar::ProgressBarMode);

        for ( ; itFolder != folders.end(); ++itFolder, ++itFile)
        {
            d->controller->deleteFile(*itFolder, *itFile);
            // the currentlyDeleting list is used to prevent loading items which
            // will immanently be deleted into the sidebar and wasting time
            d->currentlyDeleting.append(*itFolder + *itFile);
        }
    }
}

void CameraUI::slotDeleteAll()
{
    QStringList folders;
    QStringList files;
    QStringList deleteList;
    QStringList lockedList;

    for (IconItem* item = d->view->firstItem(); item;
         item = item->nextItem())
    {
        CameraIconItem* iconItem = static_cast<CameraIconItem*>(item);
        if (iconItem->itemInfo()->writePermissions != 0)  // Item not locked ?
        {
            QString folder = iconItem->itemInfo()->folder;
            QString file   = iconItem->itemInfo()->name;
            folders.append(folder);
            files.append(file);
            deleteList.append(folder + QString("/") + file);
        }
        else
        {
            lockedList.append(iconItem->itemInfo()->name);
        }
    }

    // If we want to delete some locked files, just give a feedback to user.
    if (!lockedList.isEmpty())
    {
        QString infoMsg(i18n("The items listed below are locked by camera (read-only). "
                             "These items will not be deleted. If you really want to delete these items, "
                             "please unlock them and try again."));
        KMessageBox::informationList(this, infoMsg, lockedList, i18n("Information"));
    }

    if (folders.isEmpty())
        return;

    QString warnMsg(i18np("About to delete this image. "
                          "Deleted file is unrecoverable. "
                          "Are you sure?",
                          "About to delete these %1 images. "
                          "Deleted files are unrecoverable. "
                          "Are you sure?",
                          deleteList.count()));
    if (KMessageBox::warningContinueCancelList(this, warnMsg,
                                               deleteList,
                                               i18n("Warning"),
                                               KGuiItem(i18n("Delete")))
        ==  KMessageBox::Continue)
    {
        QStringList::iterator itFolder = folders.begin();
        QStringList::iterator itFile   = files.begin();

        d->statusProgressBar->setProgressValue(0);
        d->statusProgressBar->setProgressTotalSteps(deleteList.count());
        d->statusProgressBar->progressBarMode(StatusProgressBar::ProgressBarMode);

        for ( ; itFolder != folders.end(); ++itFolder, ++itFile)
        {
            d->controller->deleteFile(*itFolder, *itFile);
            d->currentlyDeleting.append(*itFolder + *itFile);
        }
    }
}

void CameraUI::slotDeleted(const QString& folder, const QString& file, bool status)
{
    if (status)
    {
        d->view->removeItem(folder, file);
        // do this after removeItem, which will signal to slotItemsSelected, which checks for the list
        d->currentlyDeleting.removeAll(folder + file);
    }

    int curr = d->statusProgressBar->progressValue();
    d->statusProgressBar->setProgressTotalSteps(curr+1);
    refreshFreeSpace();
}

void CameraUI::slotFileView()
{
    CameraIconItem* item = d->view->firstItemSelected();
    if (item)
        slotFileView(item);
}

void CameraUI::slotFileView(CameraIconItem* item)
{
    d->controller->openFile(item->itemInfo()->folder, item->itemInfo()->name);
}

void CameraUI::slotExifFromFile(const QString& folder, const QString& file)
{
    CameraIconItem* item = d->view->findItem(folder, file);
    if (!item)
        return;

    d->rightSideBar->itemChanged(item->itemInfo(), folder + QString("/") + file,
                                 QByteArray(), d->view, item);
}

void CameraUI::slotExifFromData(const QByteArray& exifData)
{
    CameraIconItem* item = dynamic_cast<CameraIconItem*>(d->view->currentItem());

    if (!item)
        return;

    KUrl url(item->itemInfo()->folder + '/' + item->itemInfo()->name);

    // Sometimes, GPhoto2 drivers return complete APP1 JFIF section. Exiv2 cannot
    // decode (yet) exif metadata from APP1. We will find Exif header to get data at this place
    // to please with Exiv2...

    kDebug(50003) << "Size of Exif metadata from camera = " << exifData.size() << endl;
    char exifHeader[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };

    if (!exifData.isEmpty())
    {
        int i = exifData.indexOf(*exifHeader);
        if (i != -1)
        {
            kDebug(50003) << "Exif header found at position " << i << endl;
            i = i + sizeof(exifHeader);
            QByteArray data;
            data.resize(exifData.size()-i);
            memcpy(data.data(), exifData.data()+i, data.size());
            d->rightSideBar->itemChanged(item->itemInfo(), url, data, d->view, item);
            return;
        }
    }

    d->rightSideBar->itemChanged(item->itemInfo(), url, exifData, d->view, item);
}

void CameraUI::slotNewSelection(bool hasSelection)
{
    if (!d->controller) return;

    if (!d->renameCustomizer->useDefault())
    {
        d->downloadSelectedAction->setEnabled(hasSelection);
        d->downloadDelSelectedAction->setEnabled(hasSelection & d->controller->cameraDeleteSupport());
        d->deleteSelectedAction->setEnabled(hasSelection & d->controller->cameraDeleteSupport());
        d->imageViewAction->setEnabled(hasSelection);
    }
    else
    {
        d->downloadSelectedAction->setEnabled(hasSelection);
        d->downloadDelSelectedAction->setEnabled(hasSelection & d->controller->cameraDeleteSupport());
        d->deleteSelectedAction->setEnabled(hasSelection & d->controller->cameraDeleteSupport());
        d->imageViewAction->setEnabled(hasSelection);
    }

    unsigned long fSize = 0;
    unsigned long dSize = 0;
    d->view->itemsSelectionSizeInfo(fSize, dSize);
    d->albumLibraryFreeSpace->setEstimatedDSizeKb(dSize);
}

void CameraUI::slotItemsSelected(CameraIconItem* item, bool selected)
{
    if (!d->controller) return;

    d->downloadSelectedAction->setEnabled(selected);
    d->downloadDelSelectedAction->setEnabled(selected & d->controller->cameraDeleteSupport());
    d->deleteSelectedAction->setEnabled(selected & d->controller->cameraDeleteSupport());
    d->imageViewAction->setEnabled(selected);

    if (selected)
    {
        // if selected item is in the list of item which will be deleted, set no current item
        if (!d->currentlyDeleting.contains(item->itemInfo()->folder + item->itemInfo()->name))
        {
            KUrl url(item->itemInfo()->folder + '/' + item->itemInfo()->name);
            d->rightSideBar->itemChanged(item->itemInfo(), url, QByteArray(), d->view, item);
            d->controller->getExif(item->itemInfo()->folder, item->itemInfo()->name);
        }
        else
        {
            d->rightSideBar->slotNoCurrentItem();
        }
    }
    else
        d->rightSideBar->slotNoCurrentItem();
}

bool CameraUI::createAutoAlbum(const KUrl& parentURL, const QString& sub,
                               const QDate& date, QString& errMsg)
{
    KUrl u(parentURL);
    u.addPath(sub);

    // first stat to see if the album exists
    QFileInfo info(u.path());
    if (info.exists())
    {
        // now check if its really a directory
        if (info.isDir())
            return true;
        else
        {
            errMsg = i18n("A file with the same name (%1) already exists in folder %2.",
                          sub, parentURL.path());
            return false;
        }
    }

    // looks like the directory does not exist, try to create it

    PAlbum* parent = AlbumManager::instance()->findPAlbum(parentURL);
    if (!parent)
    {
        errMsg = i18n("Failed to find Album for path '%1'.", parentURL.path());
        return false;
    }
    return AlbumManager::instance()->createPAlbum(parent, sub, QString(), date, QString(), errMsg);
}

void CameraUI::slotFirstItem()
{
    CameraIconItem *currItem = dynamic_cast<CameraIconItem*>(d->view->firstItem());
    d->view->clearSelection();
    d->view->updateContents();
    if (currItem)
       d->view->setCurrentItem(currItem);
}

void CameraUI::slotPrevItem()
{
    CameraIconItem *currItem = dynamic_cast<CameraIconItem*>(d->view->currentItem());
    d->view->clearSelection();
    d->view->updateContents();
    if (currItem)
       d->view->setCurrentItem(currItem->prevItem());
}

void CameraUI::slotNextItem()
{
    CameraIconItem *currItem = dynamic_cast<CameraIconItem*>(d->view->currentItem());
    d->view->clearSelection();
    d->view->updateContents();
    if (currItem)
       d->view->setCurrentItem(currItem->nextItem());
}

void CameraUI::slotLastItem()
{
    CameraIconItem *currItem = dynamic_cast<CameraIconItem*>(d->view->lastItem());
    d->view->clearSelection();
    d->view->updateContents();
    if (currItem)
       d->view->setCurrentItem(currItem);
}

void CameraUI::slotDonateMoney()
{
    KToolInvocation::invokeBrowser("http://www.digikam.org/?q=donation");
}

void CameraUI::slotContribute()
{
    KToolInvocation::invokeBrowser("http://www.digikam.org/?q=contrib");
}

void CameraUI::slotEditKeys()
{
    KShortcutsDialog dialog(KShortcutsEditor::AllActions,
                            KShortcutsEditor::LetterShortcutsAllowed, this);
    dialog.addCollection( actionCollection(), i18n( "General" ) );
    dialog.configure();
}

void CameraUI::slotConfToolbars()
{
    saveMainWindowSettings(KGlobal::config()->group("Camera Settings"));
    KEditToolBar dlg(factory(), this);

    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(slotNewToolbarConfig()));

    dlg.exec();
}

void CameraUI::slotNewToolbarConfig()
{
    applyMainWindowSettings(KGlobal::config()->group("Camera Settings"));
}

void CameraUI::slotSetup()
{
    Setup::exec(this);
}

void CameraUI::slotToggleFullScreen()
{
    if (d->fullScreen) // out of fullscreen
    {
        setWindowState( windowState() & ~Qt::WindowFullScreen ); // reset

        menuBar()->show();
        statusBar()->show();
        showToolBars();

        if (d->removeFullScreenButton)
        {
            QList<KToolBar *> toolbars = toolBars();
            foreach(KToolBar *toolbar, toolbars)
            {
                // name is set in ui.rc XML file
                if (toolbar->objectName() == "ToolBar")
                {
                    toolbar->removeAction(d->fullScreenAction);
                    break;
                }
            }
        }

        d->rightSideBar->restore();

        d->fullScreen = false;
    }
    else  // go to fullscreen
    {
        // hide the menubar and the statusbar
        menuBar()->hide();
        statusBar()->hide();

        if (d->fullScreenHideToolBar)
        {
            hideToolBars();
        }
        else
        {
            showToolBars();

            QList<KToolBar *> toolbars = toolBars();
            KToolBar *mainToolbar = 0;
            foreach(KToolBar *toolbar, toolbars)
            {
                if (toolbar->objectName() == "ToolBar")
                {
                    mainToolbar = toolbar;
                    break;
                }
            }

            // add fullscreen action if necessary
            if ( mainToolbar && !mainToolbar->actions().contains(d->fullScreenAction) )
            {
                mainToolbar->addAction(d->fullScreenAction);
                d->removeFullScreenButton=true;
            }
            else
            {
                // If FullScreen button is enabled in toolbar settings,
                // we shall not remove it when leaving of fullscreen mode.
                d->removeFullScreenButton=false;
            }
        }

        d->rightSideBar->backup();

        setWindowState( windowState() | Qt::WindowFullScreen ); // set
        d->fullScreen = true;
    }
}

void CameraUI::slotEscapePressed()
{
    if (d->fullScreen)
        d->fullScreenAction->activate(QAction::Trigger);
}

void CameraUI::showToolBars()
{
    QList<KToolBar *> toolbars = toolBars();
    foreach(KToolBar *toolbar, toolbars)
    {
        toolbar->show();
    }
}

void CameraUI::hideToolBars()
{
    QList<KToolBar *> toolbars = toolBars();
    foreach(KToolBar *toolbar, toolbars)
    {
        toolbar->hide();
    }
}

void CameraUI::slotCameraFreeSpaceInfo(unsigned long kBSize, unsigned long kBAvail)
{
    d->cameraFreeSpace->addInformation(kBSize, kBSize-kBAvail, kBAvail, QString());
}

bool CameraUI::cameraDeleteSupport()
{
    return d->controller->cameraDeleteSupport();
}

bool CameraUI::cameraUploadSupport()
{
    return d->controller->cameraUploadSupport();
}

bool CameraUI::cameraMkDirSupport()
{
    return d->controller->cameraMkDirSupport();
}

bool CameraUI::cameraDelDirSupport()
{
    return d->controller->cameraDelDirSupport();
}

void CameraUI::slotRawCameraList()
{
    RawCameraDlg dlg(this);
    dlg.exec();
}

void CameraUI::slotThemeChanged()
{
    QStringList themes(ThemeEngine::instance()->themeNames());
    int index = themes.indexOf(AlbumSettings::instance()->getCurrentTheme());
    if (index == -1)
        index = themes.indexOf(i18n("Default"));

    d->themeMenuAction->setCurrentItem(index);
}

void CameraUI::slotChangeTheme(const QString& theme)
{
    // Theme menu entry is returned with keyboard accelerator. We remove it.
    QString name = theme;
    name.remove(QChar('&'));
    AlbumSettings::instance()->setCurrentTheme(theme);
    ThemeEngine::instance()->slotChangeTheme(theme);
}

void CameraUI::slotComponentsInfo()
{
    showDigikamComponentsInfo();
}

void CameraUI::refreshCollectionFreeSpace()
{
    d->albumLibraryFreeSpace->setPaths(CollectionManager::instance()->allAvailableAlbumRootPaths());
}

void CameraUI::slotCollectionLocationStatusChanged(const CollectionLocation &, int)
{
    refreshCollectionFreeSpace();
}

void CameraUI::slotShowMenuBar()
{
    const bool visible = menuBar()->isVisible();
    menuBar()->setVisible(!visible);
}

void CameraUI::slotSidebarTabTitleStyleChanged()
{
    d->rightSideBar->setStyle(AlbumSettings::instance()->getSidebarTitleStyle());
}

}  // namespace Digikam
