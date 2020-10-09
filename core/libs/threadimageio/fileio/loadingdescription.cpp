/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-03
 * Description : Loading parameters for multithreaded loading
 *
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "loadingdescription.h"

// Qt includes

#include <QUrl>

// Local includes

#include "icctransform.h"
#include "thumbnailinfo.h"
#include "thumbnailsize.h"

namespace Digikam
{

LoadingDescription::PreviewParameters::PreviewParameters()
    : type (NoPreview),
      size (0),
      flags(NoFlags)
{
}

bool LoadingDescription::PreviewParameters::operator==(const PreviewParameters& other) const
{
    return ((type             == other.type)            &&
            (size             == other.size)            &&
            (flags            == other.flags)           &&
            (previewSettings  == other.previewSettings) &&
            (extraParameter   == other.extraParameter)  &&
            (storageReference == other.storageReference));
}

bool LoadingDescription::PreviewParameters::onlyPregenerate() const
{
    return (flags & OnlyPregenerate);
}

bool LoadingDescription::PostProcessingParameters::operator==(const PostProcessingParameters& other) const
{
    return (colorManagement == other.colorManagement);
}

bool LoadingDescription::PostProcessingParameters::needsProcessing() const
{
    return (colorManagement != NoColorConversion);
}

void LoadingDescription::PostProcessingParameters::setTransform(const IccTransform& transform)
{
    iccData = QVariant::fromValue<IccTransform>(transform);
}

bool LoadingDescription::PostProcessingParameters::hasTransform() const
{
    return (!iccData.isNull() && iccData.canConvert<IccTransform>());
}

IccTransform LoadingDescription::PostProcessingParameters::transform() const
{
    return iccData.value<IccTransform>();
}

void LoadingDescription::PostProcessingParameters::setProfile(const IccProfile& profile)
{
    iccData = QVariant::fromValue<IccProfile>(profile);
}

bool LoadingDescription::PostProcessingParameters::hasProfile() const
{
    return (!iccData.isNull() && iccData.canConvert<IccProfile>());
}

IccProfile LoadingDescription::PostProcessingParameters::profile() const
{
    return iccData.value<IccProfile>();
}

// ----------------------------------------------------------------------------

LoadingDescription::LoadingDescription()
    : filePath                  (QString()),
      rawDecodingSettings       (DRawDecoding()),
      rawDecodingHint           (RawDecodingDefaultSettings),
      previewParameters         (PreviewParameters()),
      postProcessingParameters  (PostProcessingParameters())
{
}

LoadingDescription::LoadingDescription(const QString& filePath,
                                       ColorManagementSettings cm)
    : filePath              (filePath),
      rawDecodingSettings   (DRawDecoding()),
      rawDecodingHint       (RawDecodingDefaultSettings),
      previewParameters     (PreviewParameters())
{
      postProcessingParameters.colorManagement = cm;
}

LoadingDescription::LoadingDescription(const QString& filePath,
                                       const DRawDecoding& settings,
                                       RawDecodingHint hint,
                                       ColorManagementSettings cm)
    : filePath              (filePath),
      rawDecodingSettings   (settings),
      rawDecodingHint       (hint),
      previewParameters     (PreviewParameters())
{
      postProcessingParameters.colorManagement = cm;
}

LoadingDescription::LoadingDescription(const QString& filePath,
                                       const PreviewSettings& previewSettings,
                                       int size,
                                       ColorManagementSettings cm,
                                       LoadingDescription::PreviewParameters::PreviewType type)
    : filePath(filePath),
      rawDecodingSettings(DRawDecoding()),
      rawDecodingHint(RawDecodingDefaultSettings)
{
    previewParameters.type                   = type;
    previewParameters.size                   = size;
    previewParameters.previewSettings        = previewSettings;
    postProcessingParameters.colorManagement = cm;
}

QString LoadingDescription::cacheKey() const
{
    // Here we have the knowledge which LoadingDescriptions / RawFileDecodingSettings
    // must be cached separately.

    // Thumbnail loading. This one is easy.

    QString curlPath = QUrl(filePath).toString(QUrl::FullyEncoded);

    if      (previewParameters.type == PreviewParameters::Thumbnail)
    {
        QString fileRef = curlPath.isEmpty() ? (QLatin1String("id:/") + previewParameters.storageReference.toString())
                                             : curlPath;

        return (fileRef + QLatin1String("-thumbnail-") + QString::number(previewParameters.size));
    }
    else if (previewParameters.type == PreviewParameters::DetailThumbnail)
    {
        QString fileRef    = curlPath.isEmpty() ? (QLatin1String("id:/") + previewParameters.storageReference.toString())
                                                : curlPath;
        QRect rect         =  previewParameters.extraParameter.toRect();
        QString rectString = QString::fromLatin1("%1,%2-%3x%4-")
                             .arg(rect.x())
                             .arg(rect.y())
                             .arg(rect.width())
                             .arg(rect.height());

        return (fileRef + QLatin1String("-thumbnail-") + rectString + QString::number(previewParameters.size));
    }

    // DImg loading

    if (previewParameters.type == PreviewParameters::NoPreview)
    {
        // Assumption: Full loading. For Raw images, we need to check all parameters here.
        // Non-raw images will always be loaded full-size.
        // NOTE: do not identify these by cache key only, check the settings!

        if      (rawDecodingHint == RawDecodingGlobalSettings)
        {
            return (curlPath + QLatin1String("-globalraw"));
        }
        else if (rawDecodingHint == RawDecodingCustomSettings)
        {
            return (curlPath + QLatin1String("-customraw"));
        }
    }
    else
    {
        // Assumption: Size-limited previews are always eight bit and do not care for raw settings.

        if (previewParameters.size)
        {
            return (curlPath + QLatin1String("-previewImage-") + QString::number(previewParameters.size));
        }
        else
        {
            return (curlPath + QLatin1String("-previewImage"));
        }
    }

    QString suffix;

    // Assumption: Time-optimized loading is used for previews and non-previews

    if (rawDecodingHint == RawDecodingTimeOptimized)
    {
        // Assumption: With time-optimized, we can have 8 or 16bit and halfSize or demosaiced.

        suffix += QLatin1String("-timeoptimized");

        if (!rawDecodingSettings.rawPrm.sixteenBitsImage)
        {
            suffix += QLatin1String("-8");
        }

        if (rawDecodingSettings.rawPrm.halfSizeColorImage)
        {
            suffix += QLatin1String("-halfSize");
        }
    }

    return (curlPath + suffix);
}

QStringList LoadingDescription::lookupCacheKeys() const
{
    // Build a hierarchy which cache entries may be used for this LoadingDescription.

    // Thumbnail loading. No other cache key included!

    if ((previewParameters.type == PreviewParameters::Thumbnail) ||
        (previewParameters.type == PreviewParameters::DetailThumbnail))
    {
        return QStringList() << cacheKey();
    }

    // DImg loading.
    // Typically, the first is the best. An actual loading operation may use a
    // lower-quality loading and will effectively only add the last entry of the
    // list to the cache, although it can accept the first if already available.
    // Hierarchy:
    //  Raw with GlobalSettings and CustomSettings
    //  Raw with optimized loading, 8 or 16bit
    //      full size
    //      halfSize
    //  "Normal" image (default raw parameters)
    //  Preview image
    //      full size
    //      reduced size

    QStringList cacheKeys;

    QString curlPath = QUrl(filePath).toString(QUrl::FullyEncoded);

    if (previewParameters.type != PreviewParameters::NoPreview)
    {
        if (previewParameters.size)
        {
            cacheKeys << curlPath + QLatin1String("-previewImage-") + QString::number(previewParameters.size);
        }

        // full size preview

        cacheKeys << curlPath + QLatin1String("-previewImage");
    }

    if (rawDecodingHint == RawDecodingDefaultSettings)
    {
        cacheKeys << curlPath;
    }

    if (rawDecodingHint == RawDecodingTimeOptimized)
    {
        if (rawDecodingSettings.rawPrm.sixteenBitsImage)
        {
            cacheKeys << curlPath + QLatin1String("-timeoptimized");

            if (rawDecodingSettings.rawPrm.halfSizeColorImage)
            {
                cacheKeys << curlPath + QLatin1String("-timeoptimized-halfSize");
            }
        }
        else
        {
            cacheKeys << curlPath + QLatin1String("-timeoptimized-8");

            if (rawDecodingSettings.rawPrm.halfSizeColorImage)
            {
                cacheKeys << curlPath + QLatin1String("-timeoptimized-8-halfSize");
            }
        }
    }

    if      (rawDecodingHint == RawDecodingGlobalSettings)
    {
        cacheKeys << curlPath + QLatin1String("-globalraw");
    }
    else if (rawDecodingHint == RawDecodingCustomSettings)
    {
        cacheKeys << curlPath + QLatin1String("-customraw");
    }

    return cacheKeys;
}

bool LoadingDescription::needCheckRawDecoding() const
{
    return ((rawDecodingHint == RawDecodingGlobalSettings) ||
            (rawDecodingHint == RawDecodingCustomSettings));
}

bool LoadingDescription::isReducedVersion() const
{
    // return true if this loads anything but the full version

    return (rawDecodingSettings.rawPrm.halfSizeColorImage ||
            (previewParameters.type != PreviewParameters::NoPreview));
}

bool LoadingDescription::operator==(const LoadingDescription& other) const
{
    return ((filePath                 == other.filePath)                   &&
            (rawDecodingSettings      == other.rawDecodingSettings)        &&
            (previewParameters        == other.previewParameters)          &&
            (postProcessingParameters == other.postProcessingParameters));
}

bool LoadingDescription::operator!=(const LoadingDescription& other) const
{
    return (!operator==(other));
}

bool LoadingDescription::equalsIgnoreReducedVersion(const LoadingDescription& other) const
{
    return (filePath == other.filePath);
}

bool LoadingDescription::equalsOrBetterThan(const LoadingDescription& other) const
{
    // This method is similar to operator==. But it returns true as well if this
    // loads a "better" version than <other>.
    // Preview parameters must have the same size, or other has no size restriction.
    // Comparing raw decoding settings is complicated. We allow to be loaded with optimizeTimeLoading().

    DRawDecoding fast = rawDecodingSettings;
    fast.optimizeTimeLoading();

    return (
            (filePath == other.filePath) &&
            (
                (rawDecodingSettings == other.rawDecodingSettings) ||
                (fast                == other.rawDecodingSettings)
            ) &&
            (
                (previewParameters.size == other.previewParameters.size) ||
                other.previewParameters.size
            )
           );
}

bool LoadingDescription::isThumbnail() const
{
    return ((previewParameters.type == PreviewParameters::Thumbnail) ||
            (previewParameters.type == PreviewParameters::DetailThumbnail));
}

bool LoadingDescription::isPreviewImage() const
{
    return (previewParameters.type == PreviewParameters::PreviewImage);
}

ThumbnailIdentifier LoadingDescription::thumbnailIdentifier() const
{
    ThumbnailIdentifier id;

    if (!isThumbnail())
    {
        return id;
    }

    id.filePath = filePath;
    id.id       = previewParameters.storageReference.toLongLong();

    return id;
}

QStringList LoadingDescription::possibleCacheKeys(const QString& filePath)
{
    QStringList keys;
    QString curlPath = QUrl(filePath).toString(QUrl::FullyEncoded);

    keys << curlPath ;
    keys << curlPath + QLatin1String("-timeoptimized-8-halfSize");
    keys << curlPath + QLatin1String("-timeoptimized-8");
    keys << curlPath + QLatin1String("-timeoptimized-halfSize");
    keys << curlPath + QLatin1String("-timeoptimized");
    keys << curlPath + QLatin1String("-customraw");
    keys << curlPath + QLatin1String("-globalraw");

    for (int i = 1 ; i <= ThumbnailSize::HD ; ++i)
    {
        keys << curlPath + QLatin1String("-previewImage-") + QString::number(i);
    }

    return keys;
}

QStringList LoadingDescription::possibleThumbnailCacheKeys(const QString& filePath)
{
    // FIXME: With details, there is an endless number of possible cache keys. Need different approach.

    QStringList keys;
    QString curlPath = QUrl(filePath).toString(QUrl::FullyEncoded);

    // there are (ThumbnailSize::HD) possible keys...

    QString path = curlPath + QLatin1String("-thumbnail-");

    for (int i = 1 ; i <= ThumbnailSize::HD ; ++i)
    {
        keys << path + QString::number(i);
    }

    return keys;
}

} // namespace Digikam
