/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2003-2004 by Ralf Holzer <ralf at well dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#include "setupmetadata.h"

// Qt includes

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QRadioButton>
#include <QStandardPaths>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "advancedmetadatatab.h"
#include "applicationsettings.h"
#include "dactivelabel.h"
#include "digikam_config.h"
#include "digikam_debug.h"
#include "metaengine.h"
#include "metadatapanel.h"
#include "metaenginesettings.h"
#include "setuputils.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupMetadata::Private
{
public:

    explicit Private()
      : exifAutoRotateOriginal  (false),
        exifAutoRotateShowedInfo(false),
        clearMetadataShowedInfo (false),
        fieldsGroup             (nullptr),
        readWriteGroup          (nullptr),
        rotationGroup           (nullptr),
        rotationAdvGroup        (nullptr),
        saveTagsBox             (nullptr),
        saveCommentsBox         (nullptr),
        saveRatingBox           (nullptr),
        savePickLabelBox        (nullptr),
        saveColorLabelBox       (nullptr),
        saveDateTimeBox         (nullptr),
        saveTemplateBox         (nullptr),
        saveFaceTags            (nullptr),
        savePosition            (nullptr),
        useLazySync             (nullptr),
        writeDngFilesBox        (nullptr),
        writeRawFilesBox        (nullptr),
        writeXMPSidecarBox      (nullptr),
        readXMPSidecarBox       (nullptr),
        sidecarFileNameBox      (nullptr),
        updateFileTimeStampBox  (nullptr),
        rescanImageIfModifiedBox(nullptr),
        clearMetadataIfRescanBox(nullptr),
        writingModeCombo        (nullptr),
        rotateByFlag            (nullptr),
        rotateByContents        (nullptr),
        allowRotateByMetadata   (nullptr),
        allowLossyRotate        (nullptr),
        exifRotateBox           (nullptr),
        exifSetOrientationBox   (nullptr),
        saveToBalooBox          (nullptr),
        readFromBalooBox        (nullptr),
        tab                     (nullptr),
        displaySubTab           (nullptr),
        tagsCfgPanel            (nullptr),
        advTab                  (nullptr),
        extensionsEdit          (nullptr)
    {
    }

    bool                 exifAutoRotateOriginal;
    bool                 exifAutoRotateShowedInfo;
    bool                 clearMetadataShowedInfo;

    QGroupBox*           fieldsGroup;
    QGroupBox*           readWriteGroup;
    QGroupBox*           rotationGroup;
    QGroupBox*           rotationAdvGroup;

    QCheckBox*           saveTagsBox;
    QCheckBox*           saveCommentsBox;
    QCheckBox*           saveRatingBox;
    QCheckBox*           savePickLabelBox;
    QCheckBox*           saveColorLabelBox;
    QCheckBox*           saveDateTimeBox;
    QCheckBox*           saveTemplateBox;
    QCheckBox*           saveFaceTags;
    QCheckBox*           savePosition;

    QCheckBox*           useLazySync;
    QCheckBox*           writeDngFilesBox;
    QCheckBox*           writeRawFilesBox;
    QCheckBox*           writeXMPSidecarBox;
    QCheckBox*           readXMPSidecarBox;
    QCheckBox*           sidecarFileNameBox;
    QCheckBox*           updateFileTimeStampBox;
    QCheckBox*           rescanImageIfModifiedBox;
    QCheckBox*           clearMetadataIfRescanBox;
    QComboBox*           writingModeCombo;

    QRadioButton*        rotateByFlag;
    QRadioButton*        rotateByContents;
    QCheckBox*           allowRotateByMetadata;
    QCheckBox*           allowLossyRotate;
    QCheckBox*           exifRotateBox;
    QCheckBox*           exifSetOrientationBox;

    QCheckBox*           saveToBalooBox;
    QCheckBox*           readFromBalooBox;

    QTabWidget*          tab;
    QTabWidget*          displaySubTab;

    MetadataPanel*       tagsCfgPanel;
    AdvancedMetadataTab* advTab;

    QLineEdit*           extensionsEdit;
};

