/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-19
 * Description : a tool for color space conversion
 *
 * SPDX-FileCopyrightText: 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "profileconversiontool.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "editortoolsettings.h"
#include "iccprofileinfodlg.h"
#include "iccprofilesettings.h"
#include "iccsettings.h"
#include "iccsettingscontainer.h"
#include "icctransform.h"
#include "icctransformfilter.h"
#include "histogramwidget.h"
#include "imageiface.h"
#include "imageregionwidget.h"
#include "dmetadata.h"

namespace DigikamEditorProfileConversionToolPlugin
{

class Q_DECL_HIDDEN ProfileConversionTool::Private
{
public:

    Private() = default;

    const QString configGroupName       = QLatin1String("Profile Conversion Tool");
    const QString configProfileEntry    = QLatin1String("Profile");

    IccProfilesSettings* profilesBox    = nullptr;

    ImageRegionWidget*   previewWidget  = nullptr;
    EditorToolSettings*  gboxSettings   = nullptr;

    IccProfile           currentProfile;

    IccTransform         transform;

public:

    static IccTransform getTransform(const IccProfile& in, const IccProfile& out);
};

IccTransform ProfileConversionTool::Private::getTransform(const IccProfile& in, const IccProfile& out)
{
    ICCSettingsContainer settings = IccSettings::instance()->settings();

    IccTransform transform;
    transform.setIntent(settings.renderingIntent);
    transform.setUseBlackPointCompensation(settings.useBPC);

    transform.setInputProfile(in);
    transform.setOutputProfile(out);

    return transform;
}

// ----------------------------------------------------------------------------

ProfileConversionTool::ProfileConversionTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("profile conversion"));

    // -------------------------------------------------------------

    ImageIface iface;
    d->currentProfile = iface.originalIccProfile();

    d->gboxSettings   = new EditorToolSettings(nullptr);
    d->gboxSettings->setButtons(EditorToolSettings::Ok|
                                EditorToolSettings::Cancel);

    d->gboxSettings->setTools(EditorToolSettings::Histogram);
    d->gboxSettings->setHistogramType(LRGBAC);

    // -------------------------------------------------------------

    QGridLayout* grid            = new QGridLayout(d->gboxSettings->plainPage());
    QLabel* currentProfileTitle  = new QLabel;
    QLabel* currentProfileDesc   = new QLabel;
    QPushButton* currentProfInfo = new QPushButton(i18n("Info..."));
    d->profilesBox               = new IccProfilesSettings;

    currentProfileTitle->setText(i18n("Current Color Space:"));
    currentProfileDesc->setText(QString::fromUtf8("<b>%1</b>").arg(d->currentProfile.description()));
    currentProfileDesc->setWordWrap(true);

    const int spacing = d->gboxSettings->spacingHint();

    grid->addWidget(currentProfileTitle, 0, 0, 1, 5);
    grid->addWidget(currentProfileDesc,  1, 0, 1, 5);
    grid->addWidget(currentProfInfo,     2, 0, 1, 1);
    grid->addWidget(d->profilesBox,      3, 0, 1, 5);
    grid->setRowStretch(4, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // -------------------------------------------------------------

    d->previewWidget = new ImageRegionWidget;

    setToolSettings(d->gboxSettings);
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::AllPreviewModes);

    // -------------------------------------------------------------

    connect(currentProfInfo, SIGNAL(clicked()),
            this, SLOT(slotCurrentProfInfo()));

    connect(d->profilesBox, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotProfileChanged()));
}

ProfileConversionTool::~ProfileConversionTool()
{
    delete d;
}

void ProfileConversionTool::slotCurrentProfInfo()
{
    ICCProfileInfoDlg infoDlg(qApp->activeWindow(), QString(), d->currentProfile);
    infoDlg.exec();
}

void ProfileConversionTool::slotProfileChanged()
{
    d->gboxSettings->enableButton(EditorToolSettings::Ok, !d->profilesBox->currentProfile().isNull());
    updateTransform();
    slotTimer();
}

void ProfileConversionTool::updateTransform()
{
    d->transform = d->getTransform(d->currentProfile, d->profilesBox->currentProfile());
}

void ProfileConversionTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->profilesBox->setCurrentProfile(IccProfile(group.readPathEntry(d->configProfileEntry, d->currentProfile.filePath())));
    d->profilesBox->readSettings(group);
}

void ProfileConversionTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    group.writePathEntry(d->configProfileEntry, d->profilesBox->currentProfile().filePath());
    d->profilesBox->writeSettings(group);
    config->sync();
}

void ProfileConversionTool::slotResetSettings()
{
    d->profilesBox->resetToDefault();
}

void ProfileConversionTool::preparePreview()
{
    DImg preview = d->previewWidget->getOriginalRegionImage(true);
    setFilter(new IccTransformFilter(&preview, this, d->transform));
}

void ProfileConversionTool::setPreviewImage()
{
    DImg preview = filter()->getTargetImage();
    d->previewWidget->setPreviewImage(preview);

    // Update histogram.

    d->gboxSettings->histogramBox()->histogram()->updateData(preview.copy(), DImg(), false);
}

void ProfileConversionTool::prepareFinal()
{
    ImageIface iface;
    setFilter(new IccTransformFilter(iface.original(), this, d->transform));
}

void ProfileConversionTool::setFinalImage()
{
    ImageIface iface;
    DImg imDest = filter()->getTargetImage();

    iface.setOriginal(i18n("Color Profile Conversion"), filter()->filterAction(), imDest);
    iface.setOriginalIccProfile(imDest.getIccProfile());

    QScopedPointer<DMetadata> meta(new DMetadata(iface.originalMetadata()));
    meta->removeExifColorSpace();
    iface.setOriginalMetadata(meta->data());
}

// Static Methods.

QStringList ProfileConversionTool::favoriteProfiles()
{
    Private d;
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d.configGroupName);
    return IccProfilesSettings::favoriteProfiles(group);
}

void ProfileConversionTool::fastConversion(const IccProfile& profile)
{
    ImageIface iface;

    IccProfile currentProfile = iface.originalIccProfile();
    IccTransform transform    = Private::getTransform(currentProfile, profile);
    IccTransformFilter filter(iface.original(), nullptr, transform);
    filter.startFilterDirectly();

    DImg imDest               = filter.getTargetImage();
    iface.setOriginal(i18n("Color Profile Conversion"), filter.filterAction(), imDest);
    iface.setOriginalIccProfile(imDest.getIccProfile());

    QScopedPointer<DMetadata> meta(new DMetadata(iface.originalMetadata()));
    meta->removeExifColorSpace();
    iface.setOriginalMetadata(meta->data());
}

} // namespace DigikamEditorProfileConversionToolPlugin

#include "moc_profileconversiontool.cpp"
