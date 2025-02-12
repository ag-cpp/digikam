/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image file IO threaded interface.
 *
 * SPDX-FileCopyrightText: 2005-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "previewloadthread.h"

// Local includes

#include "iccmanager.h"
#include "previewtask.h"

namespace Digikam
{

PreviewLoadThread::PreviewLoadThread(QObject* const parent)
    : ManagedLoadSaveThread(parent)
{
      m_loadingPolicy = LoadingPolicyFirstRemovePrevious;
}

LoadingDescription PreviewLoadThread::createLoadingDescription(const QString& filePath, const PreviewSettings& settings, int size)
{
    return createLoadingDescription(filePath, settings, size, IccManager::displayProfile(m_displayingWidget));
}

LoadingDescription PreviewLoadThread::createLoadingDescription(const QString& filePath,
                                                               const PreviewSettings& previewSettings,
                                                               int size,
                                                               const IccProfile& displayProfile)
{
    LoadingDescription description(filePath, previewSettings, size);

    if (DImg::fileFormat(filePath) == DImg::RAW)
    {
        description.rawDecodingSettings.optimizeTimeLoading();
        description.rawDecodingSettings.rawPrm.sixteenBitsImage   = false;
        description.rawDecodingSettings.rawPrm.halfSizeColorImage = true;
        description.rawDecodingHint                               = LoadingDescription::RawDecodingTimeOptimized;
    }

    ICCSettingsContainer settings = IccSettings::instance()->settings();

    if (settings.enableCM && settings.useManagedPreviews)
    {
        description.postProcessingParameters.colorManagement = LoadingDescription::ConvertForDisplay;

        if (displayProfile.isNull())
        {
            description.postProcessingParameters.setProfile(IccProfile::sRGB());
        }
        else
        {
            description.postProcessingParameters.setProfile(displayProfile);
        }
    }

    return description;
}

void PreviewLoadThread::loadFast(const QString& filePath, int size)
{
    PreviewSettings settings(PreviewSettings::FastPreview);
    load(createLoadingDescription(filePath, settings, size));
}

void PreviewLoadThread::loadFastButLarge(const QString& filePath, int size)
{
    PreviewSettings settings(PreviewSettings::FastButLargePreview);
    load(createLoadingDescription(filePath, settings, size));
}

void PreviewLoadThread::loadHighQuality(const QString& filePath, PreviewSettings::RawLoading rawLoadingMode)
{
    PreviewSettings settings(PreviewSettings::HighQualityPreview, rawLoadingMode);
    load(createLoadingDescription(filePath, settings, 0));
}

void PreviewLoadThread::load(const QString& filePath, const PreviewSettings& settings, int size)
{
    load(createLoadingDescription(filePath, settings, size));
}

void PreviewLoadThread::load(const LoadingDescription& description)
{
    // creates a PreviewLoadingTask, which uses different mechanisms than a normal loading task

    ManagedLoadSaveThread::loadPreview(description, m_loadingPolicy);
}

void PreviewLoadThread::setDisplayingWidget(QWidget* const widget)
{
    m_displayingWidget = widget;
}

DImg PreviewLoadThread::loadFastSynchronously(const QString& filePath, int size, const IccProfile& profile)
{
    PreviewSettings settings(PreviewSettings::FastPreview);

    return loadSynchronously(createLoadingDescription(filePath, settings, size, profile));
}

DImg PreviewLoadThread::loadFastButLargeSynchronously(const QString& filePath, int minimumSize, const IccProfile& profile)
{
    PreviewSettings settings(PreviewSettings::FastButLargePreview);

    return loadSynchronously(createLoadingDescription(filePath, settings, minimumSize, profile));
}

DImg PreviewLoadThread::loadHighQualitySynchronously(const QString& filePath,
                                                     PreviewSettings::RawLoading rawLoadingMode,
                                                     const IccProfile& profile)
{
    PreviewSettings settings(PreviewSettings::HighQualityPreview, rawLoadingMode);

    return loadSynchronously(createLoadingDescription(filePath, settings, 0, profile));
}

DImg PreviewLoadThread::loadSynchronously(const QString& filePath, const PreviewSettings& previewSettings, int size, const IccProfile& profile)
{
    return loadSynchronously(createLoadingDescription(filePath, previewSettings, size, profile));
}

DImg PreviewLoadThread::loadSynchronously(const LoadingDescription& description)
{
    PreviewLoadingTask task(nullptr, description);
    task.execute();

    return task.img();
}

} // namespace Digikam

#include "moc_previewloadthread.cpp"
