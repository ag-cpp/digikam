/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * SPDX-FileCopyrightText: 2012-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedlg_p.h"

namespace Digikam
{

MaintenanceDlg::MaintenanceDlg(QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    setWindowTitle(i18nc("@title:window", "Maintenance"));
    setModal(true);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);

    QScrollArea* const main    = new QScrollArea(this);
    QWidget* const page        = new QWidget(main->viewport());
    main->setWidget(page);
    main->setWidgetResizable(true);

    QGridLayout* const grid    = new QGridLayout(page);

    d->logo                    = new QLabel(page);
    d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));
    d->title                   = new QLabel(i18n("<qt><b>Select Maintenance Operations to Process</b></qt>"), page);
    d->expanderBox             = new DExpanderBox(page);

    // --------------------------------------------------------------------------------------

    DVBox* const options       = new DVBox;
    d->albumSelectors          = new AlbumSelectors(i18nc("@label", "Process items from:"),
                                                    d->configGroupName, options, AlbumSelectors::AlbumType::All, true);
    d->useLastSettings         = new QCheckBox(i18nc("@option:check", "Use the last saved active tools and settings"), options);
    d->useMutiCoreCPU          = new QCheckBox(i18nc("@option:check", "Work on all processor cores (when it's possible)"), options);
    d->expanderBox->insertItem(Private::Options, options, QIcon::fromTheme(QLatin1String("configure")), i18n("Common Options"), QLatin1String("Options"), true);

    // --------------------------------------------------------------------------------------

    d->expanderBox->insertItem(Private::NewItems, new QLabel(i18n("<qt>No option<br/>"
                               "<i>Note: only Albums Collection are processed by this tool.</i></qt>")),
                               QIcon::fromTheme(QLatin1String("view-refresh")), i18n("Scan for new items"), QLatin1String("NewItems"), false);
    d->expanderBox->setCheckBoxVisible(Private::NewItems, true);

    // --------------------------------------------------------------------------------------

    d->vbox3                   = new DVBox;
    new QLabel(i18n("<qt><i>Note: If activated, the Core DB is always cleaned. You can select additional databases for cleaning.<br/>"
                    " If you select one of the options below, the process may take much time and can freeze digiKam temporarily<br/>"
                    " in order to make sure that no database corruption occurs.</i></qt>"), d->vbox3);
    d->cleanThumbsDb           = new QCheckBox(i18n("Also clean up the thumbnail database."), d->vbox3);
    d->cleanFacesDb            = new QCheckBox(i18n("Also clean up the faces database."), d->vbox3);
    d->cleanSimilarityDb       = new QCheckBox(i18n("Also clean up the similarity database."), d->vbox3);
    d->shrinkDatabases         = new QCheckBox(i18n("Extended clean up and shrink all databases."), d->vbox3);
    d->shrinkDatabases->setToolTip(i18n("This option leads to the vacuuming (shrinking) of the databases. "
                                        "Vacuuming is supported both for SQLite and MySQL."));
    d->expanderBox->insertItem(Private::DbCleanup, d->vbox3,
                               QIcon::fromTheme(QLatin1String("run-build")),
                               i18n("Perform database cleaning"), QLatin1String("DbCleanup"), false);
    d->expanderBox->setCheckBoxVisible(Private::DbCleanup, true);

    // --------------------------------------------------------------------------------------

    d->scanThumbs              = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::Thumbnails, d->scanThumbs, QIcon::fromTheme(QLatin1String("view-process-all")),
                               i18n("Rebuild Thumbnails"), QLatin1String("Thumbnails"), false);
    d->expanderBox->setCheckBoxVisible(Private::Thumbnails, true);

    // --------------------------------------------------------------------------------------

    d->scanFingerPrints        = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::FingerPrints, d->scanFingerPrints, QIcon::fromTheme(QLatin1String("run-build")),
                               i18n("Rebuild Finger-prints"), QLatin1String("Fingerprints"), false);
    d->expanderBox->setCheckBoxVisible(Private::FingerPrints, true);

    // --------------------------------------------------------------------------------------

    const ApplicationSettings* const settings = ApplicationSettings::instance();

    d->duplicatesBox                          = new DVBox;
    d->similarityRangeBox                     = new DHBox(d->duplicatesBox);
    new QLabel(i18n("Similarity range (in percents): "), d->similarityRangeBox);
    QWidget* const space                      = new QWidget(d->similarityRangeBox);
    d->similarityRangeBox->setStretchFactor(space, 10);

    d->similarityRange                        = new DIntRangeBox(d->similarityRangeBox);
    d->similarityRange->setSuffix(QLatin1String("%"));

    if (settings)
    {
        d->similarityRange->setRange(settings->getMinimumSimilarityBound(), 100);
        d->similarityRange->setInterval(settings->getDuplicatesSearchLastMinSimilarity(),
                                        settings->getDuplicatesSearchLastMaxSimilarity());
    }
    else
    {
        d->similarityRange->setRange(40, 100);
        d->similarityRange->setInterval(90, 100);
    }

    d->dupeRestrictionBox      = new DHBox(d->duplicatesBox);
    new QLabel(i18n("Restriction on duplicates:"), d->dupeRestrictionBox);
    QWidget* const space4      = new QWidget(d->dupeRestrictionBox);
    d->dupeRestrictionBox->setStretchFactor(space4, 10);
    d->searchResultRestriction = new QComboBox(d->dupeRestrictionBox);
    d->searchResultRestriction->addItem(i18n("No restriction"),                       HaarIface::DuplicatesSearchRestrictions::None);
    d->searchResultRestriction->addItem(i18n("Restrict to album of reference image"), HaarIface::DuplicatesSearchRestrictions::SameAlbum);
    d->searchResultRestriction->addItem(i18n("Exclude album of reference image"),     HaarIface::DuplicatesSearchRestrictions::DifferentAlbum);

    // Load the last choice from application settings.

    HaarIface::DuplicatesSearchRestrictions restrictions = HaarIface::DuplicatesSearchRestrictions::None;

    if (settings)
    {
        restrictions = (HaarIface::DuplicatesSearchRestrictions) settings->getDuplicatesSearchRestrictions();
    }

    d->searchResultRestriction->setCurrentIndex(d->searchResultRestriction->findData(restrictions));

    d->expanderBox->insertItem(Private::Duplicates, d->duplicatesBox, QIcon::fromTheme(QLatin1String("tools-wizard")),
                               i18n("Find Duplicate Items"), QLatin1String("Duplicates"), false);
    d->expanderBox->setCheckBoxVisible(Private::Duplicates, true);

    // --------------------------------------------------------------------------------------

    d->vbox4               = new DVBox;
    DHBox* const hbox3     = new DHBox(d->vbox4);
    new QLabel(i18n("Faces data management: "), hbox3);
    QWidget* const space3  = new QWidget(hbox3);
    hbox3->setStretchFactor(space3, 10);
    d->faceScannedHandling = new QComboBox(hbox3);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Skip images already scanned"),           FaceScanSettings::Skip);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Scan again and merge results"),          FaceScanSettings::Merge);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"),  FaceScanSettings::Rescan);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Clear all previous results and rescan"), FaceScanSettings::ClearAll);

    d->retrainAllFaces    = new QCheckBox(d->vbox4);
    d->retrainAllFaces->setText(i18nc("@option:check", "Clear and rebuild all training data"));
    d->retrainAllFaces->setToolTip(i18nc("@info:tooltip",
                                         "This will clear all training data for recognition "
                                         "and rebuild it from all available faces."));
    d->expanderBox->insertItem(Private::FaceManagement, d->vbox4, QIcon::fromTheme(QLatin1String("edit-image-face-detect")),
                               i18n("Detect and recognize Faces"), QLatin1String("FaceManagement"), false);
    d->expanderBox->setCheckBoxVisible(Private::FaceManagement, true);

    // --------------------------------------------------------------------------------------

    d->vbox5               = new DVBox;
    QLabel* const title    = new QLabel(d->vbox5);
    title->setText(i18nc("@label",
                        "<p><b>This tool allows to assign automatically tags to images by contents analysis using "
                        "deep-learning neural network.</b></p>"
                        "<p>The settings below determines the deep-learning model to use while parsing image "
                        "contents to determine the subjects of the photography. The neural network used in background "
                        "will generate automatically a serie of tags describing the contents and store the results in "
                        "the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox12    = new DHBox(d->vbox5);
    new QLabel (i18n("Auto-tagging mode: "), hbox12);
    QWidget* const space8  = new QWidget(hbox12);
    hbox12->setStretchFactor(space8, 10);

    d->autotaggingScanMode = new QComboBox(hbox12);
    d->autotaggingScanMode->addItem(i18n("Clean all and re-assign"),  AutotagsAssignment::AllItems);
    d->autotaggingScanMode->addItem(i18n("Scan non-assigned only"),   AutotagsAssignment::NonAssignedItems);

    DHBox* const hbox13    = new DHBox(d->vbox5);
    new QLabel(i18n("Selection model: "), hbox13);
    QWidget* const space9  = new QWidget(hbox13);
    hbox13->setStretchFactor(space9, 10);

    d->modelSelectionMode  = new QComboBox(hbox13);
    d->modelSelectionMode->addItem(i18n("YOLOv5 Nano"),   DetectorModel::YOLOV5NANO);
    d->modelSelectionMode->addItem(i18n("YOLOv5 XLarge"), DetectorModel::YOLOV5XLARGE);
    d->modelSelectionMode->addItem(i18n("ResNet50"),      DetectorModel::RESNET50);
    d->modelSelectionMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>YOLOv5 Nano</b>: this model is a neural network which offers exceptional speed and efficiency. It enables you to swiftly "
        "evaluate the integration of smaller-scale object detection scenarios. It's designed for objects detections, capable of recognizing "
        "and extracting the location of objects within an image. The limitation on the number of recognizable objects is set to 80.</p>"
        "<p><b>YOLOv5 XLarge</b>: as the previous one, this model is a neural network dedicated for more complex object detection requirements and "
        "showcases remarkable capabilities. Despite the additional complexity introducing more time-latency and "
        "computer resources, it's must be used for larger-scale object detection scenarios as it provides more accurate predictions at the expense of speed.</p>"
        "<p><b>ResNet50</b>: this model is a specific type of convolutional neural network formed by stacking residual blocks "
        "commonly used to power computer vision applications as object detections. This king of design allows the training of very deep networks without "
        "encountering the vanishing gradient problem. Unlike YOLO, ResNet50 is primarily focused on image classification and does not provide object localization. "
        "It can recognize objects from a vast set of more than 1,000 classes, covering a wide range of objects, animals, and scenes.</p>"));

    d->expanderBox->insertItem(Private::AutotagsAssignment, d->vbox5,
                               QIcon::fromTheme(QLatin1String("tag")), i18n("Auto-tags Assignment"),
                               QLatin1String("AutotagsAssignment"), false);
    d->expanderBox->setCheckBoxVisible(Private::AutotagsAssignment, true);

    // --------------------------------------------------------------------------------------

    d->vbox               = new DVBox;
    DHBox* const hbox11   = new DHBox(d->vbox);
    new QLabel(i18n("Scan Mode: "), hbox11);
    QWidget* const space7 = new QWidget(hbox11);
    hbox11->setStretchFactor(space7, 10);

    d->qualityScanMode    = new QComboBox(hbox11);
    d->qualityScanMode->addItem(i18n("Clean all and re-scan"),  ImageQualitySorter::AllItems);
    d->qualityScanMode->addItem(i18n("Scan non-assigned only"), ImageQualitySorter::NonAssignedItems);

    d->qualitySelector    = new ImageQualityConfSelector(d->vbox);

    d->expanderBox->insertItem(Private::ImageQualitySorter, d->vbox, QIcon::fromTheme(QLatin1String("flag-green")),
                               i18n("Image Quality Sorter"), QLatin1String("ImageQualitySorter"), false);
    d->expanderBox->setCheckBoxVisible(Private::ImageQualitySorter, true);

    // --------------------------------------------------------------------------------------

    d->vbox2              = new DVBox;
    DHBox* const hbox21   = new DHBox(d->vbox2);
    new QLabel(i18n("Sync Direction: "), hbox21);
    QWidget* const space5 = new QWidget(hbox21);
    hbox21->setStretchFactor(space5, 10);
    d->syncDirection      = new QComboBox(hbox21);
    d->syncDirection->addItem(i18n("From database to image metadata"), MetadataSynchronizer::WriteFromDatabaseToFile);
    d->syncDirection->addItem(i18n("From image metadata to database"), MetadataSynchronizer::ReadFromFileToDatabase);

    DHBox* const hbox22   = new DHBox(d->vbox2);
    new QLabel(i18n("Check metadata setup panel for details: "), hbox22);
    QWidget* const space6 = new QWidget(hbox22);
    hbox22->setStretchFactor(space6, 10);
    d->metadataSetup      = new QPushButton(i18n("Settings..."), hbox22);
    d->expanderBox->insertItem(Private::MetadataSync, d->vbox2, QIcon::fromTheme(QLatin1String("run-build-file")),
                               i18n("Sync Metadata and Database"), QLatin1String("MetadataSync"), false);
    d->expanderBox->setCheckBoxVisible(Private::MetadataSync, true);

    d->expanderBox->insertStretch(Private::Stretch);

    // --------------------------------------------------------------------------------------

    grid->addWidget(d->logo,        0, 0, 1, 1);
    grid->addWidget(d->title,       0, 1, 1, 1);
    grid->addWidget(d->expanderBox, 5, 0, 3, 2);
    grid->setSpacing(qMin(style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                          style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));
    grid->setContentsMargins(QMargins());
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(5, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(main);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------------------------------------

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOk()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->expanderBox, SIGNAL(signalItemToggled(int,bool)),
            this, SLOT(slotItemToggled(int,bool)));

    connect(d->useLastSettings, SIGNAL(toggled(bool)),
            this, SLOT(slotUseLastSettings(bool)));

    connect(d->metadataSetup, SIGNAL(clicked()),
            this, SLOT(slotMetadataSetup()));

    connect(d->qualitySelector, SIGNAL(signalQualitySetup()),
            this, SLOT(slotQualitySetup()));

    connect(d->retrainAllFaces, &QCheckBox::toggled,
            this, [hbox3](bool on)
        {
            hbox3->setEnabled(!on);
        }
    );

    // --------------------------------------------------------------------------------------

    readSettings();
}