SetupMetadata::SetupMetadata(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab                          = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    QWidget* const panel            = new QWidget;
    QVBoxLayout* const mainLayout   = new QVBoxLayout;

    // --------------------------------------------------------

    d->fieldsGroup                  = new QGroupBox;
    QGridLayout* const fieldsLayout = new QGridLayout;

    d->fieldsGroup->setWhatsThis(xi18nc("@info:whatsthis",
                                        "<para>In addition to the pixel content, image files usually "
                                        "contain a variety of metadata. A lot of the parameters you can use "
                                        "in digiKam to manage files, such as rating or comment, can be written "
                                        "to the files' metadata.</para> "
                                        "<para>Storing in metadata allows one to preserve this information "
                                        "when moving or sending the files to different systems.</para>"));

    QLabel* const fieldsIconLabel = new QLabel;
    fieldsIconLabel->setPixmap(QIcon::fromTheme(QLatin1String("format-list-unordered")).pixmap(32));

    QLabel* const fieldsLabel     = new QLabel(i18nc("@label", "Write This Information to the Metadata"));

    d->saveTagsBox       = new QCheckBox;
    d->saveTagsBox->setText(i18nc("@option:check", "Image tags"));
    d->saveTagsBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item tags "
                                       "in the XMP and IPTC tags."));

    d->saveCommentsBox   = new QCheckBox;
    d->saveCommentsBox->setText(i18nc("@option:check", "Captions and title"));
    d->saveCommentsBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store item caption and title "
                                           "in the JFIF Comment section, the Exif tag, the XMP tag, "
                                           "and the IPTC tag."));

    d->saveRatingBox     = new QCheckBox;
    d->saveRatingBox->setText(i18nc("@option:check", "Rating"));
    d->saveRatingBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item rating "
                                         "in the Exif tag and the XMP tags."));

    d->savePickLabelBox  = new QCheckBox;
    d->savePickLabelBox->setText(i18nc("@option:check", "Pick label"));
    d->savePickLabelBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item pick label "
                                            "in the XMP tags."));

    d->saveColorLabelBox = new QCheckBox;
    d->saveColorLabelBox->setText(i18nc("@option:check", "Color label"));
    d->saveColorLabelBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item color label "
                                             "in the XMP tags."));

    d->saveDateTimeBox   = new QCheckBox;
    d->saveDateTimeBox->setText(i18nc("@option:check", "Timestamps"));
    d->saveDateTimeBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the item date and time "
                                           "in the EXIF, XMP, and IPTC tags."));

    d->saveTemplateBox   = new QCheckBox;
    d->saveTemplateBox->setText(i18nc("@option:check", "Metadata templates (Copyright etc.)"));
    d->saveTemplateBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store the metadata "
                                           "template in the XMP and the IPTC tags. "
                                           "You can set template values to Template setup page."));
    d->saveFaceTags      = new QCheckBox;
    d->saveFaceTags->setText(i18nc("@option:check", "Face Tags (including face areas)"));
    d->saveFaceTags->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store face tags "
                                        "with face rectangles in the XMP tags."));

    d->savePosition      = new QCheckBox;
    d->savePosition->setText(i18nc("@option:check", "Geolocation information (GPS)"));
    d->savePosition->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to store Geolocation information "
                                        "in the Exif tag and the XMP tags."));

    fieldsLayout->addWidget(fieldsIconLabel,       0, 0, 2, 3);
    fieldsLayout->addWidget(fieldsLabel,           0, 1, 2, 3);
    fieldsLayout->addWidget(d->saveTagsBox,        2, 0, 1, 3);
    fieldsLayout->addWidget(d->saveCommentsBox,    3, 0, 1, 3);
    fieldsLayout->addWidget(d->saveRatingBox,      4, 0, 1, 3);
    fieldsLayout->addWidget(d->savePickLabelBox,   5, 0, 1, 3);
    fieldsLayout->addWidget(d->saveColorLabelBox,  6, 0, 1, 3);
    fieldsLayout->addWidget(d->saveDateTimeBox,    7, 0, 1, 3);
    fieldsLayout->addWidget(d->saveTemplateBox,    8, 0, 1, 3);
    fieldsLayout->addWidget(d->saveFaceTags,       9, 0, 1, 3);
    fieldsLayout->addWidget(d->savePosition,      10, 0, 1, 3);
    fieldsLayout->setColumnStretch(3, 10);
    d->fieldsGroup->setLayout(fieldsLayout);

    // --------------------------------------------------------

    d->readWriteGroup                  = new QGroupBox;
    QGridLayout* const readWriteLayout = new QGridLayout;

    QLabel* const readWriteIconLabel   = new QLabel;
    readWriteIconLabel->setPixmap(QIcon::fromTheme(QLatin1String("document-open")).pixmap(32));

    QLabel* const readWriteLabel       = new QLabel(i18nc("@label", "Reading and Writing Metadata"));

    d->useLazySync        = new QCheckBox;
    d->useLazySync->setText(i18nc("@option:check", "Use lazy synchronization"));
    d->useLazySync->setWhatsThis(i18nc("@info:whatsthis",
                                       "Instead of synchronizing metadata, just schedule it for synchronization."
                                       "Synchronization can be done later by triggering the apply pending, or at digikam exit"));

    d->writeDngFilesBox   = new QCheckBox;
    d->writeDngFilesBox->setText(i18nc("@option:check", "Write metadata to DNG files"));
    d->writeDngFilesBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to write metadata into DNG files."));
    d->writeDngFilesBox->setEnabled(MetaEngine::supportMetadataWriting(QLatin1String("image/x-raw")));

    d->writeRawFilesBox   = new QCheckBox;
    d->writeRawFilesBox->setText(i18nc("@option:check", "If possible write metadata to RAW files (Dangerous)"));
    d->writeRawFilesBox->setWhatsThis(i18nc("@info:whatsthis", "Turn on this option to write metadata into RAW TIFF/EP files. "
                                            "This feature requires the Exiv2 shared library, version >= 0.18.0. It is still "
                                            "experimental, and is disabled by default. Activate this option only if you know what you are doing."));
    d->writeRawFilesBox->setEnabled(MetaEngine::supportMetadataWriting(QLatin1String("image/x-raw")));

    d->updateFileTimeStampBox = new QCheckBox;
    d->updateFileTimeStampBox->setText(i18nc("@option:check", "&Update file modification timestamp when files are modified"));
    d->updateFileTimeStampBox->setWhatsThis(i18nc("@info:whatsthis",
                                                  "Turn off this option to not update file timestamps when files are changed as "
                                                  "when you update metadata or image data. Note: disabling this option can "
                                                  "introduce some dysfunctions with applications which use file timestamps "
                                                  "properties to detect file modifications automatically."));

    d->rescanImageIfModifiedBox = new QCheckBox;
    d->rescanImageIfModifiedBox->setText(i18nc("@option:check", "&Rescan file when files are modified"));
    d->rescanImageIfModifiedBox->setWhatsThis(i18nc("@info:whatsthis",
                                                    "Turning this option on, will force digiKam to rescan files that has been "
                                                    "modified outside digiKam. If a file has changed it is file size or if "
                                                    "the last modified timestamp has changed, a rescan of that "
                                                    "file will be performed when digiKam starts."));

    d->clearMetadataIfRescanBox    = new QCheckBox;
    d->clearMetadataIfRescanBox->setText(i18nc("@option:check", "&Clean up the metadata from the database when rescan files"));
    d->clearMetadataIfRescanBox->setWhatsThis(i18nc("@info:whatsthis",
                                                    "Turning this option on, will force digiKam to delete the file metadata "
                                                    "contained in the database before the file is rescanned. WARNING: "
                                                    "if your metadata has been written to the database only and not "
                                                    "to the file or sidecar, you will be able to lose inserted "
                                                    "metadata such as tags, keywords, or geographic coordinates."));

    readWriteLayout->addWidget(readWriteIconLabel,          0, 0, 2, 3);
    readWriteLayout->addWidget(readWriteLabel,              0, 1, 2, 3);
    readWriteLayout->addWidget(d->useLazySync,              2, 0, 1, 3);
    readWriteLayout->addWidget(d->writeDngFilesBox,         3, 0, 1, 3);
    readWriteLayout->addWidget(d->writeRawFilesBox,         4, 0, 1, 3);
    readWriteLayout->addWidget(d->updateFileTimeStampBox,   5, 0, 1, 3);
    readWriteLayout->addWidget(d->rescanImageIfModifiedBox, 6, 0, 1, 3);
    readWriteLayout->addWidget(d->clearMetadataIfRescanBox, 7, 0, 1, 3);
    readWriteLayout->setColumnStretch(3, 10);
    d->readWriteGroup->setLayout(readWriteLayout);

    // --------------------------------------------------------

    QFrame* const infoBox           = new QFrame;
    QGridLayout* const infoBoxGrid  = new QGridLayout;
    infoBox->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    DActiveLabel* const exiv2LogoLabel = new DActiveLabel(QUrl(QLatin1String("https://www.exiv2.org")),
                                                          QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/logo-exiv2.png")),
                                                          infoBox);
    exiv2LogoLabel->setWhatsThis(i18nc("@info", "Visit Exiv2 project website"));

    QLabel* const explanation = new QLabel(infoBox);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    QString txt;

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Exif'>Exif</a> - %1</p>")
               .arg(i18nc("@info", "a standard used by most digital cameras today to store technical "
                          "information (like aperture and shutter speed) about an image.")));

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> - %1</p>")
               .arg(i18nc("@info", "an older standard used in digital photography to store "
                          "photographer information in images.")));

    if (MetaEngine::supportXmp())
    {
        txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> - %1</p>")
                   .arg(i18nc("@info", "a new standard used in digital photography, designed to replace IPTC.")));
    }

    explanation->setText(txt);

    infoBoxGrid->addWidget(exiv2LogoLabel, 0, 0, 1, 1);
    infoBoxGrid->addWidget(explanation,    0, 1, 1, 2);
    infoBoxGrid->setColumnStretch(1, 10);
    infoBoxGrid->setRowStretch(1, 10);
    infoBoxGrid->setSpacing(0);
    infoBox->setLayout(infoBoxGrid);

    // --------------------------------------------------------

    mainLayout->addWidget(d->fieldsGroup);
    mainLayout->addWidget(d->readWriteGroup);
    mainLayout->addWidget(infoBox);
    panel->setLayout(mainLayout);

    d->tab->insertTab(Behavior, panel, i18nc("@title:tab", "Behavior"));

    // --------------------------------------------------------

    QWidget* const rotationPanel      = new QWidget(d->tab);
    QVBoxLayout* const rotationLayout = new QVBoxLayout;

    d->rotationGroup                       = new QGroupBox;
    QGridLayout* const rotationGroupLayout = new QGridLayout;

    QLabel* const rotationExplanation = new QLabel(i18nc("@label", "When rotating a file"));
    QLabel* const rotationIcon        = new QLabel;
    rotationIcon->setPixmap(QIcon::fromTheme(QLatin1String("transform-rotate")).pixmap(32));

    d->rotateByFlag          = new QRadioButton(i18nc("@option:radio", "Rotate by only setting a flag"));
    d->rotateByContents      = new QRadioButton(i18nc("@option:radio", "Rotate by changing the content if possible"));
    d->allowLossyRotate      = new QCheckBox(i18nc("@option:check", "Even allow lossy rotation if necessary"));
    d->allowRotateByMetadata = new QCheckBox(i18nc("@option:check", "Write flag to metadata if possible"));

    connect(d->rotateByContents, SIGNAL(toggled(bool)),
            d->allowLossyRotate, SLOT(setEnabled(bool)));

    d->rotateByFlag->setChecked(false);
    d->rotateByContents->setChecked(false);
    d->allowLossyRotate->setEnabled(false);
    d->allowLossyRotate->setChecked(false);
    d->allowRotateByMetadata->setChecked(true);

    d->rotateByFlag->setToolTip(i18nc("@info:tooltip",
                                      "Rotate files only by changing a flag, not touching the pixel data"));
    d->rotateByFlag->setWhatsThis(xi18nc("@info:whatsthis",
                                         "<para>A file can be rotated in two ways:<nl/> "
                                         "You can change the contents, rearranging the individual pixels of the image data.<nl/> "
                                         "Or you can set a flag that the file is to be rotated before it is shown.</para> "
                                         "<para>Select this option if you always want to set only a flag. "
                                         "This is less obtrusive, but requires support if the file is accessed with another software. "
                                         "Ensure to allow setting the flag in the metadata if you want to share your files "
                                         "outside digiKam.</para>"));

    d->rotateByContents->setToolTip(i18nc("@info:tooltip",
                                          "If possible rotate files by changing the pixel data"));
    d->rotateByContents->setWhatsThis(xi18nc("@info:whatsthis",
                                             "<para>A file can be rotated in two ways:<nl/> "
                                             "You can change the contents, rearranging the individual pixels of the image data.<nl/> "
                                             "Or you can set a flag that the file is to be rotated before it is shown.</para> "
                                             "<para>Select this option if you want the file to be rotated by changing the content. "
                                             "This is a lossless operation for JPEG files. For other formats it is a lossy operation, "
                                             "which you need to enable explicitly. "
                                             "It is not support for RAW and other read-only formats, "
                                             "which will be rotated by flag only.</para>"));

    d->allowLossyRotate->setToolTip(i18nc("@info:tooltip",
                                          "Rotate files by changing the pixel data even if the operation will incur quality loss"));
    d->allowLossyRotate->setWhatsThis(i18nc("@info:whatsthis",
                                            "For some file formats which apply lossy compression, "
                                            "data will be lost each time the content is rotated. "
                                            "Check this option to allow lossy rotation. "
                                            "If not enabled, these files will be rotated by flag."));

    d->allowRotateByMetadata->setToolTip(i18nc("@info:tooltip",
                                               "When rotating a file by setting a flag, also change this flag in the file's metadata"));
    d->allowRotateByMetadata->setWhatsThis(i18nc("@info:whatsthis",
                                                 "File metadata typically contains a flag describing "
                                                 "that a file shall be shown rotated. "
                                                 "Enable this option to allow editing this field. "));

    rotationGroupLayout->addWidget(rotationIcon,             0, 0, 1, 1);
    rotationGroupLayout->addWidget(rotationExplanation,      0, 1, 1, 2);
    rotationGroupLayout->addWidget(d->rotateByFlag,          1, 0, 1, 3);
    rotationGroupLayout->addWidget(d->rotateByContents,      2, 0, 1, 3);
    rotationGroupLayout->addWidget(d->allowLossyRotate,      3, 2, 1, 1);
    rotationGroupLayout->addWidget(d->allowRotateByMetadata, 4, 0, 1, 3);
    rotationGroupLayout->setColumnStretch(3, 10);

    d->rotationGroup->setLayout(rotationGroupLayout);

    // --------------------------------------------------------

    d->rotationAdvGroup                  = new QGroupBox;
    QGridLayout* const rotationAdvLayout = new QGridLayout;

    QLabel* const rotationAdvExpl        = new QLabel(i18nc("@label", "Rotate actions"));
    QLabel* const rotationAdvIcon        = new QLabel;
    rotationAdvIcon->setPixmap(QIcon::fromTheme(QLatin1String("configure")).pixmap(32));

    d->exifRotateBox                     = new QCheckBox;
    d->exifRotateBox->setText(i18nc("@option", "Show images/thumbnails &rotated according to orientation tag."));
    d->exifSetOrientationBox             = new QCheckBox;
    d->exifSetOrientationBox->setText(i18nc("@option", "Set orientation tag to normal after rotate/flip."));

    rotationAdvLayout->addWidget(rotationAdvIcon,          0, 0, 1, 1);
    rotationAdvLayout->addWidget(rotationAdvExpl,          0, 1, 1, 1);
    rotationAdvLayout->addWidget(d->exifRotateBox,         1, 0, 1, 3);
    rotationAdvLayout->addWidget(d->exifSetOrientationBox, 2, 0, 1, 3);
    rotationAdvLayout->setColumnStretch(2, 10);
    d->rotationAdvGroup->setLayout(rotationAdvLayout);

    // --------------------------------------------------------

    QLabel* const rotationNote = new QLabel(i18nc("@info", "Note: These settings affect the album view "
                                                           "and not the image editor. The image editor always "
                                                           "changes the image data during the rotation."));
    rotationNote->setWordWrap(true);
    rotationNote->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    rotationLayout->addWidget(d->rotationGroup);
    rotationLayout->addWidget(d->rotationAdvGroup);
    rotationLayout->addWidget(rotationNote);
    rotationLayout->addStretch();
    rotationPanel->setLayout(rotationLayout);

    d->tab->insertTab(Rotation, rotationPanel, i18nc("@title:tab", "Rotation"));

    // --------------------------------------------------------

    QWidget* const displayPanel      = new QWidget;
    QGridLayout* const displayLayout = new QGridLayout;

    QLabel* const displayLabel       = new QLabel(i18nc("@label", "Select Metadata Fields to be Displayed "
                                                                  "when using Custom Filters"));

    QLabel* const displayIcon        = new QLabel;
    displayIcon->setPixmap(QIcon::fromTheme(QLatin1String("view-list-tree")).pixmap(32));

    d->displaySubTab                 = new QTabWidget;
    d->tagsCfgPanel                  = new MetadataPanel(d->displaySubTab);

    displayLayout->addWidget(displayIcon,      0, 0);
    displayLayout->addWidget(displayLabel,     0, 1);
    displayLayout->addWidget(d->displaySubTab, 1, 0, 1, 3);
    displayLayout->setColumnStretch(2, 1);

    displayPanel->setLayout(displayLayout);
    d->tab->insertTab(Display, displayPanel, i18nc("@title:tab", "Views"));

    // --------------------------------------------------------

