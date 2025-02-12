/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-11-24
 * Description : Color management setup tab.
 *
 * SPDX-FileCopyrightText: 2005-2007 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupicc.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QApplication>
#include <QStyle>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDialogButtonBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "dlayoutbox.h"
#include "squeezedcombobox.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "applicationsettings.h"
#include "iccprofileinfodlg.h"
#include "iccprofilescombobox.h"
#include "iccsettings.h"
#include "iccsettingscontainer.h"
#include "dactivelabel.h"
#include "dfileselector.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupICC::Private
{
public:

    Private() = default;

    QLabel*                     iccFolderLabel          = nullptr;

    QCheckBox*                  enableColorManagement   = nullptr;
    QCheckBox*                  defaultSRGBConvert      = nullptr;
    QCheckBox*                  bpcAlgorithm            = nullptr;
    QCheckBox*                  managedView             = nullptr;
    QCheckBox*                  managedPreviews         = nullptr;

    QRadioButton*               defaultAskMismatch      = nullptr;
    QRadioButton*               defaultConvertMismatch  = nullptr;
    QRadioButton*               defaultAskMissing       = nullptr;
    QRadioButton*               defaultSRGBMissing      = nullptr;
    QRadioButton*               defaultWSMissing        = nullptr;
    QRadioButton*               defaultInputMissing     = nullptr;
    QRadioButton*               defaultAskRaw           = nullptr;
    QRadioButton*               defaultInputRaw         = nullptr;
    QRadioButton*               defaultGuessRaw         = nullptr;

    QPushButton*                infoWorkProfiles        = nullptr;
    QPushButton*                infoMonitorProfiles     = nullptr;
    QPushButton*                infoInProfiles          = nullptr;
    QPushButton*                infoProofProfiles       = nullptr;

    QGroupBox*                  workspaceGB             = nullptr;
    QGroupBox*                  mismatchGB              = nullptr;
    QGroupBox*                  missingGB               = nullptr;
    QGroupBox*                  rawGB                   = nullptr;
    QGroupBox*                  inputGB                 = nullptr;
    QGroupBox*                  viewGB                  = nullptr;
    QGroupBox*                  proofGB                 = nullptr;
    QGroupBox*                  iccFolderGB             = nullptr;
    QGroupBox*                  advancedSettingsGB      = nullptr;

    DFileSelector*              defaultPathKU           = nullptr;

    IccRenderingIntentComboBox* renderingIntentKC       = nullptr;

    QWidget*                    behaviorPanel           = nullptr;
    QWidget*                    profilesPanel           = nullptr;
    QWidget*                    advancedPanel           = nullptr;
    QTabWidget*                 tab                     = nullptr;
    QDialogButtonBox*           dlgBtnBox               = nullptr;

    IccProfilesComboBox*        inProfilesKC            = nullptr;
    IccProfilesComboBox*        workProfilesKC          = nullptr;
    IccProfilesComboBox*        proofProfilesKC         = nullptr;
    IccProfilesComboBox*        monitorProfilesKC       = nullptr;
};

