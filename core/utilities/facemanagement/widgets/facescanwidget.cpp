/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * Copyright (C) 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// NOTE: Uncomment this line to enable detect and recognize option
// Currently this option is hiden, since it's not handled properly and provides confusing functionality => Fix it later
//#define ENABLE_DETECT_AND_RECOGNIZE

#include "facescanwidget_p.h"

namespace Digikam
{

FaceScanWidget::FaceScanWidget(QWidget* const parent)
    : QWidget(parent),
      StateSavingObject(this),
      d(new Private)
{
    setObjectName(d->configName);
    setupUi();
    setupConnections();
}

FaceScanWidget::~FaceScanWidget()
{
    delete d;
}

void FaceScanWidget::doLoadState()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << getConfigGroup().name();
    KConfigGroup group = getConfigGroup();
    QString mainTask   = group.readEntry(entryName(d->configMainTask),
                                         d->configValueDetect);

    if (mainTask == d->configValueRecognizedMarkedFaces)
    {
        d->reRecognizeButton->setChecked(true);
    }
    else if (mainTask == d->configValueDetectAndRecognize)
    {
#ifdef ENABLE_DETECT_AND_RECOGNIZE
        d->detectAndRecognizeButton->setChecked(true);
#endif
        d->detectButton->setChecked(true);
    }
    else
    {
        d->detectButton->setChecked(true);
    }

    FaceScanSettings::AlreadyScannedHandling handling;
    QString skipHandling = group.readEntry(entryName(d->configAlreadyScannedHandling),
                                           QString::fromLatin1("Skip"));

    if (skipHandling == QLatin1String("Rescan"))
    {
        handling = FaceScanSettings::Rescan;
    }
    else if (skipHandling == QLatin1String("Merge"))
    {
        handling = FaceScanSettings::Merge;
    }
    else // Skip
    {
        handling = FaceScanSettings::Skip;
    }

    d->alreadyScannedBox->setCurrentIndex(d->alreadyScannedBox->findData(handling));

    d->accuracyInput->setValue(ApplicationSettings::instance()->getFaceDetectionAccuracy() * 100);

    d->albumSelectors->loadState();

    d->useFullCpuButton->setChecked(group.readEntry(entryName(d->configUseFullCpu), false));

    // do not load retrainAllButton state from config, dangerous
}

void FaceScanWidget::doSaveState()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << getConfigGroup().name();

    KConfigGroup group = getConfigGroup();

    QString mainTask;

    if (d->detectButton->isChecked())
    {
        mainTask = d->configValueDetect;
    }
#ifdef ENABLE_DETECT_AND_RECOGNIZE
    else if (d->detectAndRecognizeButton->isChecked())
    {
        mainTask = d->configValueDetectAndRecognize;
    }
#endif
    else // d->reRecognizeButton
    {
        mainTask = d->configValueRecognizedMarkedFaces;
    }

    group.writeEntry(entryName(d->configMainTask), mainTask);

    QString handling;

    switch ((FaceScanSettings::AlreadyScannedHandling)(d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt()))
    {
        case FaceScanSettings::Skip:
            handling = QLatin1String("Skip");
            break;

        case FaceScanSettings::Rescan:
            handling = QLatin1String("Rescan");
            break;

        case FaceScanSettings::Merge:
            handling = QLatin1String("Merge");
            break;
    }

    group.writeEntry(entryName(d->configAlreadyScannedHandling), handling);

    ApplicationSettings::instance()->setFaceDetectionAccuracy(double(d->accuracyInput->value()) / 100);
    d->albumSelectors->saveState();

    group.writeEntry(entryName(d->configUseFullCpu), d->useFullCpuButton->isChecked());
}