#ifdef HAVE_KFILEMETADATA

    QWidget* const balooPanel      = new QWidget(d->tab);
    QVBoxLayout* const balooLayout = new QVBoxLayout(balooPanel);

    QGroupBox* const balooGroup    = new QGroupBox(i18nc("@option", "Baloo Desktop Search"), balooPanel);
    QVBoxLayout* const gLayout3    = new QVBoxLayout(balooGroup);

    d->saveToBalooBox              = new QCheckBox;
    d->saveToBalooBox->setText(i18nc("@option", "Store metadata from digiKam in Baloo"));
    d->saveToBalooBox->setWhatsThis(i18nc("@info", "Turn on this option to push rating, comments and tags "
                                                   "from digiKam into the Baloo storage"));

    d->readFromBalooBox            = new QCheckBox;
    d->readFromBalooBox->setText(i18nc("@option", "Read metadata from Baloo"));
    d->readFromBalooBox->setWhatsThis(i18nc("@info", "Turn on this option if you want to apply changes to "
                                                     "rating, comments and tags made in Baloo to digiKam's metadata storage. "
                                                     "Please note that image metadata will not be edited automatically."));

    gLayout3->addWidget(d->saveToBalooBox);
    gLayout3->addWidget(d->readFromBalooBox);

    d->tab->insertTab(Baloo, balooPanel, i18nc("@title:tab", "Baloo"));

    // --------------------------------------------------------

    QFrame* const balooBox         = new QFrame(balooPanel);
    QGridLayout* const balooGrid   = new QGridLayout(balooBox);
    balooBox->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    QLabel* const balooLogoLabel   = new QLabel;
    balooLogoLabel->setPixmap(QIcon::fromTheme(QLatin1String("baloo")).pixmap(48));

    QLabel* const balooExplanation = new QLabel(balooBox);
    balooExplanation->setOpenExternalLinks(true);
    balooExplanation->setWordWrap(true);
    QString balootxt;

    balootxt.append(QString::fromUtf8("<p><a href='https://community.kde.org/Baloo'>Baloo</a> %1</p>")
                    .arg(i18nc("@info", "provides the basis to handle all kinds of metadata on the KDE desktop in a generic fashion. "
                               "It allows you to tag, rate and comment your files in KDE applications like Dolphin. "
                               "Please set here if you want to synchronize the metadata stored by digiKam desktop-wide with the "
                               "Baloo Desktop Search.")));

    balooExplanation->setText(balootxt);

    balooGrid->addWidget(balooLogoLabel,   0, 0, 1, 1);
    balooGrid->addWidget(balooExplanation, 0, 1, 1, 2);
    balooGrid->setColumnStretch(1, 10);
    balooGrid->setSpacing(0);

    // --------------------------------------------------------

    balooLayout->addWidget(balooGroup);
    balooLayout->addWidget(balooBox);