SetupICC::SetupICC(QDialogButtonBox* const dlgBtnBox, QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    const int spacing              = layoutSpacing();

    d->dlgBtnBox                   = dlgBtnBox;
    d->tab                         = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    d->behaviorPanel               = new QWidget;
    QVBoxLayout* const mainLayout  = new QVBoxLayout(d->behaviorPanel);

    // --------------------------------------------------------

    QWidget* const colorPolicy     = new QWidget;
    QGridLayout* const gridHeader  = new QGridLayout(colorPolicy);

    d->enableColorManagement       = new QCheckBox(colorPolicy);
    d->enableColorManagement->setText(i18n("Enable Color Management"));
    d->enableColorManagement->setWhatsThis(i18n("<ul><li>Checked: Color Management is enabled</li>"
                                                "<li>Unchecked: Color Management is "
                                                "disabled</li></ul>"));

    DActiveLabel* const lcmsLogoLabel = new DActiveLabel(QUrl(QLatin1String("https://www.littlecms.com")),
                                                         QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/logo-lcms.png")),
                                                         colorPolicy);
    lcmsLogoLabel->setToolTip(i18n("Visit Little CMS project website"));

    gridHeader->addWidget(d->enableColorManagement, 0, 0, 1, 1);
    gridHeader->addWidget(lcmsLogoLabel,            0, 2, 1, 1);
    gridHeader->setColumnStretch(1, 10);
    gridHeader->setContentsMargins(spacing, spacing, spacing, spacing);
    gridHeader->setSpacing(0);

    // --------------------------------------------------------

    d->workspaceGB             = new QGroupBox(i18n("Working Color Space"));
    QHBoxLayout* const hboxWS  = new QHBoxLayout(d->workspaceGB);

    QLabel* const workIcon     = new QLabel;
    workIcon->setPixmap(QIcon::fromTheme(QLatin1String("input-tablet")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));
    d->workProfilesKC          = new IccProfilesComboBox;
    d->workProfilesKC->setWhatsThis(i18n("<p>This is the color space all the images will be converted to when opened "
                                         "(if you choose to convert) and the profile that will be embedded when saving. "
                                         "Good and safe choices are <b>Adobe RGB (1998)</b> and <b>sRGB IEC61966-2.1</b></p>"));

    d->infoWorkProfiles        = new QPushButton;
    d->infoWorkProfiles->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->infoWorkProfiles->setWhatsThis(i18n("<p>You can use this button to get more detailed "
                                           "information about the selected workspace profile.</p>"));

    hboxWS->addWidget(workIcon);
    hboxWS->addWidget(d->workProfilesKC, 10);
    hboxWS->addWidget(d->infoWorkProfiles);

    // --------------------------------------------------------

    d->mismatchGB                   = new QGroupBox;  // NOTE: Behavior on Profile Mismatch
    QVBoxLayout* const vlayMismatch = new QVBoxLayout(d->mismatchGB);

    QLabel* const behaviorIcon      = new QLabel;
    behaviorIcon->setPixmap(QIcon::fromTheme(QLatin1String("view-preview")).pixmap(32));
    QLabel* const behaviorLabel     = new QLabel(i18n("When the profile of an image does not match the working color space"));
    behaviorLabel->setWordWrap(true);

    QHBoxLayout* const hboxBL       = new QHBoxLayout;
    hboxBL->addWidget(behaviorIcon);
    hboxBL->addWidget(behaviorLabel, 10);

    d->defaultAskMismatch           = new QRadioButton(d->mismatchGB);
    d->defaultAskMismatch->setText(i18n("Ask when opening the image"));
    d->defaultAskMismatch->setWhatsThis(i18n("<p>If an image has an embedded color profile not matching the working "
                                             "space profile, digiKam will ask if you want to convert to the working space, "
                                             "keep the embedded profile or discard the embedded profile and assign "
                                             "a different one.</p>"));

    d->defaultConvertMismatch       = new QRadioButton(d->mismatchGB);
    d->defaultConvertMismatch->setText(i18n("Convert the image to the working color space"));
    d->defaultConvertMismatch->setWhatsThis(i18n("<p>If an image has an embedded color profile not matching the working "
                                                 "space profile, digiKam will convert the image's color information to "
                                                 "the working color space. This changes the pixel data, but not the "
                                                 "appearance of the image.</p>"));

    vlayMismatch->addLayout(hboxBL);
    vlayMismatch->addWidget(d->defaultAskMismatch);
    vlayMismatch->addWidget(d->defaultConvertMismatch);

    // --------------------------------------------------------

    d->missingGB                    = new QGroupBox;//(i18n("Missing Profile Behavior"));
    QVBoxLayout* const vlayMissing  = new QVBoxLayout(d->missingGB);

    QLabel* const  missingIcon      = new QLabel;
    missingIcon->setPixmap(QIcon::fromTheme(QLatin1String("paint-unknown")).pixmap(32));
    QLabel* const missingLabel      = new QLabel(i18n("When an image has no color profile information"));
    missingLabel->setWordWrap(true);

    QHBoxLayout* const hboxMP       = new QHBoxLayout;
    hboxMP->addWidget(missingIcon);
    hboxMP->addWidget(missingLabel, 10);

    d->defaultAskMissing            = new QRadioButton(i18n("Ask when opening the image"));
    d->defaultAskMissing->setWhatsThis(i18n("<p>If an image has no embedded color profile, "
                                            "digiKam will ask which color space shall be used to interpret the image "
                                            "and to which color space it shall be transformed for editing.</p>"));

    d->defaultSRGBMissing = new QRadioButton(i18n("Assume it is using the sRGB color space (Internet standard)"));

    /**
     * @todo d->defaultSRGBMissing->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultSRGBConvert = new QCheckBox(i18n("and convert it to the working color space"));

    /**
     * @todo d->defaultSRGBConvert->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultSRGBConvert->setChecked(true);

    QGridLayout* const gridRgb  = new QGridLayout;
    gridRgb->addWidget(d->defaultSRGBMissing, 0, 0, 1, 2);
    gridRgb->addWidget(d->defaultSRGBConvert, 1, 1);
    gridRgb->setColumnMinimumWidth(0, 10);

    d->defaultWSMissing         = new QRadioButton(i18n("Assume it is using the working color space"));

    /**
     * @todo d->defaultWSMissing->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultInputMissing      = new QRadioButton(i18n("Convert it from default input color space to working space"));

    /**
     * @todo d->defaultWSMissing->setWhatsThis( i18n("<p></p>"));
     */

    vlayMissing->addLayout(hboxMP);
    vlayMissing->addWidget(d->defaultAskMissing);
    vlayMissing->addLayout(gridRgb);
    vlayMissing->addWidget(d->defaultWSMissing);
    vlayMissing->addWidget(d->defaultInputMissing);

    // --------------------------------------------------------

    d->rawGB                       = new QGroupBox;//(i18n("Raw File Behavior"));
    QVBoxLayout* const vlayRaw     = new QVBoxLayout(d->rawGB);

    QLabel* const rawBehaviorIcon  = new QLabel;
    rawBehaviorIcon->setPixmap(QIcon::fromTheme(QLatin1String("image-x-adobe-dng")).pixmap(32));
    QLabel* const rawBehaviorLabel = new QLabel(i18n("When loading a RAW file with uncalibrated colors"));
    rawBehaviorLabel->setWordWrap(true);

    QHBoxLayout* const hboxRF      = new QHBoxLayout;
    hboxRF->addWidget(rawBehaviorIcon);
    hboxRF->addWidget(rawBehaviorLabel, 10);

    d->defaultAskRaw   = new QRadioButton(i18n("Ask for the input profile"));

    /**
     * @todo d->defaultAskRaw->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultGuessRaw = new QRadioButton(i18n("Automatic color correction"));

    /**
     * @todo d->defaultGuessRaw->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultInputRaw = new QRadioButton(i18n("Convert it from the default input profile"));

    /**
     * @todo d->defaultSRGBMissing->setWhatsThis( i18n("<p></p>"));
     */

    d->defaultGuessRaw->setChecked(true);

    vlayRaw->addLayout(hboxRF);
    vlayRaw->addWidget(d->defaultAskRaw);
    vlayRaw->addWidget(d->defaultGuessRaw);
    vlayRaw->addWidget(d->defaultInputRaw);


    mainLayout->addWidget(colorPolicy);
    mainLayout->addWidget(d->workspaceGB);
    mainLayout->addWidget(d->mismatchGB);
    mainLayout->addWidget(d->missingGB);
    mainLayout->addWidget(d->rawGB);
    mainLayout->addStretch();

    // --------------------------------------------------------

    d->profilesPanel               = new QWidget;
    QVBoxLayout* const vboxDisplay = new QVBoxLayout(d->profilesPanel);
    d->viewGB                      = new QGroupBox(i18n("Color Managed View"));
    QGridLayout* const gridView    = new QGridLayout(d->viewGB);

    QLabel* const monitorIcon      = new QLabel;
    monitorIcon->setPixmap(QIcon::fromTheme(QLatin1String("video-display")).pixmap(32));
    QLabel* const monitorProfiles  = new QLabel(i18n("Monitor profile:"));

    d->monitorProfilesKC     = new IccProfilesComboBox;
    monitorProfiles->setBuddy(d->monitorProfilesKC);
    d->monitorProfilesKC->setWhatsThis(i18n("<p>Select the color profile for your monitor here.</p>"));

    d->infoMonitorProfiles   = new QPushButton;
    d->infoMonitorProfiles->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->infoMonitorProfiles->setWhatsThis(i18n("<p>You can use this button to get more detailed "
                                              "information about the selected monitor profile.</p>"));

    d->managedView           = new QCheckBox;
    d->managedView->setText(i18n("Use color managed view in editor"));
    d->managedView->setWhatsThis(i18n("<p>Turn on this option if "
                                      "you want to use your <b>Monitor Color Profile</b> to show your pictures in "
                                      "the Image Editor window with a color correction adapted to your monitor. "
                                      "You can at any time toggle this option from the Editor window. "
                                      "<i>Warning</i>: This can slow down rendering of the image, depending on the speed of your computer.</p>"));

    d->managedPreviews       = new QCheckBox;
    d->managedPreviews->setText(i18n("Use color managed view for previews and thumbnails"));

    /**
     * @todo d->managedPreview->setWhatsThis( i18n("") );
     */

    gridView->addWidget(monitorIcon,            0, 0);
    gridView->addWidget(monitorProfiles,        0, 1, 1, 2);
    gridView->addWidget(d->monitorProfilesKC,   1, 0, 1, 2);
    gridView->addWidget(d->infoMonitorProfiles, 1, 2);
    gridView->addWidget(d->managedView,         2, 0, 1, 3);
    gridView->addWidget(d->managedPreviews,     3, 0, 1, 3);
    gridView->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->inputGB                 = new QGroupBox(i18n("Camera and Scanner"));
    QGridLayout* const gridIP  = new QGridLayout(d->inputGB);

    QLabel* const inputIcon    = new QLabel;
    inputIcon->setPixmap(QIcon::fromTheme(QLatin1String("input-tablet")).pixmap(32));
    QLabel* const inputLabel   = new QLabel(i18n("Default input color profile:"));
    d->inProfilesKC            = new IccProfilesComboBox;
    d->inProfilesKC->setWhatsThis(i18n("<p>This is the default color profile for your input device "
                                       "(that is your camera - or your scanner). A camera input profile "
                                       "is recommended for correct conversion of RAW images in 16bit. "
                                       "Some of the options about loading behavior above refer to this profile.</p>"));

    d->infoInProfiles          = new QPushButton;
    d->infoInProfiles->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->infoInProfiles->setWhatsThis(i18n("<p>You can use this button to get more detailed "
                                         "information about the selected input profile.</p>"));

    gridIP->addWidget(inputIcon,         0, 0);
    gridIP->addWidget(inputLabel,        0, 1, 1, 2);
    gridIP->addWidget(d->inProfilesKC,   1, 0, 1, 2);
    gridIP->addWidget(d->infoInProfiles, 1, 2);
    gridIP->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->proofGB                   = new QGroupBox(i18n("Printing and Proofing"));
    QGridLayout* const gridProof = new QGridLayout(d->proofGB);

    QLabel* const proofIcon      = new QLabel;
    proofIcon->setPixmap(QIcon::fromTheme(QLatin1String("document-print")).pixmap(32));
    QLabel* const proofLabel     = new QLabel(i18n("Output device profile:"));
    d->proofProfilesKC           = new IccProfilesComboBox;
    proofLabel->setBuddy(d->proofProfilesKC);
    d->proofProfilesKC->setWhatsThis(i18n("<p>Select the profile for your output device "
                                          "(usually, your printer). This profile will be used to do a soft proof, so you will "
                                          "be able to preview how an image will be rendered via an output device.</p>"));

    d->infoProofProfiles         = new QPushButton;
    d->infoProofProfiles->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->infoProofProfiles->setWhatsThis(i18n("<p>You can use this button to get more detailed "
                                            "information about the selected proofing profile.</p>"));

    gridProof->addWidget(proofIcon,            0, 0);
    gridProof->addWidget(proofLabel,           0, 1, 1, 2);
    gridProof->addWidget(d->proofProfilesKC,   1, 0, 1, 2);
    gridProof->addWidget(d->infoProofProfiles, 1, 2);
    gridProof->setColumnStretch(1, 10);

    // --------------------------------------------------------

    d->iccFolderGB                   = new QGroupBox(i18n("Color Profiles Repository"));
    QGridLayout* const gridIccFolder = new QGridLayout(d->iccFolderGB);

    QLabel* const iccFolderIcon = new QLabel;
    iccFolderIcon->setPixmap(QIcon::fromTheme(QLatin1String("folder-downloads")).pixmap(32));
    d->iccFolderLabel           = new QLabel(i18n("digiKam looks for ICC profiles in a number of <a href='default'>default locations</a>. "
                                                 "You can specify an additional folder:"));
    d->iccFolderLabel->setWordWrap(true);

    d->defaultPathKU            = new DFileSelector;
    d->iccFolderLabel->setBuddy(d->defaultPathKU);
    d->defaultPathKU->setFileDlgMode(QFileDialog::Directory);
    d->defaultPathKU->setFileDlgOptions(QFileDialog::ShowDirsOnly);
    d->defaultPathKU->setWhatsThis(i18n("<p>digiKam searches ICC profiles in default system folders "
                                        "and ships itself a few selected profiles. "
                                        "Store all your additional color profiles in the directory set here.</p>"));

    gridIccFolder->addWidget(iccFolderIcon,     0, 0);
    gridIccFolder->addWidget(d->iccFolderLabel, 0, 1);
    gridIccFolder->addWidget(d->defaultPathKU,  1, 0, 1, 2);
    gridIccFolder->setColumnStretch(1, 10);

    vboxDisplay->addWidget(d->viewGB);
    vboxDisplay->addWidget(d->inputGB);
    vboxDisplay->addWidget(d->proofGB);
    vboxDisplay->addWidget(d->iccFolderGB);
    vboxDisplay->addStretch(1);

    // --------------------------------------------------------

    d->advancedPanel                = new QWidget;
    QVBoxLayout* const vboxAdvanced = new QVBoxLayout(d->advancedPanel);
    d->advancedSettingsGB           = new QGroupBox(i18n("Advanced Settings"));
    QGridLayout* const gridAdvanced = new QGridLayout(d->advancedSettingsGB);

    d->bpcAlgorithm                 = new QCheckBox(d->advancedSettingsGB);
    d->bpcAlgorithm->setText(i18n("Use black point compensation"));
    d->bpcAlgorithm->setWhatsThis(i18n("<p><b>Black Point Compensation</b> is a way to make "
                                       "adjustments between the maximum "
                                       "black levels of digital files and the black capabilities of various "
                                       "digital devices.</p>"));

    QLabel* const lablel            = new QLabel(d->advancedSettingsGB);
    lablel->setText(i18n("Rendering Intents:"));

    d->renderingIntentKC            = new IccRenderingIntentComboBox(d->advancedSettingsGB);

    gridAdvanced->addWidget(d->bpcAlgorithm,      0, 0, 1, 2);
    gridAdvanced->addWidget(lablel,               1, 0, 1, 1);
    gridAdvanced->addWidget(d->renderingIntentKC, 1, 1, 1, 1);
    gridAdvanced->setContentsMargins(spacing, spacing, spacing, spacing);
    gridAdvanced->setSpacing(0);

    vboxAdvanced->addWidget(d->advancedSettingsGB);
    vboxAdvanced->addStretch(1);

    // --------------------------------------------------------

    d->tab->insertTab(Behavior, d->behaviorPanel, i18nc("@title: icc panel tab", "Behavior"));
    d->tab->insertTab(Profiles, d->profilesPanel, i18nc("@title: icc panel tab", "Profiles"));
    d->tab->insertTab(Advanced, d->advancedPanel, i18nc("@title: icc panel tab", "Advanced"));

    // --------------------------------------------------------

    connect(d->enableColorManagement, SIGNAL(toggled(bool)),
            this, SLOT(slotToggledEnabled()));

    connect(d->infoProofProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedProof()));

    connect(d->infoInProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedIn()));

    connect(d->infoMonitorProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedMonitor()));

    connect(d->infoWorkProfiles, SIGNAL(clicked()),
            this, SLOT(slotClickedWork()));

    connect(d->defaultPathKU, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotUrlChanged()));

    connect(d->defaultPathKU->lineEdit(), SIGNAL(textChanged(QString)),
            this, SLOT(slotUrlTextChanged()));

    connect(d->iccFolderLabel, SIGNAL(linkActivated(QString)),
            this, SLOT(slotShowDefaultSearchPaths()));

    connect(d->defaultAskMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultSRGBMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultWSMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    connect(d->defaultInputMissing, SIGNAL(toggled(bool)),
            this, SLOT(slotMissingToggled(bool)));

    // --------------------------------------------------------

    adjustSize();

    readSettings();
    slotToggledEnabled();
}