void FaceScanWidget::setupUi()
{
    // ---- Main option box --------

    d->optionGroupBox                 = new QGroupBox(i18n("Workflow"));
    QVBoxLayout* const optionLayout   = new QVBoxLayout;

    d->alreadyScannedBox              = new SqueezedComboBox;
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Skip images already scanned"),          FaceScanSettings::Skip);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan again and merge results"),         FaceScanSettings::Merge);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"), FaceScanSettings::Rescan);
    d->alreadyScannedBox->setCurrentIndex(FaceScanSettings::Skip);

    d->detectButton                   = new QRadioButton(i18nc("@option:radio", "Detect faces"));
    d->detectButton->setToolTip(i18nc("@info", "Find all faces in your photos"));

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    d->detectAndRecognizeButton       = new QRadioButton(i18nc("@option:radio", "Detect and recognize faces"));
    d->detectAndRecognizeButton->setToolTip(i18nc("@info", "Find all faces in your photos and try to recognize which person is depicted"));
#endif

    d->reRecognizeButton              = new QRadioButton(i18nc("@option:radio", "Recognize faces"));
    d->reRecognizeButton->setToolTip(i18nc("@info", "Try again to recognize the people depicted on marked but yet unconfirmed faces."));

    optionLayout->addWidget(d->alreadyScannedBox);
    optionLayout->addWidget(d->detectButton);

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    optionLayout->addWidget(d->detectAndRecognizeButton);
#endif

    optionLayout->addWidget(d->reRecognizeButton);

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    QStyleOptionButton buttonOption;
    buttonOption.initFrom(d->detectAndRecognizeButton);
    int indent = style()->subElementRect(QStyle::SE_RadioButtonIndicator, &buttonOption, d->detectAndRecognizeButton).width();
    optionLayout->setColumnMinimumWidth(0, indent);
#endif

    d->optionGroupBox->setLayout(optionLayout);

    // ---- Album tab ---------

    d->tabWidget                      = new QTabWidget;
    QWidget* const selWidget          = new QWidget(d->tabWidget);
    QVBoxLayout* const selLayout      = new QVBoxLayout;
    d->albumSelectors                 = new AlbumSelectors(i18nc("@label", "Search in:"), d->configName, selWidget);
    selLayout->addWidget(d->albumSelectors);
    selWidget->setLayout(selLayout);

    d->tabWidget->addTab(selWidget, i18nc("@title:tab", "Albums"));

    // ---- Settings tab ------

    QWidget* const settingsTab        = new QWidget(d->tabWidget);
    QGridLayout* const settingsLayout = new QGridLayout(settingsTab);

    QLabel* const detectionLabel      = new QLabel(i18nc("@label", "Face Accuracy:"), settingsTab);

    QLabel* const accuracyLabel       = new QLabel(i18nc("@label Two extremities of a scale", "Sensitivity     -     Specificity"), settingsTab);
    accuracyLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    d->accuracyInput                  = new DIntNumInput(settingsTab);
    d->accuracyInput->setDefaultValue(70);
    d->accuracyInput->setRange(0, 100, 10);
    d->accuracyInput->setToolTip(i18nc("@info:tooltip",
                                       "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will "
                                       "be recognized, but less faces will be recognized "
                                       "(only faces that are very similar to pre-tagged faces are recognized)."));

    d->useFullCpuButton = new QCheckBox(settingsTab);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Face detection and recognition are time-consuming tasks. "
                                          "You can choose if you wish to employ all processor cores "
                                          "on your system, or work in the background only on one core."));

    d->retrainAllButton = new QCheckBox(settingsTab);
    d->retrainAllButton->setText(i18nc("@option:check", "Clear and rebuild all training data"));
    d->retrainAllButton->setToolTip(i18nc("@info:tooltip",
                                          "This will clear all training data for recognition "
                                          "and rebuild it from all available faces."));

    settingsLayout->addWidget(detectionLabel,                  0, 0, 1, 1);
    settingsLayout->addWidget(d->accuracyInput,                1, 0, 1, 1);
    settingsLayout->addWidget(accuracyLabel,                   2, 0, 1, 1);
    settingsLayout->addWidget(new DLineWidget(Qt::Horizontal), 3, 0, 1, 1);
    settingsLayout->addWidget(d->useFullCpuButton,             4, 0, 1, 1);
    settingsLayout->addWidget(d->retrainAllButton,             5, 0, 1, 1);
    settingsLayout->setColumnStretch(0, 10);
    settingsLayout->setRowStretch(6, 10);

    d->tabWidget->addTab(settingsTab, i18nc("@title:tab", "Settings"));

    // ------------------------

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(d->optionGroupBox);
    vbx->addWidget(d->tabWidget);
    vbx->setContentsMargins(QMargins());
    setLayout(vbx);
}