/*
    balooLayout->addWidget(d->resyncButton, 0, Qt::AlignRight);
*/
    balooLayout->addStretch();

#endif // HAVE_KFILEMETADATA

    //--------------Advanced Metadata Configuration --------------

    d->advTab = new AdvancedMetadataTab(this);
    d->tab->insertTab(AdvancedConfig, d->advTab, i18nc("@title:tab advanced metadata config view", "Advanced"));

    //------------------------Sidecars-------------------------

    QWidget* const sidecarsPanel      = new QWidget(d->tab);
    QVBoxLayout* const sidecarsLayout = new QVBoxLayout(sidecarsPanel);

    // --------------------------------------------------------

    QGroupBox* rwSidecarsGroup          = new QGroupBox;
    QGridLayout* const rwSidecarsLayout = new QGridLayout;

    QLabel* const rwSidecarsLabel       = new QLabel(i18nc("@label", "Reading and Writing to Sidecars"));

    d->readXMPSidecarBox  = new QCheckBox;
    d->readXMPSidecarBox->setText(i18nc("@option:check", "Read from sidecar files"));
    d->readXMPSidecarBox->setWhatsThis(i18nc("@info:whatsthis",
                                             "Turn on this option to read metadata from XMP sidecar files when reading metadata."));
    d->readXMPSidecarBox->setEnabled(MetaEngine::supportXmp());

    d->writeXMPSidecarBox = new QCheckBox;
    d->writeXMPSidecarBox->setText(i18nc("@option:check", "Write to sidecar files"));
    d->writeXMPSidecarBox->setWhatsThis(i18nc("@info:whatsthis",
                                              "Turn on this option to save, as specified, metadata to XMP sidecar files."));
    d->writeXMPSidecarBox->setEnabled(MetaEngine::supportXmp());

    d->writingModeCombo   = new QComboBox;
    d->writingModeCombo->addItem(i18nc("@item", "Write to XMP sidecar for read-only item only"), MetaEngine::WRITE_TO_SIDECAR_ONLY_FOR_READ_ONLY_FILES);
    d->writingModeCombo->addItem(i18nc("@item", "Write to XMP sidecar only"),                    MetaEngine::WRITE_TO_SIDECAR_ONLY);
    d->writingModeCombo->addItem(i18nc("@item", "Write to item and XMP Sidecar"),                MetaEngine::WRITE_TO_SIDECAR_AND_FILE);
    d->writingModeCombo->setToolTip(i18nc("@info:tooltip", "Specify the exact mode of XMP sidecar writing"));
    d->writingModeCombo->setEnabled(false);

    d->sidecarFileNameBox = new QCheckBox;
    d->sidecarFileNameBox->setText(i18nc("@option:check", "Sidecar file names are compatible with commercial programs"));
    d->sidecarFileNameBox->setWhatsThis(i18nc("@info:whatsthis",
                                              "Turn on this option to create the XMP sidecar files with a compatible "
                                              "file name (BASENAME.xmp) used by many commercial programs. "
                                              "For Darktable do not enable this option."));
    d->sidecarFileNameBox->setEnabled(false);

    connect(d->writeXMPSidecarBox, SIGNAL(toggled(bool)),
            d->writingModeCombo, SLOT(setEnabled(bool)));

    connect(d->writeXMPSidecarBox, SIGNAL(toggled(bool)),
            d->sidecarFileNameBox, SLOT(setEnabled(bool)));

    rwSidecarsLayout->addWidget(rwSidecarsLabel,       0, 0, 1, 3);
    rwSidecarsLayout->addWidget(d->readXMPSidecarBox,  1, 0, 1, 3);
    rwSidecarsLayout->addWidget(d->writeXMPSidecarBox, 2, 0, 1, 3);
    rwSidecarsLayout->addWidget(d->writingModeCombo,   3, 1, 1, 2);
    rwSidecarsLayout->addWidget(d->sidecarFileNameBox, 4, 0, 1, 3);
    rwSidecarsLayout->setColumnStretch(3, 10);
    rwSidecarsGroup->setLayout(rwSidecarsLayout);

    // --------------------------------------------------------

    QGroupBox* const extensionsGroup   = new QGroupBox(sidecarsPanel);
    QGridLayout* const extensionsGrid  = new QGridLayout(extensionsGroup);

    QLabel* const extensionsGroupLabel = new QLabel(
                i18nc("@info", "Add file types to be recognised as sidecars.\n"
                      "digiKam (optionally) writes metadata to *.xmp sidecar\n"
                      "files. Other programs might use different types, which\n"
                      "can be specified below. digiKam will neither display these\n"
                      "nor read from or write to them. But whenever a matching album\n"
                      "item (e.g. \"image.dng\" for \"image.dng.pp3\") is renamed,\n"
                      "moved, copied or deleted, the same operation will be done\n"
                      "on these sidecar files.\n"
                      "Multiple extensions must be separated by a semicolon or a space."));
    extensionsGroupLabel->setWordWrap(true);

    QLabel* const extensionsLogo = new QLabel(extensionsGroup);
    extensionsLogo->setPixmap(QIcon::fromTheme(QLatin1String("text-x-texinfo")).pixmap(48));

    d->extensionsEdit            = new QLineEdit(extensionsGroup);
    d->extensionsEdit->setWhatsThis(i18nc("@info", "Here you can add extra extensions\n"
                                          "of sidecars files to be processed alongside\n"
                                          "regular items. These files will not be visible,\n"
                                          "but regarded as an extension of the main file.\n"
                                          "Just write \"xyz abc\" to support files with\n"
                                          "the *.xyz and *.abc extensions. The internally\n"
                                          "used sidecars type *.xmp is always included."));
    d->extensionsEdit->setClearButtonEnabled(true);
    d->extensionsEdit->setPlaceholderText(i18nc("@info", "Enter additional sidecars file extensions."));

    QLabel* const extensionsLabel = new QLabel(extensionsGroup);
    extensionsLabel->setText(i18nc("@info", "Additional &sidecar file extensions"));
    extensionsLabel->setBuddy(d->extensionsEdit);

    extensionsGrid->addWidget(extensionsGroupLabel, 0, 0, 1, -1);
    extensionsGrid->addWidget(extensionsLogo,       1, 0, 2, 1);
    extensionsGrid->addWidget(extensionsLabel,      1, 1, 1, -1);
    extensionsGrid->addWidget(d->extensionsEdit,    2, 1, 1, -1);
    extensionsGrid->setColumnStretch(1, 10);

    // --------------------------------------------------------

    sidecarsLayout->addWidget(rwSidecarsGroup);
    sidecarsLayout->addWidget(extensionsGroup);
    sidecarsLayout->addStretch();

    d->tab->insertTab(Sidecars, sidecarsPanel, i18nc("@title:tab", "Sidecars"));

    // --------------------------------------------------------

    readSettings();

    connect(d->exifRotateBox, SIGNAL(toggled(bool)),
            this, SLOT(slotExifAutoRotateToggled(bool)));

    connect(d->clearMetadataIfRescanBox, SIGNAL(toggled(bool)),
            this, SLOT(slotClearMetadataToggled(bool)));

    connect(d->writeRawFilesBox, SIGNAL(toggled(bool)),
            this, SLOT(slotWriteRawFilesToggled(bool)));
}