SetupICC::~SetupICC()
{
    delete d;
}

void SetupICC::setActiveTab(ICCTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupICC::ICCTab SetupICC::activeTab() const
{
    return (ICCTab)d->tab->currentIndex();
}

void SetupICC::applySettings()
{
    ICCSettingsContainer settings;
    settings.enableCM = d->enableColorManagement->isChecked();

    if      (d->defaultAskMismatch->isChecked())
    {
        settings.defaultMismatchBehavior = ICCSettingsContainer::AskUser;
    }
    else if (d->defaultConvertMismatch->isChecked())
    {
        settings.defaultMismatchBehavior = ICCSettingsContainer::EmbeddedToWorkspace;
    }

    if      (d->defaultAskMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::AskUser;
    }
    else if (d->defaultSRGBMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::UseSRGB;

        if (d->defaultSRGBConvert->isChecked())
        {
            settings.defaultMissingProfileBehavior |= ICCSettingsContainer::ConvertToWorkspace;
        }
        else
        {
            settings.defaultMissingProfileBehavior |= ICCSettingsContainer::KeepProfile;
        }
    }
    else if (d->defaultWSMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::UseWorkspace | ICCSettingsContainer::KeepProfile;
    }
    else if (d->defaultInputMissing->isChecked())
    {
        settings.defaultMissingProfileBehavior = ICCSettingsContainer::InputToWorkspace;
    }

    if      (d->defaultAskRaw->isChecked())
    {
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::AskUser;
    }
    else if (d->defaultInputRaw->isChecked())
    {
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::InputToWorkspace;
    }
    else if (d->defaultGuessRaw->isChecked())
    {
        settings.defaultUncalibratedBehavior = ICCSettingsContainer::AutomaticColors | ICCSettingsContainer::ConvertToWorkspace;
    }

    settings.iccFolder           = d->defaultPathKU->fileDlgPath();
    settings.useBPC              = d->bpcAlgorithm->isChecked();
    settings.renderingIntent     = d->renderingIntentKC->intent();
    settings.useManagedView      = d->managedView->isChecked();
    settings.useManagedPreviews  = d->managedPreviews->isChecked();

    settings.defaultInputProfile = d->inProfilesKC->currentProfile().filePath();
    settings.workspaceProfile    = d->workProfilesKC->currentProfile().filePath();
    settings.defaultProofProfile = d->proofProfilesKC->currentProfile().filePath();

    if (!IccSettings::instance()->monitorProfileFromSystem())
    {
        settings.monitorProfile = d->monitorProfilesKC->currentProfile().filePath();
    }

    IccSettings::instance()->setSettings(settings);
}

void SetupICC::readSettings(bool restore)
{
    ICCSettingsContainer settings = IccSettings::instance()->settings();

    if (!restore)
    {
        d->enableColorManagement->setChecked(settings.enableCM);
    }

    d->bpcAlgorithm->setChecked(settings.useBPC);
    d->renderingIntentKC->setIntent(settings.renderingIntent);
    d->managedView->setChecked(settings.useManagedView);
    d->managedPreviews->setChecked(settings.useManagedPreviews);

    if      (settings.defaultMismatchBehavior & ICCSettingsContainer::AskUser)
    {
        d->defaultAskMismatch->setChecked(true);
    }
    else if (settings.defaultMismatchBehavior & ICCSettingsContainer::ConvertToWorkspace)
    {
        d->defaultConvertMismatch->setChecked(true);
    }

    if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::AskUser)
    {
        d->defaultAskMissing->setChecked(true);
    }
    else
    {
        if      (settings.defaultMissingProfileBehavior & ICCSettingsContainer::UseSRGB)
        {
            d->defaultSRGBMissing->setChecked(true);
            d->defaultSRGBConvert->setChecked(settings.defaultMissingProfileBehavior & ICCSettingsContainer::ConvertToWorkspace);
        }
        else if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::UseWorkspace)
        {
            d->defaultWSMissing->setChecked(true);
        }
        else if (settings.defaultMissingProfileBehavior & ICCSettingsContainer::UseDefaultInputProfile)
        {
            d->defaultInputMissing->setChecked(true);
        }
    }

    if      (settings.defaultUncalibratedBehavior & ICCSettingsContainer::AskUser)
    {
        d->defaultAskRaw->setChecked(true);
    }
    else if (settings.defaultUncalibratedBehavior & ICCSettingsContainer::UseDefaultInputProfile)
    {
        d->defaultInputRaw->setChecked(true);
    }
    else if (settings.defaultUncalibratedBehavior & ICCSettingsContainer::AutomaticColors)
    {
        d->defaultGuessRaw->setChecked(true);
    }

    d->defaultPathKU->setFileDlgPath(settings.iccFolder);
    fillCombos(false);

    d->workProfilesKC->setCurrentProfile(IccProfile(settings.workspaceProfile));
    d->inProfilesKC->setCurrentProfile(IccProfile(settings.defaultInputProfile));
    d->proofProfilesKC->setCurrentProfile(IccProfile(settings.defaultProofProfile));

    if (IccSettings::instance()->monitorProfileFromSystem())
    {
        d->monitorProfilesKC->clear();
        d->monitorProfilesKC->setNoProfileIfEmpty(i18n("Monitor Profile From System Settings"));
    }
    else
    {
        d->monitorProfilesKC->setCurrentProfile(IccProfile(settings.monitorProfile));
    }
}