MaintenanceDlg::~MaintenanceDlg()
{
    delete d;
}

void MaintenanceDlg::slotOk()
{
    writeSettings();
    accept();
}

MaintenanceSettings MaintenanceDlg::settings() const
{
    MaintenanceSettings prm;
    prm.wholeAlbums                         = d->albumSelectors->wholeAlbumsChecked();
    prm.wholeTags                           = d->albumSelectors->wholeTagsChecked();
    prm.albums                              = d->albumSelectors->selectedAlbums();
    prm.tags                                = d->albumSelectors->selectedTags();
    prm.useMutiCoreCPU                      = d->useMutiCoreCPU->isChecked();
    prm.newItems                            = d->expanderBox->isChecked(Private::NewItems);
    prm.databaseCleanup                     = d->expanderBox->isChecked(Private::DbCleanup);
    prm.cleanThumbDb                        = d->cleanThumbsDb->isChecked();
    prm.cleanFacesDb                        = d->cleanFacesDb->isChecked();
    prm.cleanSimilarityDb                   = d->cleanSimilarityDb->isChecked();
    prm.shrinkDatabases                     = d->shrinkDatabases->isChecked();
    prm.thumbnails                          = d->expanderBox->isChecked(Private::Thumbnails);
    prm.scanThumbs                          = d->scanThumbs->isChecked();
    prm.fingerPrints                        = d->expanderBox->isChecked(Private::FingerPrints);
    prm.scanFingerPrints                    = d->scanFingerPrints->isChecked();
    prm.duplicates                          = d->expanderBox->isChecked(Private::Duplicates);
    prm.minSimilarity                       = d->similarityRange->minValue();
    prm.maxSimilarity                       = d->similarityRange->maxValue();
    prm.duplicatesRestriction               = (HaarIface::DuplicatesSearchRestrictions)
                                                  d->searchResultRestriction->itemData(d->searchResultRestriction->currentIndex()).toInt();
    prm.faceManagement                      = d->expanderBox->isChecked(Private::FaceManagement);
    prm.faceSettings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
                                                  d->faceScannedHandling->itemData(d->faceScannedHandling->currentIndex()).toInt();
    prm.faceSettings.task                   = d->retrainAllFaces->isChecked() ? FaceScanSettings::RetrainAll
                                                                              : FaceScanSettings::DetectAndRecognize;
    prm.faceSettings.albums                 = d->albumSelectors->selectedAlbums();

    prm.autotagsAssignment                  = d->expanderBox->isChecked(Private::AutotagsAssignment);
    prm.autotaggingScanMode                 = d->autotaggingScanMode->itemData(d->autotaggingScanMode->currentIndex()).toInt();
    prm.modelSelectionMode                  = d->modelSelectionMode->itemData(d->modelSelectionMode->currentIndex()).toInt();

    prm.qualitySort                         = d->expanderBox->isChecked(Private::ImageQualitySorter);
    prm.qualityScanMode                     = d->qualityScanMode->itemData(d->qualityScanMode->currentIndex()).toInt();
    prm.qualitySettingsSelected             = (int)d->qualitySelector->settingsSelected();

    if (prm.qualitySettingsSelected == ImageQualityConfSelector::GlobalSettings)
    {
        ImageQualityContainer imgq;
        imgq.readFromConfig();
        prm.quality                         = imgq;
    }
    else
    {
        prm.quality                         = d->qualitySelector->customSettings();
    }

    prm.metadataSync                        = d->expanderBox->isChecked(Private::MetadataSync);
    prm.syncDirection                       = d->syncDirection->itemData(d->syncDirection->currentIndex()).toInt();

    return prm;
}

