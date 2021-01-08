/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-20
 * Description : MetaEngine Settings Container.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_META_ENGINE_SETTINGS_CONTAINER_H
#define DIGIKAM_META_ENGINE_SETTINGS_CONTAINER_H

// Qt includes

#include <QFlags>
#include <QDebug>

// Local includes

#include "digikam_export.h"
#include "metaengine.h"

class QStringList;

class KConfigGroup;

namespace Digikam
{

/**
 * The class MetaEngineSettingsContainer encapsulates all metadata related settings.
 * NOTE: this allows supply changed arguments to MetadataHub without changing the global settings.
 */
class DIGIKAM_EXPORT MetaEngineSettingsContainer
{
public:

    /**
     * Describes the allowed and desired operation when rotating a picture.
     * The modes are in escalating order and describe if an operation is allowed.
     * What is actually done will be governed by what is possible:
     * 1) RAW files cannot by rotated by content, setting the metadata may be problematic
     * 2) Read-Only files cannot edited, neither content nor metadata
     * 3) Writable files will have lossy compression
     * 4) Only JPEG and PGF offer lossless rotation
     * Using a contents-based rotation always implies resetting the flag.
     */
    enum RotationBehaviorFlag
    {
        NoRotation               = 0,
        RotateByInternalFlag     = 1 << 0,
        RotateByMetadataFlag     = 1 << 1,
        RotateByLosslessRotation = 1 << 2,
        RotateByLossyRotation    = 1 << 3,

        RotatingFlags            = RotateByInternalFlag     | RotateByMetadataFlag,
        RotatingPixels           = RotateByLosslessRotation | RotateByLossyRotation
    };
    Q_DECLARE_FLAGS(RotationBehaviorFlags, RotationBehaviorFlag)

public:

    explicit MetaEngineSettingsContainer();
    ~MetaEngineSettingsContainer();

public:

    void readFromConfig(KConfigGroup& group);
    void writeToConfig(KConfigGroup& group) const;

public:

    bool                            exifRotate;
    bool                            exifSetOrientation;

    bool                            saveComments;
    bool                            saveDateTime;
    bool                            savePickLabel;
    bool                            saveColorLabel;
    bool                            saveRating;

    bool                            saveTemplate;
    bool                            saveTags;
    bool                            saveFaceTags;
    bool                            savePosition;

    bool                            writeRawFiles;
    bool                            writeDngFiles;
    bool                            updateFileTimeStamp;
    bool                            rescanImageIfModified;
    bool                            clearMetadataIfRescan;
    bool                            useXMPSidecar4Reading;
    bool                            useCompatibleFileName;
    bool                            useLazySync;

    MetaEngine::MetadataWritingMode metadataWritingMode;

    RotationBehaviorFlags           rotationBehavior;

    QStringList                     sidecarExtensions;
};

//! qDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const MetaEngineSettingsContainer& inf);

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::MetaEngineSettingsContainer::RotationBehaviorFlags)

#endif // DIGIKAM_META_ENGINE_SETTINGS_CONTAINER_H