void SetupICC::slotUrlChanged()
{
    IccSettings::instance()->setIccPath(d->defaultPathKU->fileDlgPath());
    fillCombos(true);
}

void SetupICC::slotUrlTextChanged()
{
    d->defaultPathKU->setFileDlgPath(d->defaultPathKU->fileDlgPath());
}

void SetupICC::fillCombos(bool report)
{
    if (!d->enableColorManagement->isChecked())
    {
        return;
    }

    QList<IccProfile> profiles = IccSettings::instance()->allProfiles();

    if (profiles.isEmpty())
    {
        if (report)
        {
            QString message = i18n("No ICC profiles files found.");
            QMessageBox::information(this, qApp->applicationName(), message);
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "No ICC profile files found!!!";
        d->dlgBtnBox->button(QDialogButtonBox::Ok)->setEnabled(false);

        return;
    }

    d->workProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->workspaceProfiles());
    d->monitorProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->displayProfiles());
    d->inProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->inputProfiles());
    d->proofProfilesKC->replaceProfilesSqueezed(IccSettings::instance()->outputProfiles());

    d->workProfilesKC->setNoProfileIfEmpty(i18n("No Profile Available"));
    d->monitorProfilesKC->setNoProfileIfEmpty(i18n("No Display Profile Available"));
    d->inProfilesKC->setNoProfileIfEmpty(i18n("No Input Profile Available"));
    d->proofProfilesKC->setNoProfileIfEmpty(i18n("No Output Profile Available"));

    if (d->monitorProfilesKC->count() == 0)
    {
        d->managedView->setEnabled(false);
        d->managedPreviews->setEnabled(false);
    }
    else
    {
        d->dlgBtnBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        d->managedPreviews->setEnabled(true);
    }

    if (d->workProfilesKC->count() == 0)
    {
        // If there is no workspace ICC profiles available,
        // the CM is broken and cannot be used.

        d->dlgBtnBox->button(QDialogButtonBox::Ok)->setEnabled(false);

        return;
    }

    d->dlgBtnBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void SetupICC::setWidgetsEnabled(bool enabled)
{
    d->workspaceGB->setEnabled(enabled);
    d->mismatchGB->setEnabled(enabled);
    d->missingGB->setEnabled(enabled);
    d->rawGB->setEnabled(enabled);
    d->tab->setTabEnabled(1, enabled);
    d->tab->setTabEnabled(2, enabled);
/*
    d->profilesPanel->setEnabled(enabled);
    d->advancedPanel->setEnabled(enabled);
*/
}