void FaceScanWidget::setupConnections()
{
/*
     connect(d->detectButton, SIGNAL(toggled(bool)),
            d->alreadyScannedBox, SLOT(setEnabled(bool)));
*/
#ifdef ENABLE_DETECT_AND_RECOGNIZE
    connect(d->detectAndRecognizeButton, SIGNAL(toggled(bool)),
            d->alreadyScannedBox, SLOT(setEnabled(bool)));
#endif
    connect(d->detectButton, SIGNAL(toggled(bool)),
            this, SLOT(slotPrepareForDetect(bool)));

    connect(d->reRecognizeButton, SIGNAL(toggled(bool)),
            this, SLOT(slotPrepareForRecognize(bool)));

    connect(d->retrainAllButton, SIGNAL(toggled(bool)),
            this, SLOT(retrainAllButtonToggled(bool)));
}

void FaceScanWidget::slotPrepareForDetect(bool status)
{
    d->alreadyScannedBox->setEnabled(status);

    // Set default for Tag tab as all unchecked
    d->albumSelectors->resetTAlbumSelection();
}

void FaceScanWidget::slotPrepareForRecognize(bool /*status*/)
{
    /**
     * TODO:
     * reRecognizeButton always disables d->alreadyScannedBox, while it should be
     * Find out why and fix it.
     */
    d->alreadyScannedBox->setEnabled(false);

    // First we set all tags unchecked
    d->albumSelectors->resetTAlbumSelection();

    // Set default for Tag tab so that People and its children are checked
    AlbumList tagAlbums = AlbumManager::instance()->allTAlbums();
    QString people      = i18nc("People on your photos", "People");

    for (int i = 0 ; i < tagAlbums.size() ; ++i)
    {
        Album* const album = tagAlbums[i];

        if (album->title() == people ||
           ((album->parent() != nullptr) && (album->parent()->title() == people)))
        {
            d->albumSelectors->setTagSelected(album, false);
        }
    }
}

void FaceScanWidget::retrainAllButtonToggled(bool on)
{
    d->optionGroupBox->setEnabled(!on);
    d->albumSelectors->setEnabled(!on);
}

bool FaceScanWidget::settingsConflicted() const
{
    return d->settingsConflicted;
}

FaceScanSettings FaceScanWidget::settings() const
{
    FaceScanSettings settings;

    if (d->retrainAllButton->isChecked())
    {
        settings.task = FaceScanSettings::RetrainAll;
    }
    else if (d->detectButton->isChecked())
    {
        settings.task = FaceScanSettings::Detect;
    }
    else
    {
#ifdef ENABLE_DETECT_AND_RECOGNIZE
        if (d->detectAndRecognizeButton->isChecked())
        {
            settings.task = FaceScanSettings::DetectAndRecognize;
        }
        else // recognize only
#endif
        {
            settings.task = FaceScanSettings::RecognizeMarkedFaces;

            // preset settingsConflicted as True, since by default there are no tags to recognize
            d->settingsConflicted = true;
        }
    }

    settings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
                                      d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt();

    settings.accuracy               = double(d->accuracyInput->value()) / 100;

    // TODO: why does the original code append but not assign here???
    // settings.albums << d->albumSelectors->selectedAlbumsAndTags();
    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.useFullCpu             = d->useFullCpuButton->isChecked();

    return settings;
}

} // namespace Digikam