void MaintenanceDlg::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->readSettings(group);
    d->albumSelectors->loadState();

    d->useLastSettings->blockSignals(true);
    d->useLastSettings->setChecked(group.readEntry(d->configUseLastSettings, false));
    d->useLastSettings->blockSignals(false);

    if (d->useLastSettings->isChecked())
    {
        MaintenanceSettings prm;

        d->useMutiCoreCPU->setChecked(group.readEntry(d->configUseMutiCoreCPU,                                  prm.useMutiCoreCPU));
        d->expanderBox->setChecked(Private::NewItems,           group.readEntry(d->configNewItems,              prm.newItems));

        d->expanderBox->setChecked(Private::DbCleanup,          group.readEntry(d->configCleanupDatabase,       prm.databaseCleanup));
        d->cleanThumbsDb->setChecked(group.readEntry(d->configCleanupThumbDatabase,                             prm.cleanThumbDb));
        d->cleanFacesDb->setChecked(group.readEntry(d->configCleanupFacesDatabase,                              prm.cleanFacesDb));
        d->cleanSimilarityDb->setChecked(group.readEntry(d->configCleanupSimilarityDatabase,                    prm.cleanSimilarityDb));
        d->shrinkDatabases->setChecked(group.readEntry(d->configShrinkDatabases,                                prm.shrinkDatabases));

        d->expanderBox->setChecked(Private::Thumbnails,         group.readEntry(d->configThumbnails,            prm.thumbnails));
        d->scanThumbs->setChecked(group.readEntry(d->configScanThumbs,                                          prm.scanThumbs));

        d->expanderBox->setChecked(Private::FingerPrints,       group.readEntry(d->configFingerPrints,          prm.fingerPrints));
        d->scanFingerPrints->setChecked(group.readEntry(d->configScanFingerPrints,                              prm.scanFingerPrints));

        d->expanderBox->setChecked(Private::Duplicates,         group.readEntry(d->configDuplicates,            prm.duplicates));
        d->similarityRange->setInterval(group.readEntry(d->configMinSimilarity,                                 prm.minSimilarity),
                                        group.readEntry(d->configMaxSimilarity,                                 prm.maxSimilarity));
        int restrictions = d->searchResultRestriction->findData(group.readEntry(d->configDuplicatesRestriction, (int)prm.duplicatesRestriction));
        d->searchResultRestriction->setCurrentIndex(restrictions);

        d->expanderBox->setChecked(Private::FaceManagement,     group.readEntry(d->configFaceManagement,        prm.faceManagement));
        int faceHandling = d->faceScannedHandling->findData(group.readEntry(d->configFaceScannedHandling,       (int)prm.faceSettings.alreadyScannedHandling));
        d->faceScannedHandling->setCurrentIndex(faceHandling);

        d->expanderBox->setChecked(Private::AutotagsAssignment, group.readEntry(d->configAutotagsAssignment,    prm.autotagsAssignment));
        int tagScanMode  = d->autotaggingScanMode->findData(group.readEntry(d->configAutotaggingScanMode,       prm.autotaggingScanMode));
        d->autotaggingScanMode->setCurrentIndex(tagScanMode);
        int tagSelection = d->modelSelectionMode->findData(group.readEntry(d->configModelSelectionMode,         prm.modelSelectionMode));
        d->modelSelectionMode->setCurrentIndex(tagSelection);

        d->expanderBox->setChecked(Private::ImageQualitySorter, group.readEntry(d->configImageQualitySorter,    prm.qualitySort));
        int qualityMod   = d->qualityScanMode->findData(group.readEntry(d->configQualityScanMode,               prm.qualityScanMode));
        d->qualityScanMode->setCurrentIndex(qualityMod);
        d->qualitySelector->setSettingsSelected(
            (ImageQualityConfSelector::SettingsType)group.readEntry(d->configQualitySettingsSelected,           prm.qualitySettingsSelected));

        ImageQualityContainer imq;
        imq.readFromConfig(group);
        d->qualitySelector->setCustomSettings(imq);

        d->expanderBox->setChecked(Private::MetadataSync,       group.readEntry(d->configMetadataSync,          prm.metadataSync));
        int direction    = d->syncDirection->findData(group.readEntry(d->configSyncDirection,                   prm.syncDirection));
        d->syncDirection->setCurrentIndex(direction);
    }

    for (int i = Private::NewItems ; i < Private::Stretch ; ++i)
    {
        slotItemToggled(i, d->expanderBox->isChecked(i));
    }

    winId();
    DXmlGuiWindow::setGoodDefaultWindowSize(windowHandle());
    DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