SetupMetadata::~SetupMetadata()
{
    delete d;
}

void SetupMetadata::setActiveMainTab(MetadataTab tab)
{
    d->tab->setCurrentIndex(tab);
}

void SetupMetadata::setActiveSubTab(int tab)
{
    d->displaySubTab->setCurrentIndex(tab);
}

void SetupMetadata::applySettings()
{
    MetaEngineSettings* const mSettings = MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    MetaEngineSettingsContainer set;

    set.rotationBehavior = MetaEngineSettingsContainer::RotateByInternalFlag;

    if (d->allowRotateByMetadata->isChecked())
    {
        set.rotationBehavior |= MetaEngineSettingsContainer::RotateByMetadataFlag;
    }

    if (d->rotateByContents->isChecked())
    {
        set.rotationBehavior |= MetaEngineSettingsContainer::RotateByLosslessRotation;

        if (d->allowLossyRotate->isChecked())
        {
            set.rotationBehavior |= MetaEngineSettingsContainer::RotateByLossyRotation;
        }
    }

    set.exifRotate            = d->exifRotateBox->isChecked();
    set.exifSetOrientation    = d->exifSetOrientationBox->isChecked();

    set.saveComments          = d->saveCommentsBox->isChecked();
    set.saveDateTime          = d->saveDateTimeBox->isChecked();
    set.savePickLabel         = d->savePickLabelBox->isChecked();
    set.saveColorLabel        = d->saveColorLabelBox->isChecked();
    set.saveRating            = d->saveRatingBox->isChecked();
    set.saveTags              = d->saveTagsBox->isChecked();
    set.saveTemplate          = d->saveTemplateBox->isChecked();
    set.saveFaceTags          = d->saveFaceTags->isChecked();
    set.savePosition          = d->savePosition->isChecked();

    set.useLazySync           = d->useLazySync->isChecked();
    set.writeDngFiles         = d->writeDngFilesBox->isChecked();
    set.writeRawFiles         = d->writeRawFilesBox->isChecked();
    set.useXMPSidecar4Reading = d->readXMPSidecarBox->isChecked();
    set.useCompatibleFileName = d->sidecarFileNameBox->isChecked();

    if (d->writeXMPSidecarBox->isChecked())
    {
        set.metadataWritingMode = (MetaEngine::MetadataWritingMode)
                                  d->writingModeCombo->itemData(d->writingModeCombo->currentIndex()).toInt();
    }
    else
    {
        set.metadataWritingMode = MetaEngine::WRITE_TO_FILE_ONLY;
    }

    set.updateFileTimeStamp   = d->updateFileTimeStampBox->isChecked();
    set.rescanImageIfModified = d->rescanImageIfModifiedBox->isChecked();
    set.clearMetadataIfRescan = d->clearMetadataIfRescanBox->isChecked();

    set.sidecarExtensions     = cleanUserFilterString(d->extensionsEdit->text(), true);
    set.sidecarExtensions.removeAll(QLatin1String("xmp"));
    set.sidecarExtensions.removeDuplicates();

    mSettings->setSettings(set);


#ifdef HAVE_KFILEMETADATA

    ApplicationSettings* const aSettings = ApplicationSettings::instance();

    if (!aSettings)
    {
        return;
    }

    aSettings->setSyncDigikamToBaloo(d->saveToBalooBox->isChecked());
    aSettings->setSyncBalooToDigikam(d->readFromBalooBox->isChecked());

    aSettings->saveSettings();

#endif // HAVE_KFILEMETADATA

    d->tagsCfgPanel->applySettings();

    d->advTab->applySettings();
}