void SetupICC::slotToggledEnabled()
{
    bool enabled = d->enableColorManagement->isChecked();

    setWidgetsEnabled(enabled);

    if (enabled)
    {
        readSettings(true);
    }
    else
    {
        d->dlgBtnBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void SetupICC::slotClickedWork()
{
    IccProfile profile = d->workProfilesKC->currentProfile();

    if (!profile.isNull())
    {
        profileInfo(profile);
    }
}

void SetupICC::slotClickedIn()
{
    IccProfile profile = d->inProfilesKC->currentProfile();

    if (!profile.isNull())
    {
        profileInfo(profile);
    }
}

void SetupICC::slotClickedMonitor()
{
    IccProfile profile;

    if (IccSettings::instance()->monitorProfileFromSystem())
    {
        profile = IccSettings::instance()->monitorProfile();
    }
    else
    {
        profile = d->monitorProfilesKC->currentProfile();
    }

    if (!profile.isNull())
    {
        profileInfo(profile);
    }
}

void SetupICC::slotClickedProof()
{
    IccProfile profile = d->proofProfilesKC->currentProfile();

    if (!profile.isNull())
    {
        profileInfo(profile);
    }
}

void SetupICC::profileInfo(const IccProfile& profile)
{
    if (profile.isNull())
    {
        QMessageBox::critical(this, i18nc("@title:window", "Profile Error"), i18n("No profile is selected."));
        return;
    }

    ICCProfileInfoDlg infoDlg(this, profile.filePath(), profile);
    infoDlg.exec();
}

void SetupICC::slotMissingToggled(bool on)
{
    if (!on)
    {
        return;
    }

    d->defaultSRGBConvert->setEnabled(d->defaultSRGBMissing->isChecked());
}

void SetupICC::slotShowDefaultSearchPaths()
{
    QStringList defaultSearchPaths = IccProfile::defaultSearchPaths();
    QString existingPaths;

    if (defaultSearchPaths.isEmpty())
    {
        existingPaths = i18nc("none of the paths", "none");
    }
    else
    {
        existingPaths = defaultSearchPaths.join(QLatin1String("</li><li>"));
    }

#ifdef Q_OS_WIN

    QString text = i18n("On Windows, the default search paths include "
                        "<ul>"
                        "<li>%1/Windows/System32/spool/drivers/color/</li>"   // For Win10 and Win7
                        "<li>%1/Windows/Spool/Drivers/Color/</li>"            // For Win2K and WinXP
                        "<li>%2/Windows/Color/</li>"                          // For Win98 and WinMe
                        "</ul>"
                        "On your system, currently these paths exist and are scanned:"
                        "<ul>"
                        "<li>%3</li>"
                        "</ul>",
                        QDir::rootPath(),
                        QDir::rootPath(),
                        existingPaths);

#elif defined (Q_OS_MACOS)

    QString text = i18n("On macOS, the default search paths include "
                        "<ul>"
                        "<li>/System/Library/ColorSync/Profiles</li>"
                        "<li>/Library/ColorSync/Profiles</li>"
                        "<li>~/Library/ColorSync/Profiles</li>"
                        "<li>/opt/local/share/color/icc</li>"
                        "<li>%1/Contents/Resources/digikam/profiles/</li>"
                        "<li>~/.local/share/color/icc/</li>"
                        "<li>~/.local/share/icc/</li>"
                        "<li>~/.color/icc/</li>"
                        "</ul>"
                        "On your system, currently these paths exist and are scanned:"
                        "<ul>"
                        "<li>%2</li>"
                        "</ul>",
                        macOSBundlePrefix(),
                        existingPaths);
#else // Linux

    QString text = i18n("On Linux, the default search paths include "
                        "<ul>"
                        "<li>/usr/share/color/icc</li>"
                        "<li>/usr/local/share/color/icc</li>"
                        "<li>~/.local/share/color/icc/</li>"
                        "<li>~/.local/share/icc/</li>"
                        "<li>~/.color/icc/</li>"
                        "</ul>"
                        "On your system, currently these paths exist and are scanned:"
                        "<ul>"
                        "<li>%1</li>"
                        "</ul>",
                        existingPaths);

#endif

    QWhatsThis::showText(d->iccFolderLabel->mapToGlobal(QPoint(0, 0)), text, d->iccFolderLabel);
}

bool SetupICC::iccRepositoryIsValid()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Color Management"));

    // If color management is disable, no need to check anymore.

    if (!group.readEntry(QLatin1String("EnableCM"), false))
    {
        return true;
    }

    // Can at least RawEngine profiles be opened?

    if (IccProfile::sRGB().open())
    {
        return true;
    }

    // To be valid, the ICC profiles repository must exist and be readable.

    QString extraPath = group.readEntry(QLatin1String("DefaultPath"), QString());
    QFileInfo info(extraPath);

    if (info.isDir() && info.exists() && info.isReadable())
    {
        return true;
    }

    QStringList paths = IccProfile::defaultSearchPaths();

    return (!paths.isEmpty());
}

} // namespace Digikam

#include "moc_setupicc.cpp"