void MaintenanceDlg::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->writeSettings(group);
    d->albumSelectors->saveState();

    group.writeEntry(d->configUseLastSettings, d->useLastSettings->isChecked());

    if (d->useLastSettings->isChecked())
    {
        MaintenanceSettings prm   = settings();

        group.writeEntry(d->configUseMutiCoreCPU,             prm.useMutiCoreCPU);
        group.writeEntry(d->configNewItems,                   prm.newItems);
        group.writeEntry(d->configCleanupDatabase,            prm.databaseCleanup);
        group.writeEntry(d->configCleanupThumbDatabase,       prm.cleanThumbDb);
        group.writeEntry(d->configCleanupFacesDatabase,       prm.cleanFacesDb);
        group.writeEntry(d->configCleanupSimilarityDatabase,  prm.cleanSimilarityDb);
        group.writeEntry(d->configShrinkDatabases,            prm.shrinkDatabases);
        group.writeEntry(d->configThumbnails,                 prm.thumbnails);
        group.writeEntry(d->configScanThumbs,                 prm.scanThumbs);
        group.writeEntry(d->configFingerPrints,               prm.fingerPrints);
        group.writeEntry(d->configScanFingerPrints,           prm.scanFingerPrints);
        group.writeEntry(d->configDuplicates,                 prm.duplicates);
        group.writeEntry(d->configMinSimilarity,              prm.minSimilarity);
        group.writeEntry(d->configMaxSimilarity,              prm.maxSimilarity);
        group.writeEntry(d->configDuplicatesRestriction,      (int)prm.duplicatesRestriction);
        group.writeEntry(d->configFaceManagement,             prm.faceManagement);
        group.writeEntry(d->configFaceScannedHandling,        (int)prm.faceSettings.alreadyScannedHandling);
        group.writeEntry(d->configAutotagsAssignment,         prm.autotagsAssignment);
        group.writeEntry(d->configAutotaggingScanMode,        prm.autotaggingScanMode);
        group.writeEntry(d->configModelSelectionMode,         prm.modelSelectionMode);
        group.writeEntry(d->configImageQualitySorter,         prm.qualitySort);
        group.writeEntry(d->configQualityScanMode,            prm.qualityScanMode);
        group.writeEntry(d->configQualitySettingsSelected,    prm.qualitySettingsSelected);

        ImageQualityContainer imq = d->qualitySelector->customSettings();
        imq.writeToConfig(group);

        group.writeEntry(d->configMetadataSync,               prm.metadataSync);
        group.writeEntry(d->configSyncDirection,              prm.syncDirection);
    }

    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
}