void SetupMetadata::readSettings()
{
    MetaEngineSettings* const mSettings = MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    MetaEngineSettingsContainer set = mSettings->settings();

    if (set.rotationBehavior & MetaEngineSettingsContainer::RotatingPixels)
    {
        d->rotateByContents->setChecked(true);
    }
    else
    {
        d->rotateByFlag->setChecked(true);
    }

    d->allowRotateByMetadata->setChecked(set.rotationBehavior & MetaEngineSettingsContainer::RotateByMetadataFlag);
    d->allowLossyRotate->setChecked(set.rotationBehavior & MetaEngineSettingsContainer::RotateByLossyRotation);

    d->exifAutoRotateOriginal       = set.exifRotate;
    d->exifRotateBox->setChecked(d->exifAutoRotateOriginal);
    d->exifSetOrientationBox->setChecked(set.exifSetOrientation);

    d->saveTagsBox->setChecked(set.saveTags);
    d->saveCommentsBox->setChecked(set.saveComments);
    d->saveRatingBox->setChecked(set.saveRating);
    d->savePickLabelBox->setChecked(set.savePickLabel);
    d->saveColorLabelBox->setChecked(set.saveColorLabel);
    d->saveDateTimeBox->setChecked(set.saveDateTime);
    d->saveTemplateBox->setChecked(set.saveTemplate);
    d->saveFaceTags->setChecked(set.saveFaceTags);
    d->savePosition->setChecked(set.savePosition);

    d->useLazySync->setChecked(set.useLazySync);
    d->writeDngFilesBox->setChecked(set.writeDngFiles);
    d->writeRawFilesBox->setChecked(set.writeRawFiles);
    d->readXMPSidecarBox->setChecked(set.useXMPSidecar4Reading);
    d->sidecarFileNameBox->setChecked(set.useCompatibleFileName);
    d->updateFileTimeStampBox->setChecked(set.updateFileTimeStamp);
    d->rescanImageIfModifiedBox->setChecked(set.rescanImageIfModified);
    d->clearMetadataIfRescanBox->setChecked(set.clearMetadataIfRescan);

    if (set.metadataWritingMode == MetaEngine::WRITE_TO_FILE_ONLY)
    {
        d->writeXMPSidecarBox->setChecked(false);
    }
    else
    {
        d->writeXMPSidecarBox->setChecked(true);
        d->writingModeCombo->setCurrentIndex(d->writingModeCombo->findData(set.metadataWritingMode));
    }

    d->extensionsEdit->setText(set.sidecarExtensions.join(QLatin1Char(' ')));

#ifdef HAVE_KFILEMETADATA

    ApplicationSettings* const aSettings = ApplicationSettings::instance();

    if (!aSettings)
    {
        return;
    }

    d->saveToBalooBox->setChecked(aSettings->getSyncDigikamToBaloo());
    d->readFromBalooBox->setChecked(aSettings->getSyncBalooToDigikam());

#endif // HAVE_KFILEMETADATA

}

