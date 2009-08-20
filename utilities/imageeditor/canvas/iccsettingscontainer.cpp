/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-12-08
 * Description : ICC Settings Container.
 *
 * Copyright (C) 2005-2007 by F.J. Cruz <fj.cruz@supercable.es>
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iccsettingscontainer.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "icctransform.h"
#include "iccprofile.h"

namespace Digikam
{

ICCSettingsContainer::ICCSettingsContainer()
{
    // Note: by default, ICC color management is disabled.
    enableCM                      = false;

    defaultMismatchBehavior       = KeepProfile;
    defaultMissingProfileBehavior = KeepProfile;
    defaultUncalibratedBehavior   = ConvertToWorkspace;

    lastMismatchBehavior          = KeepProfile;
    lastMissingProfileBehavior    = KeepProfile;
    lastUncalibratedBehavior      = ConvertToWorkspace;

    useManagedView                = false;
    useBPC                        = true;
    renderingIntent               = IccTransform::Perceptual;
}

void ICCSettingsContainer::readFromConfig(KConfigGroup& group)
{
    enableCM             = group.readEntry("EnableCM", false);

    //if (!group.hasKey("OnProfileMismatch") && group.hasKey("BehaviourICC")) // legacy
      //  behavior = group.readEntry("BehaviourICC", false) ? "convert" : "ask";

    QString sRGB = IccProfile::sRGB().filePath();

    workspaceProfile     = group.readPathEntry("WorkProfileFile", sRGB);
    monitorProfile       = group.readPathEntry("MonitorProfileFile", sRGB);
    defaultInputProfile  = group.readPathEntry("InProfileFile", QString());
    defaultProofProfile  = group.readPathEntry("ProofProfileFile", QString());

    defaultMismatchBehavior       = (Behavior)group.readEntry("DefaultMismatchBehavior", (int)EmbeddedToWorkspace);
    defaultMissingProfileBehavior = (Behavior)group.readEntry("DefaultMissingProfileBehavior", (int)(UseSRGB|ConvertToWorkspace));
    defaultUncalibratedBehavior   = (Behavior)group.readEntry("DefaultUncalibratedBehavior", (int)InputToWorkspace);

    lastMismatchBehavior          = (Behavior)group.readEntry("LastMismatchBehavior", (int)EmbeddedToWorkspace);
    lastMissingProfileBehavior    = (Behavior)group.readEntry("LastMissingProfileBehavior", (int)(UseSRGB|ConvertToWorkspace));
    lastUncalibratedBehavior      = (Behavior)group.readEntry("LastUncalibratedBehavior", (int)InputToWorkspace);
    lastSpecifiedAssignProfile    = group.readEntry("LastSpecifiedAssignProfile", sRGB);
    lastSpecifiedInputProfile     = group.readEntry("LastSpecifiedInputProfile", defaultInputProfile);

    useBPC               = group.readEntry("BPCAlgorithm", true);
    useManagedView       = group.readEntry("ManagedView", false);
    renderingIntent      = group.readEntry("RenderingIntent", (int)IccTransform::Perceptual);
    iccFolder            = group.readEntry("DefaultPath", QString());
}

void ICCSettingsContainer::writeToConfig(KConfigGroup& group) const
{
    group.writeEntry("EnableCM", enableCM);

    if (!enableCM)
        return;          // No need to write settings in this case.

    group.writeEntry("DefaultMismatchBehavior", (int)defaultMismatchBehavior);
    group.writeEntry("DefaultMissingProfileBehavior", (int)defaultMissingProfileBehavior);
    group.writeEntry("DefaultUncalibratedBehavior", (int)defaultUncalibratedBehavior);

    group.writeEntry("LastMismatchBehavior", (int)lastMismatchBehavior);
    group.writeEntry("LastMissingProfileBehavior", (int)lastMissingProfileBehavior);
    group.writeEntry("LastUncalibratedBehavior", (int)lastUncalibratedBehavior);
    group.writeEntry("LastSpecifiedAssignProfile", lastSpecifiedAssignProfile);
    group.writeEntry("LastSpecifiedInputProfile", lastSpecifiedInputProfile);

    group.writeEntry("BPCAlgorithm", useBPC);
    group.writeEntry("ManagedView", useManagedView);
    group.writeEntry("RenderingIntent", renderingIntent);

    group.writePathEntry("WorkProfileFile", workspaceProfile);
    group.writePathEntry("MonitorProfileFile", monitorProfile);
    group.writePathEntry("InProfileFile", defaultInputProfile);
    group.writePathEntry("ProofProfileFile", defaultProofProfile);
    group.writeEntry("DefaultPath", iccFolder);
}

void ICCSettingsContainer::writeManagedViewToConfig(KConfigGroup& group) const
{
    // Save Color Managed View setting in config file. For performance
    // reason, no need to flush file, it cached in memory and will be flushed
    // to disk at end of session.
    group.writeEntry("ManagedView", useManagedView);
}

}  // namespace Digikam

