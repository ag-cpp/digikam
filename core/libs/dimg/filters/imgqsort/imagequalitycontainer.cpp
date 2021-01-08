/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image quality Settings Container.
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "imagequalitycontainer.h"

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

namespace Digikam
{

ImageQualityContainer::ImageQualityContainer()
    : enableSorter(false),
      detectBlur(true),
      detectNoise(true),
      detectCompression(true),
      detectExposure(true),
      lowQRejected(true),
      mediumQPending(true),
      highQAccepted(true),
      speed(1),
      rejectedThreshold(10),
      pendingThreshold(40),
      acceptedThreshold(60),
      blurWeight(100),
      noiseWeight(100),
      compressionWeight(100)
{
}

ImageQualityContainer::ImageQualityContainer(const ImageQualityContainer& other)
    : enableSorter(other.enableSorter),
      detectBlur(other.detectBlur),
      detectNoise(other.detectNoise),
      detectCompression(other.detectCompression),
      detectExposure(other.detectExposure),
      lowQRejected(other.lowQRejected),
      mediumQPending(other.mediumQPending),
      highQAccepted(other.highQAccepted),
      speed(other.speed),
      rejectedThreshold(other.rejectedThreshold),
      pendingThreshold(other.pendingThreshold),
      acceptedThreshold(other.acceptedThreshold),
      blurWeight(other.blurWeight),
      noiseWeight(other.noiseWeight),
      compressionWeight(other.compressionWeight)
{
}

ImageQualityContainer& ImageQualityContainer::operator=(const ImageQualityContainer& other)
{
    enableSorter       = other.enableSorter;
    detectBlur         = other.detectBlur;
    detectNoise        = other.detectNoise;
    detectCompression  = other.detectCompression;
    detectExposure     = other.detectExposure;
    lowQRejected       = other.lowQRejected;
    mediumQPending     = other.mediumQPending;
    highQAccepted      = other.highQAccepted;
    rejectedThreshold  = other.rejectedThreshold;
    pendingThreshold   = other.pendingThreshold;
    acceptedThreshold  = other.acceptedThreshold;
    blurWeight         = other.blurWeight;
    noiseWeight        = other.noiseWeight;
    compressionWeight  = other.compressionWeight;
    speed              = other.speed;

    return *this;
}

ImageQualityContainer::~ImageQualityContainer()
{
}

void ImageQualityContainer::readFromConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Image Quality Settings");

    enableSorter              = group.readEntry("Enable Sorter",      false);
    detectBlur                = group.readEntry("Detect Blur",        true);
    detectNoise               = group.readEntry("Detect Noise",       true);
    detectCompression         = group.readEntry("Detect Compression", true);
    detectExposure            = group.readEntry("Detect Exposure",    true);
    lowQRejected              = group.readEntry("LowQ Rejected",      true);
    mediumQPending            = group.readEntry("MediumQ Pending",    true);
    highQAccepted             = group.readEntry("HighQ Accepted",     true);
    speed                     = group.readEntry("Speed",              1);
    rejectedThreshold         = group.readEntry("Rejected Threshold", 10);
    pendingThreshold          = group.readEntry("Pending Threshold",  40);
    acceptedThreshold         = group.readEntry("Accepted Threshold", 60);
    blurWeight                = group.readEntry("Blur Weight",        100);
    noiseWeight               = group.readEntry("Noise Weight",       100);
    compressionWeight         = group.readEntry("Compression Weight", 100);
}

void ImageQualityContainer::writeToConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group("Image Quality Settings");

    group.writeEntry("Enable Sorter",       enableSorter);
    group.writeEntry("Detect Blur",         detectBlur);
    group.writeEntry("Detect Noise",        detectNoise);
    group.writeEntry("Detect Compression",  detectCompression);
    group.writeEntry("Detect Exposure",     detectExposure);
    group.writeEntry("LowQ Rejected",       lowQRejected);
    group.writeEntry("MediumQ Pending",     mediumQPending);
    group.writeEntry("HighQ Accepted",      highQAccepted);
    group.writeEntry("Speed",               speed);
    group.writeEntry("Rejected Threshold",  rejectedThreshold);
    group.writeEntry("Pending Threshold",   pendingThreshold);
    group.writeEntry("Accepted Threshold",  acceptedThreshold);
    group.writeEntry("Blur Weight",         blurWeight);
    group.writeEntry("Noise Weight",        noiseWeight);
    group.writeEntry("Compression Weight",  compressionWeight);
}

QDebug operator<<(QDebug dbg, const ImageQualityContainer& s)
{
    dbg.nospace() << endl;
    dbg.nospace() << "EnableSorter       :" << s.enableSorter       << endl;
    dbg.nospace() << "DetectBlur         :" << s.detectBlur         << endl;
    dbg.nospace() << "DetectNoise        :" << s.detectNoise        << endl;
    dbg.nospace() << "DetectCompression  :" << s.detectCompression  << endl;
    dbg.nospace() << "DetectExposure     :" << s.detectExposure     << endl;
    dbg.nospace() << "LowQRejected       :" << s.lowQRejected       << endl;
    dbg.nospace() << "MediumQPending     :" << s.mediumQPending     << endl;
    dbg.nospace() << "HighQAccepted      :" << s.highQAccepted      << endl;
    dbg.nospace() << "Speed              :" << s.speed              << endl;
    dbg.nospace() << "Rejected Threshold :" << s.rejectedThreshold  << endl;
    dbg.nospace() << "Pending Threshold  :" << s.pendingThreshold   << endl;
    dbg.nospace() << "Accepted Threshold :" << s.acceptedThreshold  << endl;
    dbg.nospace() << "Blur Weight        :" << s.blurWeight         << endl;
    dbg.nospace() << "Noise Weight       :" << s.noiseWeight        << endl;
    dbg.nospace() << "Compression Weight :" << s.compressionWeight  << endl;

    return dbg.space();
}

} // namespace Digikam