void MaintenanceDlg::slotItemToggled(int index, bool b)
{
    switch (index)
    {
        case Private::Thumbnails:
        {
            d->scanThumbs->setEnabled(b);
            break;
        }

        case Private::FingerPrints:
        {
            d->scanFingerPrints->setEnabled(b);
            break;
        }

        case Private::Duplicates:
        {
            d->duplicatesBox->setEnabled(b);
            break;
        }

        case Private::FaceManagement:
        {
            d->vbox4->setEnabled(b);
            break;
        }

        case Private::ImageQualitySorter:
        {
            d->vbox->setEnabled(b);
            break;
        }

        case Private::AutotagsAssignment:
        {
            d->vbox5->setEnabled(b);
            break;
        }

        case Private::MetadataSync:
        {
            d->vbox2->setEnabled(b);
            break;
        }

        case Private::DbCleanup:
        {
            d->vbox3->setEnabled(b);
            break;
        }

        default :  // NewItems
        {
            break;
        }
    }
}

void MaintenanceDlg::slotUseLastSettings(bool checked)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->configUseLastSettings, d->useLastSettings->isChecked());

    if (checked)
    {
        readSettings();
    }
    else
    {
        MaintenanceSettings prm;

        d->useMutiCoreCPU->setChecked(prm.useMutiCoreCPU);
        d->expanderBox->setChecked(Private::NewItems,           prm.newItems);

        d->expanderBox->setChecked(Private::DbCleanup,          prm.databaseCleanup);
        d->cleanThumbsDb->setChecked(prm.cleanThumbDb);
        d->cleanFacesDb->setChecked(prm.cleanFacesDb);
        d->cleanSimilarityDb->setChecked(prm.cleanSimilarityDb);
        d->shrinkDatabases->setChecked(prm.shrinkDatabases);

        d->expanderBox->setChecked(Private::Thumbnails,         prm.thumbnails);
        d->scanThumbs->setChecked(prm.scanThumbs);

        d->expanderBox->setChecked(Private::FingerPrints,       prm.fingerPrints);
        d->scanFingerPrints->setChecked(prm.scanFingerPrints);

        d->expanderBox->setChecked(Private::Duplicates,         prm.duplicates);
        d->similarityRange->setInterval(prm.minSimilarity,      prm.maxSimilarity);
        int restrictions = d->searchResultRestriction->findData(prm.duplicatesRestriction);
        d->searchResultRestriction->setCurrentIndex(restrictions);

        d->expanderBox->setChecked(Private::FaceManagement,     prm.faceManagement);
        d->faceScannedHandling->setCurrentIndex(prm.faceSettings.alreadyScannedHandling);

        d->expanderBox->setChecked(Private::AutotagsAssignment, prm.autotagsAssignment);
        d->autotaggingScanMode->setCurrentIndex(prm.autotaggingScanMode);

        d->expanderBox->setChecked(Private::ImageQualitySorter, prm.qualitySort);
        d->qualityScanMode->setCurrentIndex(prm.qualityScanMode);

        d->expanderBox->setChecked(Private::MetadataSync,       prm.metadataSync);
        d->syncDirection->setCurrentIndex(prm.syncDirection);
    }
}

void MaintenanceDlg::slotMetadataSetup()
{
    Setup::execSinglePage(this, Setup::MetadataPage);
}

void MaintenanceDlg::slotQualitySetup()
{
    Setup::execSinglePage(this, Setup::ImageQualityPage);
}

void MaintenanceDlg::slotHelp()
{
    openOnlineDocumentation(QLatin1String("maintenance_tools"));
}

} // namespace Digikam