bool SetupMetadata::exifAutoRotateHasChanged() const
{
    return (d->exifAutoRotateOriginal != d->exifRotateBox->isChecked());
}

void SetupMetadata::slotExifAutoRotateToggled(bool b)
{
    // Show info if rotation was switched off, and only once.

    if (!b && !d->exifAutoRotateShowedInfo && exifAutoRotateHasChanged())
    {
        d->exifAutoRotateShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "Switching off exif auto rotation will most probably show "
                                       "your images in a wrong orientation, so only change this "
                                       "option if you explicitly require this."));
    }
}

void SetupMetadata::slotClearMetadataToggled(bool b)
{
    // Show info if delete metadata from the database was switched on, and only once.

    if (b && !d->clearMetadataShowedInfo)
    {
        d->clearMetadataShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "Switching on this option and your metadata has been written to the "
                                       "database only and not to the file or sidecar, you will be able to "
                                       "lose inserted metadata such as tags, keywords, or geographic "
                                       "coordinates."));
    }
}

void SetupMetadata::slotWriteRawFilesToggled(bool b)
{
    // Show info if write metadata to raw files was switched on

    if (b)
    {
        QApplication::beep();

        QPointer<QMessageBox> msgBox1 = new QMessageBox(QMessageBox::Warning,
                 qApp->applicationName(),
                 i18nc("@info",
                       "Do you really want to enable metadata writing to RAW files? "
                       "digiKam delegates this task to the Exiv2 library.\n"
                       "With different RAW formats, problems are known which can "
                       "lead to the destruction of RAW files.\n"
                       "If you decide to do so, make a backup of your RAW files. "
                       "We strongly recommend not to enable this option."),
                 QMessageBox::Yes | QMessageBox::No, this);

        msgBox1->button(QMessageBox::Yes)->setText(i18nc("@action", "Yes I understand"));
        msgBox1->setDefaultButton(QMessageBox::No);

        int result1 = msgBox1->exec();
        delete msgBox1;

        if (result1 == QMessageBox::Yes)
        {
            QPointer<QMessageBox> msgBox2 = new QMessageBox(QMessageBox::Warning,
                     qApp->applicationName(),
                     i18nc("@info", "You would rather disable writing metadata to RAW files?"),
                     QMessageBox::Yes | QMessageBox::No, this);

            int result2 = msgBox2->exec();
            delete msgBox2;

            if (result2 == QMessageBox::No)
            {
                return;
            }
        }

        d->writeRawFilesBox->setChecked(false);
    }
}

} // namespace Digikam
